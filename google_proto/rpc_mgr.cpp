struct rpc_method
{
	Service* svc;
	Message* req;
	Message* rsp;
	MethodDescriptor* method;
};

//---------------------------------
std::map<int, rpc_method*> rpc_map;
//---------------------------------

//注册service的接口，在每个***_mod.cpp里调用
void register_service(Service* s)
{
	ServiceDescriptor* ds = s->GetDescriptor();

	for (int i=0; i<ds->method_count(); i++) {
		MethodDescriptor* method = ds->method(i);

		Message* req = &s->GetRequestProtoType(method);
		Message* rsp = &s->GetResponseProtoType(method);

		int method_id = hash_string(method->full_name());

		//save the echo
		rpc_method* rm = new rpc_method(s, req, rsp, method);			
		rpc_map[method_id] = rm;
	}
}
