#include "ptos/test.pb.h"

//为每一个service写一个***_mod.cpp文件，用来注册service
class TestServiceImpl : public TestService
{
public:
	TestServiceImpl() : TestService()
	{
		//在构造函数里进行注册，然后生成一个全局变量，实现静态初始化
		register_service(this);
	}

	virtual void Echo(
		gpb::RpcController* controller,
		TestRequest* req,
		TestResponse* rsp,
		Closure* done
	)
	{}
};

//静态初始化svc
Service* s = new TestServiceImpl();
