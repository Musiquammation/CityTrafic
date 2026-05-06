#include "Job.hpp"

#include "Game.hpp"
#include "Character.hpp"

void Job::give(int money) {
	this->money += money;
}

int Job::pay(int money) {
	if (this->money >= money) {
		this->money -= money;
		return money;
	}

	int given = this->money;
	this->money = 0;
	return given;
}

void Job::destroy(Game& game, int owner) {
	Player* player = game.getPlayer(owner);
	player->money += this->money;

	// Fire everyone
	this->forAllWorkers([&game](Character* c) {
		c->leaveJob(game);
	});
}

int Job::getMoney() const {
	return this->money;
}