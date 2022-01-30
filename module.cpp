#include <iostream>
#include <thread>
#include <chrono>  
#include "IModule.h"


void Module1:: doJob() {
	std::cout << std::this_thread::get_id();
	std::cout << " "<<__PRETTY_FUNCTION__ << std::endl;
	uint8_t x[] = { 0,1,2,3,4,5,6,7,8,9 };
	std::this_thread::sleep_for(std::chrono::seconds(3));
	std::cout << "mod1, unused: " << _wrapBufPtr->unused() << std::endl;
	_wrapBufPtr->write (x, 10);
	std::cout << "mod1, unused: "<<_wrapBufPtr->unused() << std::endl;

	std::this_thread::sleep_for(std::chrono::seconds(10));
	std::cout << "mod1, ends" << std::endl;
}

void Module2::doJob() {
	std::cout << std::this_thread::get_id();
	std::cout << " "<<__PRETTY_FUNCTION__ << std::endl;
	uint8_t x[10];
	uint8_t n = _wrapBufPtr->read(x);
	std::cout << "mod2, reads n: "<<n << std::endl;
	n = _wrapBufPtr->read(x);
	std::cout << "mod2, ends" << std::endl;
}

void Module3::doJob() {
	std::cout << std::this_thread::get_id();
	std::cout << " "<<__PRETTY_FUNCTION__ << std::endl;
	std::this_thread::sleep_for(std::chrono::seconds(10));
	std::cout << "mod3, ends" << std::endl;
}