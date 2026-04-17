#include "action_test.hpp"

#include "actionTemplates.hpp"
#include <stdio.h>

bool initialized = false;
using namespace actionNodes;

ActionNode all_result;
	ActionNode all_goTo;
		ActionNode search;
		ActionNode moveTo;
	
	ActionNode fst_open;
		ActionNode all_get;
			ActionNode takeKey;
			ActionNode unlock;

		ActionNode force;

	ActionNode pass;





def(search) {
	printf("search\n");


	return ActionCode::SUCCESS;
}

def(moveTo) {
	printf("moveTo\n");

	setData(Test);

	data->time--;
	if (data->time > 0)
		return ActionCode::RUNNING;

	return ActionCode::SUCCESS;
}

def(takeKey) {
	printf("takeKey\n");
	return ActionCode::SUCCESS;
}

def(unlock) {
	printf("unlock\n");
	return ActionCode::SUCCESS;
}

def(open) {
	printf("open\n");
	return ActionCode::SUCCESS;
}

def(force) {
	printf("force\n");
	return ActionCode::SUCCESS;
}

def(pass) {
	printf("pass\n");
	return ActionCode::SUCCESS;
}






auto list_all_result = list(&all_goTo, &fst_open, &pass);
auto list_all_goTo   = list(&search, &moveTo);
auto list_fst_open   = list(&all_get, &force);
auto list_all_get    = list(&takeKey, &unlock);


const ActionNode* Test::init() {
	if (initialized)
		return &all_result;

	initialized = true;


	makeAll(result);
		makeAll(goTo);
			makeFnc(search);
			makeFnc(moveTo);
		makeFst(open);
			makeAll(get);
				makeFnc(takeKey);
				makeFnc(unlock);
			makeFnc(force);
		makeFnc(pass);			


	return &all_result;
}