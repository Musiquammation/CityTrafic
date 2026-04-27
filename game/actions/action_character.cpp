#include "action_character.hpp"

#include "ActionNode.hpp"
#include "actionTemplates.hpp"
#include <stdio.h>


startns(character)



#define LIST(all, fst, run, link)\
	all(result);\
		fst(runDayJob);\
			all(workSection);\
				run(isWorkDay);\
				all(workDay);\
					link(takeCarToWork);\
					link(waitCarNotification);\
					link(walkToWork);\
					link(takeCarToHome);\
					link(waitCarNotification);\
					link(walkToHome);\
			all(chillSection);\
				run(isChillDay);\
				all(chillDay);\
					run(chillDayTest);\
	run(takeCarToWork);\
	run(takeCarToHome);\
	run(walkToWork);\
	run(walkToHome);\
	run(waitCarNotification);\
			
		



declList();

def(isWorkDay) {
	return ActionCode::SUCCESS;
}

def(isChillDay) {
	return ActionCode::FAILURE;
}

def(takeCarToWork) {
	printf("take car to work\n");
	return ActionCode::PENDING;
}

def(takeCarToHome) {
	printf("take car to home\n");
	return ActionCode::PENDING;
}

def(walkToWork) {
	printf("walkToWork\n");
	return ActionCode::PENDING;
}

def(walkToHome) {
	printf("walkToHome\n");
	return ActionCode::PENDING;
}

def(waitCarNotification) {
	printf("waitCarNotification\n");
	return ActionCode::PENDING;
}




give(result, &runDayJob);

give(runDayJob, &workSection, &chillSection);

give(workSection, &isWorkDay, &workDay);

give(workDay,
    &takeCarToWork,
    &waitCarNotification,
    &walkToWork,
    &takeCarToHome,
    &waitCarNotification,
    &walkToHome
);

give(chillSection, &isChillDay, &chillDay);

give(chillDay, &chillDayTest);







const ActionNode* init() {
	if (initialized)
		return &result;

	initialized = true;

	makeLink();

	return &result;
}

finishns()
