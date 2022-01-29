#include <iostream>
#include <thread>

#include "RingBuffer.h"
#include "WrapBuffer.h"
#include "IModule.h"

void someTests();

int main() {
    someTests();

    iModule* m = new Module1();
    Module1* m1 = static_cast<Module1*>(m1);
    m1->doJob();

    std::thread th(&Module1::doJob, m1);
    th.join();

    delete m1;
}

