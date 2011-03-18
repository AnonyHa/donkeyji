//为每一个service写一个***_mod.cpp文件，用来注册service
class LoginServiceImpl : public LoginService
{
public:
	LoginServiceImpl() : LoginService() {
		//在构造函数里进行注册，然后生成一个全局变量，实现静态初始化
		register_service(this);
	}

	virtual void Echo(
	    RpcController* controller,
	    TestRequest* req,
	    TestResponse* rsp,
	    Closure* done
	)
	{}
};

//静态初始化
Service* s = new LoginServiceImpl();
