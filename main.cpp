#include <iostream>
#include <memory>

#include "RingBuffer.h"
#include "WrapBuffer.h"
#include "IModule.h"

using namespace Modules;

extern void someTests();//kind of uts

//I did static these stuff just for no use stack
static std::mutex mtx_1_2;//for buffer thread 1 to 2 
static std::mutex mtx_2_3;//for buffer thread 2 to 2 

static buffers::WrapBuffer<BufferLen, true> wrapBuf_1_2 (mtx_1_2);
static buffers::WrapBuffer<BufferLen, true> wrapBuf_2_3 (mtx_2_3);


int main() {
#ifdef DEBUG
    someTests();//kind of unit testing
#endif
    srand (time(NULL));

    std::unique_ptr<iModule> im1 (new Module1());
    std::unique_ptr<iModule> im2 (new Module2());
    std::unique_ptr<iModule> im3 (new Module3());

    const uint8_t match[] = {6,7,8};

    static_cast<Module2*>(im2.get())->setMatch (match, 1);

    im1.get()->setWrapBuf_1_to_2 (&wrapBuf_1_2);
    im2.get()->setWrapBuf_1_to_2 (&wrapBuf_1_2);
    im2.get()->setWrapBuf_2_to_3 (&wrapBuf_2_3);
    im3.get()->setWrapBuf_2_to_3 (&wrapBuf_2_3);

    std::thread th1 (&iModule::doJob, im1.get());
    std::thread th2 (&iModule::doJob, im2.get());
    std::thread th3 (&iModule::doJob, im3.get());

    th1.join();
    th2.join();
    th3.join();

    //prints map again just for check
    auto finalTimeVecMap = getfinalTimeVecMap();
    for (auto const& x : *finalTimeVecMap){
        std::cout << x.first << ":";
        for (const auto& xv : x.second) {
            std::cout << " " << std::hex << (unsigned)xv;
        }
        std::cout<< std::endl;
    }

    return 0;
}





