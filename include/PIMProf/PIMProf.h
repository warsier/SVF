
/*
 * PIMProf.h
 *
 *  Created on: Oct 2018
 *      Author: Yizhou Wei
 */

#ifndef PIMPROF_H_
#define PIMPROF_H_

#include "SABER/CFLSolver.h"
#include "MSSA/SVFGOPT.h"
#include "SABER/ProgSlice.h"
#include "SABER/SaberSVFGBuilder.h"
#include "WPA/Andersen.h"

typedef CFLSolver<SVFG*,CxtDPItem> CFLSrcSnkSolver;

class PIMProf : public CFLSrcSnkSolver {
public:
    typedef ProgSlice::SVFGNodeSet SVFGNodeSet;
    typedef SVFGNodeSet::iterator SVFGNodeSetIter;
private:
    ProgSlice* _curSlice;		/// current program slice
    SVFGNodeSet sources;		/// source nodes
    SaberSVFGBuilder memSSA;
    SVFG* svfg;
    PathCondAllocator* pathCondAllocator;
    PTACallGraph* ptaCallGraph;
public:
    /// Constructor
    PIMProf() : _curSlice(NULL), svfg(NULL), ptaCallGraph(NULL) {
        pathCondAllocator = new PathCondAllocator();
    }
    /// Destructor
    virtual ~PIMProf() {
        if (svfg != NULL)
            delete svfg;
        svfg = NULL;

        if (ptaCallGraph != NULL)
            delete ptaCallGraph;
        ptaCallGraph = NULL;

        //if(pathCondAllocator)
        //    delete pathCondAllocator;
        //pathCondAllocator = NULL;
    }

    /// Start analysis here
    virtual void analyze(SVFModule module);

    /// Initialize analysis
    virtual void initialize(SVFModule module) {
        ptaCallGraph = new PTACallGraph(module);
        AndersenWaveDiff* ander = AndersenWaveDiff::createAndersenWaveDiff(module);
        svfg = memSSA.buildSVFG(ander);
        setGraph(memSSA.getSVFG());
    }

    /// Finalize analysis
    virtual void finalize() {
        // dumpSlices();
    }

    /// Get SVFG
    inline const SVFG* getSVFG() const {
        return graph();
    }

    /// Slice operations
    //@{
    void setCurSlice(const SVFGNode* src);

    inline ProgSlice* getCurSlice() const {
        return _curSlice;
    }
    inline void addSinkToCurSlice(const SVFGNode* node) {
        _curSlice->addToSinks(node);
        addToCurForwardSlice(node);
    }
    inline bool isInCurForwardSlice(const SVFGNode* node) {
        return _curSlice->inForwardSlice(node);
    }
    inline bool isInCurBackwardSlice(const SVFGNode* node) {
        return _curSlice->inBackwardSlice(node);
    }
    inline void addToCurForwardSlice(const SVFGNode* node) {
        _curSlice->addToForwardSlice(node);
    }
    inline void addToCurBackwardSlice(const SVFGNode* node) {
        _curSlice->addToBackwardSlice(node);
    }
    //@}

    /// Get sources/sinks
    //@{
    inline const SVFGNodeSet& getSources() const {
        return sources;
    }
    inline SVFGNodeSetIter sourcesBegin() const {
        return sources.begin();
    }
    inline SVFGNodeSetIter sourcesEnd() const {
        return sources.end();
    }
    inline void addToSources(const SVFGNode* node) {
        sources.insert(node);
    }
    //@}
protected:
    void dumpSlices();
};

#endif /* PIMPROF_H_ */
