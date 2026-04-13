#include "ApiWrapper.hpp"

#if PRODUCE_NAPI

Napi::FunctionReference ApiWrapper::constructor;

Napi::Object ApiWrapper::Init(Napi::Env env, Napi::Object exports) {
	Napi::Function func = DefineClass(env, "ApiWrapper", {
		InstanceMethod("createSession", &ApiWrapper::CreateSession),
		InstanceMethod("deleteSession", &ApiWrapper::DeleteSession),
		InstanceMethod("take", &ApiWrapper::Take),
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("ApiWrapper", func);
	return exports;
}

ApiWrapper::ApiWrapper(const Napi::CallbackInfo& info)
	: Napi::ObjectWrap<ApiWrapper>(info)
{
	int threadnum = info[0].As<Napi::Number>().Int32Value();
	this->api = Api_createApi(threadnum);
}

Napi::Value ApiWrapper::CreateSession(const Napi::CallbackInfo& info) {
	int id = Api_createSession(this->api);
	return Napi::Number::New(info.Env(), id);
}

Napi::Value ApiWrapper::DeleteSession(const Napi::CallbackInfo& info) {
	int id = info[0].As<Napi::Number>().Int32Value();
	Api_deleteSession(this->api, id);
	return info.Env().Undefined();
}

Napi::Value ApiWrapper::Take(const Napi::CallbackInfo& info) {
	int id = info[0].As<Napi::Number>().Int32Value();
	int datacode = info[1].As<Napi::Number>().Int32Value();

	void* args = info[2].As<Napi::Buffer<uint8_t>>().Data();

	void* result = Api_take(this->api, id, datacode, args);

	return Napi::External<void>::New(info.Env(), result);
}


#endif
