#pragma once

// #define PRODUCE_NAPI 1

#if PRODUCE_NAPI
#include <napi.h>

#include "Api.hpp"


// -------------------------
// ApiWrapper N-API
// -------------------------
class ApiWrapper : public Napi::ObjectWrap<ApiWrapper> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    ApiWrapper(const Napi::CallbackInfo& info);
    static Napi::FunctionReference constructor;


private:
    Napi::Value CreateSession(const Napi::CallbackInfo& info);
    Napi::Value DeleteSession(const Napi::CallbackInfo& info);
    Napi::Value Take(const Napi::CallbackInfo& info);

    Api* api_{nullptr};
};

Napi::Value CreateApi(const Napi::CallbackInfo& info);
Napi::Value DeleteApi(const Napi::CallbackInfo& info);
Napi::Value CreateSession(const Napi::CallbackInfo& info);
Napi::Value DeleteSession(const Napi::CallbackInfo& info);
Napi::Value Take(const Napi::CallbackInfo& info);


#endif