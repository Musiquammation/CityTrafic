#pragma once

#include "declarations.hpp"


#include <vector>




class ActionExecutor {
	const ActionNode* root;
	const ActionNode* currentNode;
	void* args;
	std::vector<int> list;
	void(*destructor)(void* args);

	friend struct serialize;
public:
	static void FREE_DESTRUCTOR(void* args);

	ActionExecutor(
		const ActionNode* root,
		void* args,
		void(*destructor)(void* args)
	);
	~ActionExecutor();

	bool run(Game& game, Character* character);
	void restart();
};
