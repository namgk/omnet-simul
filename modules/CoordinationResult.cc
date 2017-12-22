/*
 * CoordinationResult.cc
 *
 *  Created on: Nov 8, 2017
 *      Author: nhong
 */
#include <omnetpp.h>
#include "CoordinationResult.h"

CoordinationResult::CoordinationResult() {
    // TODO Auto-generated constructor stub
    setName("CoordinationResult");
}

CoordinationResult::~CoordinationResult() {
    // TODO Auto-generated destructor stub
}

CoordinationResult::CoordinationResult(const CoordinationResult& r) {
    std::vector<SampleApp*> coordinationResults = {};
    for (int i = 0; i < r.getCoordinationResults().size(); i++){
        SampleApp *appAssignment = r.getCoordinationResults().at(i);
        SampleApp *copy = new SampleApp();
        for (int j = 0; j < appAssignment->assignment->length(); j++){
            copy->assignment[j] = appAssignment->assignment[j];
        }
        coordinationResults.push_back(copy);
    }
    setCoordinationResults(coordinationResults);
}
