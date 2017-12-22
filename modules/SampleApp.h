/*
 * SasmpleApp.h
 *
 *  Created on: Nov 8, 2017
 *      Author: nhong
 */

#include <string.h>
#include <omnetpp.h>

class SampleApp {
    public:
        SampleApp();
        virtual ~SampleApp();

    public:
        int components[5];
        std::string assignment[5];
};
