/*
 * ContextSync.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: nhong
 */

#include <omnetpp.h>
#include "ContextSync.h"


ContextSync::ContextSync() {
    // TODO Auto-generated constructor stub
    setName("ctx");
}
ContextSync::ContextSync(const ContextSync& r) {
    setName("ctx");
    setDevice(r.getDevice());
    setX(r.getX());
    setY(r.getY());
}

ContextSync::~ContextSync() {
    // TODO Auto-generated destructor stub
}


