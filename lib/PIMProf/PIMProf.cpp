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

static cl::opt<bool> DumpSlice("dump-slice", cl::init(false),
                               cl::desc("Dump dot graph of Saber Slices"));

static cl::opt<unsigned> cxtLimit("cxtlimit",  cl::init(3),
                                  cl::desc("Source-Sink Analysis Contexts Limit"));

void PIMProf::analyze(SVFModule module) {
	initialize(module);
	finalize();
}