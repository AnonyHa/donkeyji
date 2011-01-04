#include "svc_login.h"

LoginServiceImpl::LoginServiceImpl()
{
	register_service(this);
}

void LoginServiceImpl::Echo(
		::google::protobuf::RpcController* controller,
		const ::login::LoginRequest* req,
		::login::LoginResponse* rsp,
		::google::protobuf::Closure* done)
{

}

void svc_login_init()
{
	//静态初始化svc
	gpb::Service* svc = new LoginServiceImpl();
}
