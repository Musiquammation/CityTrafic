#include "ActionExecutor.hpp"
#include "actions/ActionNode.hpp"

#include <stdexcept>

using Node = const ActionNode;

Node* getListNode(Node* root, const std::vector<int>& list, bool ignoreLast = false) {
	Node* node = root;

	size_t limit = list.size();
	if (ignoreLast && limit > 0) {
		limit--;
	}

	for (size_t i = 0; i < limit; i++) {
		int idx = list[i];

		switch (node->type) {
		case ActionNodeType::FIRST:
			node = node->first.children[idx];
			break;

		case ActionNodeType::ALL:
			node = node->all.children[idx];
			break;

		case ActionNodeType::RUNNER:
			throw std::runtime_error{"Invalid action node type in getListNode"};
		}
	}

	return node;
}

void ActionExecutor::FREE_DESTRUCTOR(void* args) {
	free(args);
}


ActionExecutor::ActionExecutor(
	const ActionNode* root,
	void* args,
	void(*destructor)(void* args)
):
	root(root),
	currentNode(root),
	args(args),
	destructor(destructor)
{
	
}

ActionExecutor::~ActionExecutor() {
	if (this->destructor == nullptr)
		return;

	if (this->destructor == ActionExecutor::FREE_DESTRUCTOR)
		free(this->args);

	this->destructor(this->args);
}

bool ActionExecutor::run(Game& game, Character* character) {
	ActionCode prevResult = ActionCode::PENDING;
	
	Node* node = this->currentNode;
	if (!node)
		return true;
	
	do {
		switch (node->type) {
		case ActionNodeType::RUNNER:
		{
			auto result = node->runner.run(game, character, this->args);
			if (result == ActionCode::PENDING) {
				goto exit;
			}

			prevResult = result;

			int childIdx = this->list.back();
			node = getListNode(this->root, this->list, true);
			break;
		}

		case ActionNodeType::FIRST:
		{
			// First iteration
			if (prevResult == ActionCode::PENDING) {
				this->list.push_back(0);
				node = node->first.children[0];
				break;
			}

			// Collect result of execution
			if (prevResult == ActionCode::FAILURE) {				
				Node* parent = getListNode(this->root, this->list);

				int idx = this->list.back();
				idx++;

				// 'All' is finished (return failure to parent)
				if (idx >= node->first.length) {
					prevResult = ActionCode::FAILURE;
					goto callParent;
				}

				// Run next child
				this->list.back() = idx;
				node = getListNode(this->root, this->list);
				prevResult = ActionCode::PENDING;
				break;
			}

			// Success: call parent
			prevResult = ActionCode::SUCCESS;
			goto callParent;
		}

		case ActionNodeType::ALL:
		{
			// First iteration
			if (prevResult == ActionCode::PENDING) {
				this->list.push_back(0);
				node = node->first.children[0];
				break;
			}

			// Collect result of execution
			if (prevResult == ActionCode::SUCCESS) {				
				Node* parent = getListNode(this->root, this->list);

				int idx = this->list.back();
				idx++;

				// 'All' is finished (return success to parent)
				if (idx >= node->all.length) {
					prevResult = ActionCode::SUCCESS;
					goto callParent;
				}

				// Run next child
				this->list.back() = idx;
				node = getListNode(this->root, this->list);
				prevResult = ActionCode::PENDING;
				break;
			}

			// Failure: call parent
			prevResult = ActionCode::FAILURE;
			goto callParent;
		}
		}

		continue;


		callParent:
		this->list.pop_back(); // delete child
		node = getListNode(this->root, this->list, true);

	} while (this->list.size());

	

	// Action graph finished
	this->currentNode = NULL;
	return true;

	exit:
	// Performing pending instruction
	this->currentNode = node;
	return false;
}


void ActionExecutor::restart() {
	this->currentNode = this->root;
}