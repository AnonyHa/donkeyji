
//为每一个service写一个***_mod.cpp文件，用来注册service
class LoginServiceImpl : public LoginService
{
public:
	LoginServiceImpl();

	virtual void Echo(
		gpb::RpcController* controller,
		TestRequest* req,
		TestResponse* rsp,
		Closure* done
	);
};

void svc_login_init();
