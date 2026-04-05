#include "api.hpp"

Api* Api_create() {
	auto api = new Api{};

	return api;
}


void Api_delete(Api* api) {
	delete api;
}

void Api_frame(Api* api) {
	
}


void Api_reserve(Api* api, int code) {
	
}
