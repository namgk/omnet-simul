/*
 * Assignment.h
 *
 *  Created on: Nov 6, 2017
 *      Author: nhong
 */

#include <omnetpp.h>

class Assignment : public ::omnetpp::cNamedObject{
    public:
        const char *a;
        const char *b;
        const char *c;

    public:
        Assignment();
        Assignment(const Assignment& r);
        Assignment(const char *a, const char *b, const char *c);
        virtual ~Assignment();
        virtual Assignment *dup() const {
            return new Assignment(*this);
        }

    const char* getA() const {
        return a;
    }

    void setA(const char* a) {
        this->a = a;
    }

    const char* getB() const {
        return b;
    }

    void setB(const char* b) {
        this->b = b;
    }

    const char* getC() const {
        return c;
    }

    void setC(const char* c) {
        this->c = c;
    }
};

