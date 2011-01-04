//c std
#include <stdint.h>
//c++ std
#include <string>
#include <map>
//google proto buffer
#include <google/protobuf/service.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/stubs/common.h>

struct rpc_method
{
	gpb::Service* svc;
	gpb::Message* req;
	gpb::Message* rsp;
	gpb::MethodDescriptor* method;
};

//---------------------------------
std::map<uint32_t, rpc_method*> rpc_map;
//---------------------------------

uint32_t hashword(const uint32_t *k, size_t length, uint32_t initval)
{
	uint32_t a,b,c;

	a = b = c = 0xdeadbeef + (((uint32_t)length)<<2) + initval;

	while (length > 3) {
		a += k[0];
		b += k[1];
		c += k[2];
		mix(a,b,c);
		length -= 3;
		k += 3;
	}

	switch(length) { 
	case 3: 
		c+=k[2];
	case 2:
		b+=k[1];
	case 1:
		a+=k[0];
		final(a,b,c);
	case 0:     /* case 0: nothing left to add */
	   break;
	}
	return c;
}


uint32_t hash_string(const string& str)
{
	uint32_t hw = hashword(reinterpret_cast<const uint32_t*>(str.c_str()), str.size() / 4, HASH_INITVAL);
	uint32_t ret = static_cast<uint32_t>(hw);
	return ret;
}

//注册service的接口，在每个***_mod.cpp里调用
void register_service(gpb::Service* svc)
{
	gpb::ServiceDescriptor* sd = svc->GetDescriptor();

	for (int i=0; i<sd->method_count(); i++) {
		gpb::MethodDescriptor* method = sd->method(i);

		gpb::Message* req = &svc->GetRequestProtoType(method);
		gpb::Message* rsp = &svc->GetResponseProtoType(method);

		uint32_t method_id = hash_string(method->full_name());

		//save the echo
		rpc_method* rm = new rpc_method(svc, req, rsp, method);			
		rpc_map[method_id] = rm;
	}
}
