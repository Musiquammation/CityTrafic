#pragma once

#include <stddef.h>
#include "ActionNode.hpp"

#include <array>

template<typename... T>
constexpr auto fillList(T... elems) {
	return std::array<const ActionNode*, sizeof...(T)>{ elems... };
}

template<size_t N>
constexpr ActionNode _makeAll(const std::array<const ActionNode*, N>& children) {
	return ActionNode{
		.type = ActionNodeType::ALL,
		.all = {
			children.data(),
			(int)N
		}
	};
}



template<size_t N>
constexpr ActionNode _makeFst(const std::array<const ActionNode*, N>& children) {
	return ActionNode{
		.type = ActionNodeType::FIRST,
		.all = {
			children.data(),
			(int)N
		}
	};
}



#define def(name) ActionCode run_##name(Game& game, Character* character, void* _data)
#define setData() Data* data = (Data*)_data;

#define declNode(name) ActionNode name;
#define declLink(name) 
#define declList() LIST(declNode, declNode, declNode, declLink)

#define makeAll(name) name = _makeAll(list_## name);
#define makeFst(name) name = _makeFst(list_## name);
#define makeFnc(name) name = ActionNode{ \
	.type = ActionNodeType::RUNNER, \
	.runner = {run_##name} \
};
#define makeLink(name)
#define makeList() LIST(makeAll, makeFst, makeFnc, makeLink)


#define give(name, ...) auto list_##name = fillList(__VA_ARGS__);


#define startns(name) namespace actionNodes::name { bool initialized = false;
#define finishns() }