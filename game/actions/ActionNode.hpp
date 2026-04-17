#pragma once

#include "../declarations.hpp"

enum class ActionCode {
	FAILURE,
	SUCCESS,
	RUNNING
};

enum class ActionNodeType {
	RUNNER,
	FIRST,
	ALL
};

struct ActionNode {
	ActionNodeType type;

	union {
		struct {
			ActionCode (*run)(Game& game, Character* character, void* data);
		} runner;

		struct {
			const ActionNode* const* children;
			int length;
		} first;

		struct {
			const ActionNode* const* children;
			int length;
		} all;
	};
};

