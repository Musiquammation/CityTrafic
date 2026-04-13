#include "ApiWrapper.hpp"

#if PRODUCE_NAPI
#include <iostream>

using namespace std;

// -------------------------
// ApiWrapper N-API
// -------------------------
Napi::FunctionReference ApiWrapper::constructor;

ApiWrapper::ApiWrapper(const Napi::CallbackInfo& info)
: Napi::ObjectWrap<ApiWrapper>(info)
{
	int threads = info[0].As<Napi::Number>().Int32Value();
	api_ = new Api(threads);
	api_->init();
}

Napi::Value ApiWrapper::CreateSession(const Napi::CallbackInfo& info) {
	int id = api_->createSession();
	return Napi::Number::New(info.Env(), id);
}

Napi::Value ApiWrapper::DeleteSession(const Napi::CallbackInfo& info) {
	int id = info[0].As<Napi::Number>().Int32Value();
	api_->deleteSession(id);
	return info.Env().Undefined();
}

Napi::Value ApiWrapper::Take(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();

	int id = info[0].As<Napi::Number>().Int32Value();
	int code = info[1].As<Napi::Number>().Int32Value();

	void* argsPtr = nullptr;

	// Optional args buffer
	if (info.Length() >= 3 && !info[2].IsUndefined() && !info[2].IsNull()) {
		if (!info[2].IsBuffer()) {
			Napi::TypeError::New(env, "args must be a Buffer").ThrowAsJavaScriptException();
			return env.Null();
		}

		Napi::Buffer<uint8_t> buf = info[2].As<Napi::Buffer<uint8_t>>();
		argsPtr = static_cast<void*>(buf.Data());
	}

	void* ptr = api_->take(id, code, argsPtr);

	// Return pointer to JS (opaque external)
	return Napi::External<void>::New(env, ptr);
}

Napi::Object ApiWrapper::Init(Napi::Env env, Napi::Object exports) {
	Napi::HandleScope scope(env);

	Napi::Function func = DefineClass(env, "Api", {
		ApiWrapper::InstanceMethod("createSession", &ApiWrapper::CreateSession),
		ApiWrapper::InstanceMethod("deleteSession", &ApiWrapper::DeleteSession),
		ApiWrapper::InstanceMethod("take", &ApiWrapper::Take)
	});

	constructor = Napi::Persistent(func);
	constructor.SuppressDestruct();

	exports.Set("Api", func);
	return exports;
}

// -------------------------
// Global API instance
// -------------------------
Napi::ObjectReference gApiRef;

Napi::Value CreateApi(const Napi::CallbackInfo& info) {
	Napi::Env env = info.Env();
	int threads = info[0].As<Napi::Number>().Int32Value();

	Napi::Object obj = ApiWrapper::constructor.New({
		Napi::Number::New(env, threads)
	});

	gApiRef = Napi::Persistent(obj);
	gApiRef.SuppressDestruct();

	return obj;
}

Napi::Value DeleteApi(const Napi::CallbackInfo& info) {
	if (!gApiRef.IsEmpty()) {
		gApiRef.Reset();
	}
	return info.Env().Undefined();
}

Napi::Value CreateSession(const Napi::CallbackInfo& info) {
	if (gApiRef.IsEmpty()) return info.Env().Undefined();

	Napi::Object obj = gApiRef.Value();

	return obj.Get("createSession")
		.As<Napi::Function>()
		.Call(obj, {});
}

Napi::Value DeleteSession(const Napi::CallbackInfo& info) {
	if (gApiRef.IsEmpty()) return info.Env().Undefined();

	int id = info[0].As<Napi::Number>().Int32Value();
	Napi::Object obj = gApiRef.Value();

	obj.Get("deleteSession")
		.As<Napi::Function>()
		.Call(obj, { Napi::Number::New(info.Env(), id) });

	return info.Env().Undefined();
}

Napi::Value Take(const Napi::CallbackInfo& info) {
	if (gApiRef.IsEmpty()) return info.Env().Undefined();

	Napi::Object obj = gApiRef.Value();

	Napi::Value args = (info.Length() >= 3)
		? info[2]
		: info.Env().Undefined();

	return obj.Get("take")
		.As<Napi::Function>()
		.Call(obj, {
			info[0],
			info[1],
			args
		});
}

// -------------------------
// Module initialization
// -------------------------
Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
	ApiWrapper::Init(env, exports);

	exports.Set("Api_create", Napi::Function::New(env, CreateApi));
	exports.Set("Api_delete", Napi::Function::New(env, DeleteApi));

	return exports;
}

NODE_API_MODULE(addon, InitAll)

#endif
