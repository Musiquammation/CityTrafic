#include "jobSerializator.hpp"

#include <iosfwd>

#include "../jobs/AgricultorJob.hpp"
#include "../utils/streams.hpp"
#include "game/jobs/CashierJob.hpp"
#include "game/jobs/ConstructionJob.hpp"
#include "game/jobs/OilFieldJob.hpp"


enum class Types: int {
	AGRICULTOR,
	CASHIER,
	CONSTRUCTION,
	OIL_FIELD
};

template<typename U>
static void saveMap(WriteStream &stream, const std::map<Character*, U>& map) {
	for (auto& [key, value]: map) {
		stream.write(key);
		stream.write(value);
	}
	stream.write(nullptr);
}

void jobSerializator::save(WriteStream &stream, const Job *job) {
	if (auto* j = dynamic_cast<const AgricultorJob*>(job)) {
		stream.write(Types::AGRICULTOR);
		stream.write(j->salaryPerSeed);
		stream.write(j->pricePerSeed);
		stream.write(j->startTime);
		stream.write(j->finishTime);
		stream.write(j->employeesCounters);
		saveMap(stream, j->workers);
		return;
	}

	if (auto* j = dynamic_cast<const CashierJob*>(job)) {
		stream.write(Types::CASHIER);
		stream.write(j->salaryPerHour);
		stream.write(j->seedPrice);
		stream.write(j->startTime);
		stream.write(j->finishTime);
		stream.write(j->employeesCounters);
		saveMap(stream, j->workers);
		return;
	}

	if (auto* j = dynamic_cast<const ConstructionJob*>(job)) {
		stream.write(Types::CONSTRUCTION);
		stream.write(j->salaryPerUnit);
		stream.write(j->startTime);
		stream.write(j->finishTime);
		stream.write(j->employeesCounters);
		saveMap(stream, j->workers);
		return;
	}

	if (auto* j = dynamic_cast<const OilFieldJob*>(job)) {
		stream.write(Types::OIL_FIELD);
		stream.write(j->salaryPerLiter);
		stream.write(j->pricePerLiter);
		stream.write(j->startTime);
		stream.write(j->finishTime);
		stream.write(j->employeesCounters);
		saveMap(stream, j->workers);
		return;
	}

	throw std::runtime_error{"Job not implemented"};
}

template<typename U>
static void openMap(
	ReadStream &stream,
	std::map<Character*, U>& map,
	const std::unordered_map<Character*, Character*>& characterMap
) {
	while (true) {
		auto prev = stream.read<Character*>();
		if (prev == nullptr)
			break;

		if (auto it = characterMap.find(prev); it != characterMap.end()) {
			map[it->second] = stream.read<U>();
		} else {
			throw std::runtime_error{"Cannot find character"};
		}
	}
}



Job *jobSerializator::open(ReadStream &stream, const std::unordered_map<Character*, Character*>& characterMap) {
	auto type = stream.read<Types>();

	switch (type) {
		case Types::AGRICULTOR: {
			float salaryPerSeed = stream.read<float>();
			float pricePerSeed = stream.read<float>();
			auto j = new AgricultorJob{salaryPerSeed, pricePerSeed};
			stream.read(j->startTime);
			stream.read(j->finishTime);
			stream.read(j->employeesCounters);
			openMap(stream, j->workers, characterMap);
			return j;
		}

		case Types::CASHIER: {
			auto j = new CashierJob{};
			stream.read(j->salaryPerHour);
			stream.read(j->seedPrice);
			stream.read(j->startTime);
			stream.read(j->finishTime);
			stream.read(j->employeesCounters);
			openMap(stream, j->workers, characterMap);
			return j;
		}

		case Types::CONSTRUCTION: {
			auto salaryPerUnit = stream.read<float>();
			auto j = new ConstructionJob{salaryPerUnit};
			stream.read(j->startTime);
			stream.read(j->finishTime);
			stream.read(j->employeesCounters);
			openMap(stream, j->workers, characterMap);
			return j;
		}

		case Types::OIL_FIELD: {
			auto salaryPerLiter = stream.read<float>();
			auto pricePerLiter = stream.read<float>();
			auto j = new OilFieldJob{salaryPerLiter, pricePerLiter};
			stream.read(j->startTime);
			stream.read(j->finishTime);
			stream.read(j->employeesCounters);
			openMap(stream, j->workers, characterMap);
			return j;
		}
	}


	throw std::runtime_error{"Job not implemented"};
}
