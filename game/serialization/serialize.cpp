#include "serialize.hpp"

#include <cmath>
#include <game/actions/ActionNode.hpp>

#include "jobSerializator.hpp"
#include "../utils/streams.hpp"
#include "../Game.hpp"
#include "../Character.hpp"
#include "../Car.hpp"
#include "../Building.hpp"
#include "../Cell.hpp"
#include "../cell_t.hpp"
#include "../actions/action_character.hpp"

void serialize::save(const Game &game, WriteStream &stream) {
	// Save map
	stream.write(game.map.x);
	stream.write(game.map.y);
	stream.write(game.map.width);
	stream.write(game.map.height);

	// Copy map
	int mapLength = (game.map.width - game.map.x) *
		(game.map.height - game.map.y);
		
	stream.copy(game.map.cells, (size_t)(mapLength * sizeof(cell_t)));


	// Save cars
	stream.write(game.carHandler.cars.size());
	for (auto it: game.carHandler.cars) {
		Car* car = it.second;
		stream.write(car);
		stream.write(*car);
		if (car->pathHandler.array && car->pathHandler.step >= 0) {
			for (int i = 0; i < car->pathHandler.step; i++)
				stream.write(car->pathHandler.array[i]);

			int n = (car->pathHandler.step+7)/8;
			for (int i = 0; i < n; i++)
				stream.write(car->pathHandler.dirArray[i]);
		}
	}

	// Save characters
	stream.write(game.characterHandler.characters.size());
	for (Character* c: game.characterHandler.characters) {
		stream.write(c);
		stream.write(c->car);
		stream.write(c->jobLoc);
		stream.write(c->home);
		stream.write(c->state);
		stream.write(c->status);
		stream.write(c->money);
		stream.write(c->waitingJoyMoney);
		stream.write(c->salaryEstimation);
		stream.write(c->pointId);
		stream.write(c->nextRentPayMonth);
		stream.write(c->seeds);
		stream.write((int)c->executor.list.size());
		for (auto i: c->executor.list) {stream.write(i);}
	}

	// Calendar indicator
	stream.write(game.calendar.indicator);

	// Players
	stream.write(game.players.size());
	for (auto& player: game.players) {
		stream.write(player);
	}

	// Jobs
	for (const auto&[_, b]: game.map.buildings) {
		if (Job* job = b->getJob()) {
			stream.write(job);
			jobSerializator::save(stream, job);
		}
	}

	stream.write(nullptr); // end of jobs

	// Buildings
	stream.write(game.map.buildings.size());
	for (const auto&[p, b]: game.map.buildings) {
		stream.write(p);
		b->fileSave(stream);
	}
}



void serialize::open(Game &game, ReadStream &stream) {
	// Load map dimensions
	game.map.x = stream.read<int>();
	game.map.y = stream.read<int>();
	game.map.width = stream.read<int>();
	game.map.height = stream.read<int>();
	free(game.map.cells);

	size_t mapLength = (game.map.width - game.map.x) * (game.map.height - game.map.y);
	printf("%ld\n", mapLength);
	game.map.cells = (Cell*)calloc(mapLength, sizeof(cell_t));
	stream.copy(game.map.cells, mapLength*sizeof(cell_t));

	// Map to link old pointers to new instances
	std::unordered_map<Car*, Car*> carMap;

	// Load cars
	size_t carCount;
	stream.read(carCount);
	for (size_t i = 0; i < carCount; ++i) {
		Car* oldPtr;
		stream.read(oldPtr);

		Car* car = (Car*)::operator new(sizeof(Car));
		stream.read(*car);

		game.carHandler.cars[{
			(int)std::floor(car->x),
			(int)std::floor(car->y),
		}] = car;


		if (car->pathHandler.array && car->pathHandler.step >= 0) {
			car->pathHandler.array = new Vector<int>[car->pathHandler.length];
			car->pathHandler.dirArray =  new uint8_t[car->pathHandler.length];
			for (int i = 0; i < car->pathHandler.step; i++)
				stream.read(car->pathHandler.array[i]);

			for (int i = 0; i < car->pathHandler.step; i++)
				stream.read(car->pathHandler.dirArray[i]);
		}

		carMap[oldPtr] = car;
	}

	// Load characters
	size_t charCount;
	stream.read(charCount);
	std::unordered_map<Character*, Character*> characterMap;
	game.characterHandler.characters.reserve(charCount);
	for (size_t i = 0; i < charCount; ++i) {
		Character* oldPtr;
		stream.read(oldPtr);

		auto c = new Character{};
		stream.read(c->car);
		stream.read(c->jobLoc);
		stream.read(c->home);
		stream.read(c->state);
		stream.read(c->status);
		stream.read(c->money);
		stream.read(c->waitingJoyMoney);
		stream.read(c->salaryEstimation);
		stream.read(c->pointId);
		stream.read(c->nextRentPayMonth);
		stream.read(c->seeds);
		int size = stream.read<int>();
		c->executor.list.reserve(size);
		for (int i = 0; i < size; i++) {
			c->executor.list.push_back(stream.read<int>());
		}
		c->executor.root = actionNodes::character::init();
		c->executor.destructor = nullptr;
		// Get current node
		{
			const ActionNode* node = c->executor.root;
			for (int i = 0; i < size; i++) {
				switch (node->type) {
					case ActionNodeType::ALL:
						node = node->all.children[c->executor.list[i]];
						break;

					case ActionNodeType::FIRST:
						node = node->first.children[c->executor.list[i]];
						break;

					case ActionNodeType::RUNNER:
						break;

				}
			}

			c->executor.currentNode = node;
		}
		c->executor.args = c;

		characterMap[oldPtr] = c;

		// Update car pointer using hash map
		if (c->car) {
			c->car = carMap[c->car];
		}

		game.characterHandler.characters.push_back(c);
	}

	// Update car drivers
	for (auto [_, car]: game.carHandler.cars) {
		if (auto old = car->driver) {
			car->driver = characterMap[old];
		}
	}

	// Calendar
	auto calendar = stream.read<calendar_t>();
	game.calendar.updateIndicator(calendar);

	// Players
	size_t playerCount;
	stream.read(playerCount);
	game.players.resize(playerCount);
	for (auto& player : game.players) {
		stream.read(player);
	}

	std::unordered_map<Job*, Job*> jobMap;
	// Jobs
	while (true) {
		auto oldJobPtr = stream.read<Job*>();
		if (oldJobPtr == nullptr) break;

		Job* job = jobSerializator::open(stream, characterMap);
		jobMap[oldJobPtr] = job;
	}

	// Buildings
	size_t buildCount;
	stream.read(buildCount);


	for (size_t i = 0; i < buildCount; ++i) {
		Vector<int> p;
		stream.read(p);

		auto* b = new Building();
		game.map.buildings[p] = b;
		b->fileLoad(stream, jobMap);
		game.map.buildings[p] = b;
	}
}
