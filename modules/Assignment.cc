/*
 * Assignment.cpp
 *
 *  Created on: Nov 6, 2017
 *      Author: nhong
 */

#include <omnetpp.h>
#include "Assignment.h"


Assignment::Assignment() {
    // TODO Auto-generated constructor stub
}
Assignment::Assignment(const Assignment& r) {
    setName(r.getA());
    setA(r.getA());
    setB(r.getB());
    setC(r.getC());
}
Assignment::Assignment(const char *a, const char *b, const char *c){
    setName(a);
    this->a = a;
    this->b = b;
    this->c = c;
}

Assignment::~Assignment() {
    // TODO Auto-generated destructor stub
}


