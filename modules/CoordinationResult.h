/*
 * CoordinationResult.h
 *
 *  Created on: Nov 8, 2017
 *      Author: nhong
 */

#include <string.h>
#include <omnetpp.h>
#include "SampleApp.h"

class CoordinationResult  : public ::omnetpp::cNamedObject{
    public:
        CoordinationResult();
        CoordinationResult(const CoordinationResult& r);
        virtual ~CoordinationResult();
        virtual CoordinationResult *dup() const {
            return new CoordinationResult(*this);
        }

    public:
        std::vector<SampleApp*> coordinationResults;

    const std::vector<SampleApp*>& getCoordinationResults() const {
        return coordinationResults;
    }

    void setCoordinationResults(
            const std::vector<SampleApp*>& coordinationResults) {
        this->coordinationResults = coordinationResults;
    }

};

