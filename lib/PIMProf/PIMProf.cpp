/*
 * PIMProf.cpp
 *
 *  Created on: Oct 2018
 *      Author: Yizhou Wei
 */


#include "SABER/SrcSnkDDA.h"
#include "MSSA/SVFGStat.h"
#include "Util/GraphUtil.h"

using namespace llvm;

static cl::opt<bool> DumpSlice("dump-slice", cl::init(false),
                               cl::desc("Dump dot graph of Saber Slices"));

static cl::opt<unsigned> cxtLimit("cxtlimit",  cl::init(3),
                                  cl::desc("Source-Sink Analysis Contexts Limit"));

void SrcSnkDDA::analyze(SVFModule module) {

    initialize(module);

    ContextCond::setMaxCxtLen(cxtLimit);

    for (SVFGNodeSetIter iter = sourcesBegin(), eiter = sourcesEnd();
            iter != eiter; ++iter) {
        setCurSlice(*iter);

        DBOUT(DGENERAL, outs() << "Analysing slice:" << (*iter)->getId() << ")\n");
        ContextCond cxt;
        DPIm item((*iter)->getId(),cxt);
        forwardTraverse(item);

        /// do not consider there is bug when reaching a global SVFGNode
        /// if we touch a global, then we assume the client uses this memory until the program exits.
        if (getCurSlice()->isReachGlobal()) {
            DBOUT(DSaber, outs() << "Forward analysis reaches globals for slice:" << (*iter)->getId() << ")\n");
        }
        else {
            DBOUT(DSaber, outs() << "Forward process for slice:" << (*iter)->getId() << " (size = " << getCurSlice()->getForwardSliceSize() << ")\n");

            for (SVFGNodeSetIter sit = getCurSlice()->sinksBegin(), esit =
                        getCurSlice()->sinksEnd(); sit != esit; ++sit) {
                ContextCond cxt;
                DPIm item((*sit)->getId(),cxt);
                backwardTraverse(item);
            }

            DBOUT(DSaber, outs() << "Backward process for slice:" << (*iter)->getId() << " (size = " << getCurSlice()->getBackwardSliceSize() << ")\n");

            AllPathReachability();

            DBOUT(DSaber, outs() << "Guard computation for slice:" << (*iter)->getId() << ")\n");
        }

        reportBug(getCurSlice());
    }

    finalize();
}


/*!
 * Propagate information forward by matching context
 */
void SrcSnkDDA::forwardpropagate(const DPIm& item, SVFGEdge* edge) {
    DBOUT(DSaber,outs() << "\n##processing source: " << getCurSlice()->getSource()->getId() <<" forward propagate from (" << edge->getSrcID());

    // for indirect SVFGEdge, the propagation should follow the def-use chains
    // points-to on the edge indicate whether the object of source node can be propagated

    const SVFGNode* dstNode = edge->getDstNode();
    DPIm newItem(dstNode->getId(),item.getContexts());

    /// handle globals here
    if(isGlobalSVFGNode(dstNode) || getCurSlice()->isReachGlobal()) {
        getCurSlice()->setReachGlobal();
        return;
    }


    /// perform context sensitive reachability
    // push context for calling
    if (edge->isCallVFGEdge()) {
        CallSiteID csId = 0;
        if(const CallDirSVFGEdge* callEdge = dyn_cast<CallDirSVFGEdge>(edge))
            csId = callEdge->getCallSiteId();
        else
            csId = cast<CallIndSVFGEdge>(edge)->getCallSiteId();

        newItem.pushContext(csId);
        DBOUT(DSaber, outs() << " push cxt [" << csId << "] ");
    }
    // match context for return
    else if (edge->isRetVFGEdge()) {
        CallSiteID csId = 0;
        if(const RetDirSVFGEdge* callEdge = dyn_cast<RetDirSVFGEdge>(edge))
            csId = callEdge->getCallSiteId();
        else
            csId = cast<RetIndSVFGEdge>(edge)->getCallSiteId();

        if (newItem.matchContext(csId) == false) {
            DBOUT(DSaber, outs() << "-|-\n");
            return;
        }
        DBOUT(DSaber, outs() << " pop cxt [" << csId << "] ");
    }

    /// whether this dstNode has been visited or not
    if(forwardVisited(dstNode,newItem)) {
        DBOUT(DSaber,outs() << " node "<< dstNode->getId() <<" has been visited\n");
        return;
    }
    else
        addForwardVisited(dstNode, newItem);

    if(pushIntoWorklist(newItem))
        DBOUT(DSaber,outs() << " --> " << edge->getDstID() << ", cxt size: " << newItem.getContexts().cxtSize() <<")\n");

}

/*!
 * Propagate information backward without matching context, as forward analysis already did it
 */
void SrcSnkDDA::backwardpropagate(const DPIm& item, SVFGEdge* edge) {
    DBOUT(DSaber,outs() << "backward propagate from (" << edge->getDstID() << " --> " << edge->getSrcID() << ")\n");
    const SVFGNode* srcNode = edge->getSrcNode();
    if(backwardVisited(srcNode))
        return;
    else
        addBackwardVisited(srcNode);

    ContextCond cxt;
    DPIm newItem(srcNode->getId(), cxt);
    pushIntoWorklist(newItem);
}

/// Guarded reachability search
void SrcSnkDDA::AllPathReachability() {
    /// annotate SVFG with slice information for debugging purpose
    if(DumpSlice)
        annotateSlice(_curSlice);

    _curSlice->AllPathReachableSolve();

    if(isSatisfiableForAll(_curSlice)== true)
        _curSlice->setAllReachable();
}

/// Set current slice
void SrcSnkDDA::setCurSlice(const SVFGNode* src) {
    if(_curSlice!=NULL) {
        delete _curSlice;
        _curSlice = NULL;
        clearVisitedMap();
    }

    _curSlice = new ProgSlice(src,getPathAllocator(), getSVFG());
}

void SrcSnkDDA::annotateSlice(ProgSlice* slice) {
    getSVFG()->getStat()->addToSources(slice->getSource());
    for(SVFGNodeSetIter it = slice->sinksBegin(), eit = slice->sinksEnd(); it!=eit; ++it )
        getSVFG()->getStat()->addToSinks(*it);
    for(SVFGNodeSetIter it = slice->forwardSliceBegin(), eit = slice->forwardSliceEnd(); it!=eit; ++it )
        getSVFG()->getStat()->addToForwardSlice(*it);
    for(SVFGNodeSetIter it = slice->backwardSliceBegin(), eit = slice->backwardSliceEnd(); it!=eit; ++it )
        getSVFG()->getStat()->addToBackwardSlice(*it);
}

void SrcSnkDDA::dumpSlices() {

    if(DumpSlice)
        const_cast<SVFG*>(getSVFG())->dump("Slice",true);
}

void SrcSnkDDA::printBDDStat() {

    outs() << "BDD Mem usage: " << PathCondAllocator::getMemUsage() << "\n";
    outs() << "BDD Number: " << PathCondAllocator::getCondNum() << "\n";
    outs() << "BDD max live number: " << PathCondAllocator::getMaxLiveCondNumber() << "\n";
}
