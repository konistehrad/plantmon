#pragma once

#include <Thread.h>

class BucketThread : public Thread {
    virtual bool init() = 0;
    void run() override {
        runned();
    }
};
