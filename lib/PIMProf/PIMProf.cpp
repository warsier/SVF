/*
 * PIMProf.cpp
 *
 *  Created on: Oct 2018
 *      Author: Yizhou Wei
 */


#include "PIMProf/PIMProf.h"
#include "MSSA/SVFGStat.h"
#include "Util/GraphUtil.h"

using namespace llvm;


void PIMProf::analyze(SVFModule module) {
	initialize(module);
	std::cout << "w" << std::endl;
	for (SVFGNodeSetIter iter = sourcesBegin(), eiter = sourcesEnd();
            iter != eiter; ++iter) {
        setCurSlice(*iter);
		DBOUT(DGENERAL, outs() << "Analysing slice:" << (*iter)->getId() << ")\n");
    }
	finalize();
}

void PIMProf::initSrcs() {
	std::cout << "inited" << std::endl;
}
