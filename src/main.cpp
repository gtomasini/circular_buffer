#include <iostream>
#include <memory>
#include <cassert>

#include "RingBuffer.h"
#include "Module1.h"
#include "Module2.h"
#include "Module3.h"
#include "Color.h"

using namespace modules;

extern void someTests();//kind of uts


#define DEBUG

int main() {
    std::cout<<Clr::def;
    static buffers::RingBuffer<uint8_t, BufferLen, true> wrapBuf_1_2;
    static buffers::RingBuffer<uint8_t, BufferLen, true> wrapBuf_2_3;

#ifdef DEBUG
    someTests();// kind of unit testing
#endif
    srand (time(NULL));

    std::unique_ptr<Module3> m3 (new Module3);
    std::unique_ptr<Module2> m2 (new Module2(*m3.get()));
    std::unique_ptr<Module1> m1 (new Module1(*m2.get()));

    const uint8_t match[] = {6, 2};// pattern to search

    static_cast<Module2*>(m2.get())->setMatch (match, 2);

    std::thread th1 (&iModule::doMyJob, m1.get());
    std::thread th2 (&iModule::doMyJob, m2.get());
    std::thread th3 (&iModule::doMyJob, m3.get());
	
    m1->start();
    m2->start();
    m3->start();
	std::this_thread::sleep_for(std::chrono::seconds(10));

    m1->stop();
    m2->stop();
    //m3->stop();

    th1.join();
    th2.join();
    th3.join();

    //prints map again just for check the record map
	std::cout<<Clr::def<<"main(), final recorded map:"<<std::endl;
    auto finalTimeVecMap = getfinalTimeVecMap();
    for (auto const& x : *finalTimeVecMap){
        std::cout << std::dec<<x.first << ":";
        for (const auto& xv : x.second) {
            std::cout << " " << std::hex << (unsigned)xv;
        }
        std::cout<< std::endl;
    }

    return 0;
}





