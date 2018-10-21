
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

private:
    ProgSlice* _curSlice;		/// current program slice
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
protected:
    void dumpSlices();
};

#endif /* PIMPROF_H_ */
