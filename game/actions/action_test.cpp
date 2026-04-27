#include "action_test.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"
#include <stdio.h>


startns(test)



ActionNode result;
	ActionNode goTo;
		ActionNode search;
		ActionNode moveTo;
	
	ActionNode open;
		ActionNode get;
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

	setData();

	data->time--;
	if (data->time > 0)
		return ActionCode::PENDING;

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






auto list_result = fillList(&goTo, &open, &pass);
auto list_goTo   = fillList(&search, &moveTo);
auto list_open   = fillList(&get, &force);
auto list_get    = fillList(&takeKey, &unlock);


const ActionNode* init() {
	if (initialized)
		return &result;

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


	return &result;
}


finishns()