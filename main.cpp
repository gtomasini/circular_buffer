#include <iostream>

#include "RingBuffer.h"
#include "WrapBuffer.h"
#include "IModule.h"


void someTests();

static std::mutex  mtx_1_2;

int main() {
    someTests();
    
    srand(time(NULL));

    buffers::WrapBuffer<1000, true> wrapBuf_1_2(mtx_1_2);
    iModule *im1 = new Module1();
    iModule *im2 = new Module2();
    iModule *im3 = new Module3();

    Module1* m1 = static_cast<Module1*>(im1);
    Module2* m2 = static_cast<Module2*>(im2);
    Module3* m3 = static_cast<Module3*>(im3);
    m1->setWrapBuf (&wrapBuf_1_2);
    m2->setWrapBuf (&wrapBuf_1_2);

    std::thread th1(&iModule::doJob, im1);
    std::thread th2(&iModule::doJob, im2);
    std::thread th3(&iModule::doJob, im3);

    th1.join();
    th2.join();
    th3.join();
    delete im1;
    delete im2;
    delete im3;
}


