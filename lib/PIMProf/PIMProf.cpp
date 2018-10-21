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
	finalize();
	std::cout << "w" << std::endl;
}