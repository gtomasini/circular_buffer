#include <iostream>

#include "RingBuffer.h"
#include "WrapBuffer.h"
#include "IModule.h"

void someTests();

static std::mutex  mtx_1_2;

int main() {
    //someTests();
    buffers::WrapBuffer<1000, true> wrapBuf_1_2(mtx_1_2);
    iModule *im1 = new Module1();
    iModule *im2 = new Module2();
    iModule *im3 = new Module3();

    Module1* m1 = static_cast<Module1*>(im1);
    Module2* m2 = static_cast<Module2*>(im2);
    m1->setWrapBuf (&wrapBuf_1_2);
    m2->setWrapBuf (&wrapBuf_1_2);

    im2->launchThr();
    im1->launchThr();
    im3->launchThr();

    delete im1;
    delete im2;
    delete im3;
}

