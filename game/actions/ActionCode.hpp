#pragma once

enum class ActionCode {
	FAILURE,
	SUCCESS,
	PENDING
};

#define ActionCode_get(test) (test) ?\
	ActionCode::SUCCESS : ActionCode::FAILURE;
