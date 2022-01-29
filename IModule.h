#ifndef IMODULE
#define IMODULE

#include <thread>
#include "WrapBuffer.h"

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

struct iModule {
	virtual void doJob() = 0;
	virtual void launchThr();
	virtual void setWrapBuf(buffers::WrapBuffer<1000, true> *wrapBufPtr) {
		_wrapBufPtr = wrapBufPtr;
	}

protected:
	buffers::WrapBuffer<1000, true>* _wrapBufPtr;
};

struct Module1 :iModule {
	void doJob();
	~Module1() {};
};

struct Module2 :iModule {
	void doJob();
	~Module2() {};
};

struct Module3 :iModule {
	void doJob();
	~Module3() {};
};

#endif
