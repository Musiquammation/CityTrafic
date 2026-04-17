#pragma once

#include "declarations.hpp"


#include <vector>




class ActionNodeExecutor {
	const ActionNode* const root;
	const ActionNode* currentNode;
	void* const args;
	std::vector<int> list;
	void(*const destructor)(void* args);

public:
	static void FREE_DESTRUCTOR(void* args);

	ActionNodeExecutor(
		const ActionNode* root,
		void* args,
		void(*destructor)(void* args)
	);
	~ActionNodeExecutor();

	bool run(Game& game, Character* character);
};