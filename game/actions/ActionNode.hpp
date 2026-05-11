#pragma once

#include "../declarations.hpp"
#include "ActionCode.hpp"


enum class ActionNodeType {
	RUNNER,
	FIRST,
	ALL,
	LOOP
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

		struct {
			const ActionNode* child;
		} loop;
	};
};

