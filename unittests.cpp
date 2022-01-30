#include "WrapBuffer.h"
#include <cassert>
#include <cstdint>
#include <string.h>
#include <iostream>

static std::mutex  mtx;

//this is for kind of doing unit tests but I don't have time, sorry    
void someTests(){
    std::cout << "some utests.." << std::endl;
    uint8_t myData[100], x[10];
    for (int i = 0; i < 100; i++)  myData[i] = i;
    memset (x, 0, sizeof(x)/(sizeof(x[0])));

    //just for debug
    buffers::RingBuffer<uint8_t, 30> ringbuf;
    assert(ringbuf.size() == 30);

    ringbuf.write(myData, 10);
    assert(ringbuf.unused() == 20);
    assert(ringbuf.used() == 10);

    ringbuf.write(&myData[10], 10);
    assert(ringbuf.unused() == 10);
    assert(ringbuf.used() == 20);

    ringbuf.write(&myData[20], 5);
    assert(ringbuf.unused() == 5);
    assert(ringbuf.used() == 25);

    ringbuf.write(&myData[25], 10);
    assert(ringbuf.unused() == 0);
    assert(ringbuf.used() == 30);

    ringbuf.read(x, 10);
    assert(ringbuf.unused() == 10);
    assert(ringbuf.used() == 20);

    ringbuf.write(&myData[25], 10);
    assert(ringbuf.unused() == 0);
    assert(ringbuf.used() == 30);

    ringbuf.read(x, 10);
    assert(ringbuf.unused() == 10);
    assert(ringbuf.used() == 20);

    ringbuf.read(x, 5);
    assert(ringbuf.unused() == 15);
    assert(ringbuf.used() == 15);

    ringbuf.read(x, 10);
    assert(ringbuf.unused() == 25);
    assert(ringbuf.used() == 5);

    buffers::WrapBuffer<30, true> wrapBuf(mtx);

    assert(wrapBuf.unused() == 30);
    auto n = wrapBuf.write(myData, 10);//write 10  (0 to 9), free(19)
    assert(wrapBuf.unused() == 19);
    assert(n == 10);
    n = wrapBuf.write(&myData[10], 10);//write 10  (10 to 19), free(8)
    assert(wrapBuf.unused() == 8);
    assert(n == 10);
    n = wrapBuf.write(&myData[20], 5); //write 5   (20 to 24), free (2)
    assert(wrapBuf.unused() == 2);
    assert(n == 5);
    n = wrapBuf.write(&myData[25], 10);//write 0   free(2)
    assert(n == 0);
    assert(wrapBuf.unused() == 2);
    n = wrapBuf.read(x);               //read 10 (0 to 9), free(13)
    assert(wrapBuf.unused() == 13);
    assert(!memcmp(x, myData, n));
    assert(n == 10);
    n = wrapBuf.write(&myData[25], 5);  //write 5 (25 to 29), free(7)
    assert(n == 5);
    assert(wrapBuf.unused() == 7);
    n = wrapBuf.read(x);                //read 10 (10 to 19), free(18)
    assert(n == 10);
    assert(!memcmp(x, &myData[10], n));
    assert(wrapBuf.unused() == 18);
    n = wrapBuf.read(x);                //read 5 (20 to 24), free(24) 
    assert(!memcmp(x, &myData[20], n));
    assert(n == 5);
    assert(wrapBuf.unused() == 24);
    n = wrapBuf.read(x);                //read 5 (25 to 29)
    assert(!memcmp(x, &myData[25], n));
    assert(n == 5);
    assert(wrapBuf.unused() == 30);
    n = wrapBuf.read(x);                //read 0
    assert(n == 0);

    std::cout << "end of uts" << std::endl;
}