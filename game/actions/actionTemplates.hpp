#pragma once

#include <stddef.h>
#include "ActionNode.hpp"

#include <array>

template<typename... T>
constexpr auto list(T... elems) {
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

#define makeAll(name) all_##name = _makeAll(list_all_ ## name);
#define makeFst(name) fst_##name = _makeFst(list_fst_ ## name);


#define def(name) ActionCode run_##name(Game& game, Character* character, void* _data)
#define setData(T) T* data = (T*)_data;


#define makeFnc(name) name = ActionNode{ \
	.type = ActionNodeType::RUNNER, \
	.runner = {run_##name} \
};

