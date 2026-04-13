#pragma once

// #define PRODUCE_NAPI 1

#if PRODUCE_NAPI



#pragma once

#include <napi.h>
#include "Api.hpp"

class ApiWrapper : public Napi::ObjectWrap<ApiWrapper> {
public:
	static Napi::Object Init(Napi::Env env, Napi::Object exports);

	ApiWrapper(const Napi::CallbackInfo& info);

private:
	static Napi::FunctionReference constructor;

	Api* api;

	Napi::Value CreateSession(const Napi::CallbackInfo& info);
	Napi::Value DeleteSession(const Napi::CallbackInfo& info);
	Napi::Value Take(const Napi::CallbackInfo& info);
};

#endif