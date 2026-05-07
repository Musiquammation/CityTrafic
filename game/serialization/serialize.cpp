#include "serialize.hpp"

#include <cmath>

#include "jobSerializator.hpp"
#include "../utils/streams.hpp"
#include "../Game.hpp"
#include "../Character.hpp"
#include "../Car.hpp"
#include "../Building.hpp"

void serialize::save(const Game &game, WriteStream &stream) {
	// Save map
	stream.write(game.map.x);
	stream.write(game.map.y);
	stream.write(game.map.width);
	stream.write(game.map.height);

	// Copy map
	int mapLength = (game.map.width - game.map.x) *
		(game.map.height - game.map.y);
		
	stream.copy(game.map.cells, (size_t)mapLength);


	// Save cars
	stream.write(game.carHandler.cars.size());
	for (auto it: game.carHandler.cars) {
		Car* car = it.second;
		stream.write(car);
		stream.write(car);
	}

	// Save characters
	stream.write(game.characterHandler.characters.size());
	for (Character* c: game.characterHandler.characters) {
		stream.write(c);
		stream.write(*c);
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
		Job* job = b->getJob();
		stream.write(job);

		jobSerializator::save(stream, job);
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

	game.map = Map{
		stream.read<int>(),
		stream.read<int>(),
		stream.read<int>(),
		stream.read<int>(),
	};

	// Restore map cells
	int mapLength = (game.map.width - game.map.x) * (game.map.height - game.map.y);


	// Map to link old pointers to new instances
	std::unordered_map<Car*, Car*> carMap;

	// Load cars
	size_t carCount;
	stream.read(carCount);
	for (size_t i = 0; i < carCount; ++i) {
		Car* oldPtr;
		stream.read(oldPtr);

		Car* newCar = (Car*)::operator new(sizeof(Car));
		stream.read(*newCar);

		game.carHandler.cars[{
			(int)std::floor(newCar->x),
			(int)std::floor(newCar->y),
		}] = newCar;

		carMap[oldPtr] = newCar;
	}

	// Load characters
	size_t charCount;
	std::unordered_map<Character*, Character*> characterMap;
	stream.read(charCount);
	for (size_t i = 0; i < charCount; ++i) {
		Character* oldPtr;
		stream.read(oldPtr);

		Character* c = new Character{};
		stream.read(*c);
		characterMap[oldPtr] = c;

		// Update car pointer using hash map
		if (c->car) {
			c->car = carMap[c->car];
		}

		game.characterHandler.characters.push_back(c);
	}

	// Calendar
	stream.read(game.calendar.indicator);

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
