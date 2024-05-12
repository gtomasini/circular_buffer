#include "WrapBuffer.h"
#include <cassert>
#include <cstdint>
#include <string.h>
#include <iostream>

//this is for kind of doing unit tests but I don't have time, sorry    
void someTests(){
    std::cout << "some utests.." << std::endl;
    uint8_t myData[100], x[100];
    for (int i = 0; i < 100; i++)  myData[i] = i+'A';
    memset (x, 0x00, sizeof(x)/(sizeof(x[0])));

    //just for debug
    buffers::RingBuffer<uint8_t, 30> ringbuf;
    assert(ringbuf.getTotalSize() == 30);

    auto n=ringbuf.write(myData, 10);
    assert(n==10);
    assert(ringbuf.getFreeSlots() == 20);
    assert(ringbuf.getUsedSlots() == 10);

    n=ringbuf.write(&myData[10], 10);
    assert(n==10);
    assert(ringbuf.getFreeSlots() == 10);
    assert(ringbuf.getUsedSlots() == 20);

    n=ringbuf.write(&myData[20], 5);
    assert(n==5);
    assert(ringbuf.getFreeSlots() == 5);
    assert(ringbuf.getUsedSlots() == 25);

    n=ringbuf.write(&myData[25], 10);
    assert(n==0);
    assert(ringbuf.getFreeSlots() == 5);
    assert(ringbuf.getUsedSlots() == 25);

    n=ringbuf.read(x, 100);
    assert(n==25);
    assert(ringbuf.getFreeSlots() == 30);
    assert(ringbuf.getUsedSlots() == 0);

    ringbuf.write(&myData[25], 10);
    assert(ringbuf.getFreeSlots() == 0);
    assert(ringbuf.getUsedSlots() == 30);

    ringbuf.read(x, 10);
    assert(ringbuf.getFreeSlots() == 10);
    assert(ringbuf.getUsedSlots() == 20);

    ringbuf.read(x, 5);
    assert(ringbuf.getFreeSlots() == 15);
    assert(ringbuf.getUsedSlots() == 15);

    ringbuf.read(x, 10);
    assert(ringbuf.getFreeSlots() == 25);
    assert(ringbuf.getUsedSlots() == 5);

    buffers::WrapBuffer<30, true> wrapBuf;

    assert(wrapBuf.getUnusedSlots() == 30);
    n = wrapBuf.write(myData, 10);//write 10  (0 to 9), free(19)
    assert(wrapBuf.getUnusedSlots() == 19);
    assert(n == 10);
    n = wrapBuf.write(&myData[10], 10);//write 10  (10 to 19), free(8)
    assert(wrapBuf.getUnusedSlots() == 8);
    assert(n == 10);
    n = wrapBuf.write(&myData[20], 5); //write 5   (20 to 24), free (2)
    assert(wrapBuf.getUnusedSlots() == 2);
    assert(n == 5);
    n = wrapBuf.write(&myData[25], 10);//write 0   free(2)
    assert(n == 0);
    assert(wrapBuf.getUnusedSlots() == 2);
    n = wrapBuf.read(x);               //read 10 (0 to 9), free(13)
    assert(wrapBuf.getUnusedSlots() == 13);
    assert(!memcmp(x, myData, n));
    assert(n == 10);
    n = wrapBuf.write(&myData[25], 5);  //write 5 (25 to 29), free(7)
    assert(n == 5);
    assert(wrapBuf.getUnusedSlots() == 7);
    n = wrapBuf.read(x);                //read 10 (10 to 19), free(18)
    assert(n == 10);
    assert(!memcmp(x, &myData[10], n));
    assert(wrapBuf.getUnusedSlots() == 18);
    n = wrapBuf.read(x);                //read 5 (20 to 24), free(24) 
    assert(!memcmp(x, &myData[20], n));
    assert(n == 5);
    assert(wrapBuf.getUnusedSlots() == 24);
    n = wrapBuf.read(x);                //read 5 (25 to 29)
    assert(!memcmp(x, &myData[25], n));
    assert(n == 5);
    assert(wrapBuf.getUnusedSlots() == 30);
    n = wrapBuf.read(x);                //read 0
    assert(n == 0);

    std::cout << "end of uts" << std::endl;
}