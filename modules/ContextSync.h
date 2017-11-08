/*
 * ContextSync.h
 *
 *  Created on: Nov 6, 2017
 *      Author: nhong
 */

#include <omnetpp.h>

class ContextSync : public ::omnetpp::cNamedObject{
    public:
        const char *device;
        double x;
        double y;

    public:
        ContextSync();
        ContextSync(const ContextSync& r);
        virtual ~ContextSync();
        virtual ContextSync *dup() const {
            return new ContextSync(*this);
        }

    const char* getDevice() const {
        return device;
    }

    void setDevice(const char* device) {
        this->device = device;
    }

    double getX() const {
        return x;
    }

    void setX(double x) {
        this->x = x;
    }

    double getY() const {
        return y;
    }

    void setY(double y) {
        this->y = y;
    }
};

