#include "svc_login.h"
LoginServiceImpl::LoginServiceImpl()
{
	register_service(this);
}

void LoginServiceImpl::Echo(
		gpb::RpcController* controller,
		TestRequest* req,
		TestResponse* rsp,
		Closure* done
	)
{}

void svc_login_init()
{
	//静态初始化svc
	Service* s = new TestServiceImpl();
}
