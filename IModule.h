#ifndef IMODULE
#define IMODULE

#if !defined(__PRETTY_FUNCTION__) && !defined(__GNUC__)
#define __PRETTY_FUNCTION__ __FUNCSIG__
#endif

struct iModule {
	virtual void doJob() = 0;
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
