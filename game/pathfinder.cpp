#include "pathfinder.hpp"

#include "PathHandler.hpp"
#include "Map.hpp"
#include "Direction.hpp"
#include "direction.hpp"

#include <queue>
#include <vector>
#include <cmath>
#include <unordered_map>
#include <limits.h>
#include <unordered_map>
#include <queue>
#include <vector>
#include <tuple>
#include <functional>



struct Node {
	int x, y;
	int g, h;
	int parentX, parentY;
	char dir;

	int f() const { return g + h; }
	bool operator>(const Node& other) const { return f() > other.f(); }
};

struct Pos {
	int x, y;
	bool operator==(const Pos& other) const { return x == other.x && y == other.y; }
};

struct PosHash {
	std::size_t operator()(const Pos& p) const {
		return std::hash<int>{}(p.x) ^ (std::hash<int>{}(p.y) << 1);
	}
};

char* makePedestranPath(const Map& map, int startX, int startY, int destX, int destY) {
	// Directions: 0:R, 1:UR, 2:U, 3:UL, 4:L, 5:DL, 6:D, 7:DR
	const int dx[] = {1,  1,  0, -1, -1, -1,  0,  1};
	const int dy[] = {0, -1, -1, -1,  0,  1,  1,  1};
	const int costs[] = {10, 14, 10, 14, 10, 14, 10, 14};

	int curX = startX;
	int curY = startY;
	const Cell* startCell = map.getCell(curX, curY);
	
	if (!startCell) return nullptr;

	// 1. ESCAPE STRATEGY
	// If the pedestrian starts on a ROAD or inside a BUILDING/LINK (any non-NONE cell),
	// we must find the nearest valid NONE cell to begin the pathfinding.
	if (startCell->getType() != CellType::NONE) {
		int bestDist = 999999;
		int targetX = curX, targetY = curY;
		bool foundExit = false;

		// Radial search (up to 10 cells away) to find the nearest walkable ground
		for (int r = 1; r <= 10; ++r) {
			for (int i = -r; i <= r; ++i) {
				for (int j = -r; j <= r; ++j) {
					// We only check the perimeter of the current radius 'r'
					if (abs(i) != r && abs(j) != r) continue; 

					const Cell* c = map.getCell(curX + i, curY + j);
					if (c && c->getType() == CellType::NONE) {
						int d = i*i + j*j;
						if (d < bestDist) {
							bestDist = d;
							targetX = curX + i;
							targetY = curY + j;
							foundExit = true;
						}
					}
				}
			}
			if (foundExit) break; 
		}
		curX = targetX;
		curY = targetY;
	}

	// 2. A* ALGORITHM
	std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
	std::unordered_map<Pos, Node, PosHash> closedList;

	auto getH = [&](int x, int y) {
		int dX = std::abs(x - destX);
		int dY = std::abs(y - destY);
		// Octile distance heuristic
		// printf("dist %d %d\n", dX, dY);
		return 10 * (dX + dY) + (14 - 20) * std::min(dX, dY);
	};

	// Starting node (dir 8 means 'start' or 'end')
	openList.push({curX, curY, 0, getH(curX, curY), -1, -1, 8});

	bool found = false;
	Pos finalPos = {curX, curY};

	while (!openList.empty()) {
		Node current = openList.top();
		openList.pop();

		Pos curPos = {current.x, current.y};
		if (closedList.count(curPos)) continue;
		closedList[curPos] = current;

		// Destination reached
		if (current.x == destX && current.y == destY) {
			finalPos = curPos;
			found = true;
			break;
		}

		for (int i = 0; i < 8; ++i) {
			int nx = current.x + dx[i];
			int ny = current.y + dy[i];
			
			const Cell* nextCell = map.getCell(nx, ny);
			if (!nextCell) continue;

			// Only walk on NONE cells
			// Special case: we allow the destination cell even if it's not NONE
			// (e.g., if the destination is the edge of a building)
			bool isWalkable = (nextCell->getType() == CellType::NONE);
			bool isDest = (nx == destX && ny == destY);

			if (isWalkable || isDest) {
				if (closedList.count({nx, ny})) continue;

				int gScore = current.g + costs[i];
				openList.push({nx, ny, gScore, getH(nx, ny), current.x, current.y, (char)i});
			}
		}
	}

	if (!found) return nullptr;

	// 3. PATH RECONSTRUCTION
	std::vector<char> tempPath;
	Pos backtrack = finalPos;
	while (backtrack.x != curX || backtrack.y != curY) {
		Node& n = closedList[backtrack];
		tempPath.push_back(n.dir);
		backtrack = {n.parentX, n.parentY};
	}

	// Allocate result buffer: path length + 1 byte for the final '8'
	char* result = (char*)malloc(tempPath.size() + 1);
	if (!result) return nullptr;

	for (size_t i = 0; i < tempPath.size(); ++i) {
		// Reverse the order (backtrack goes from end to start)
		result[i] = tempPath[tempPath.size() - 1 - i];
	}
	result[tempPath.size()] = 8; // Terminator

	return result;
}


















// Hash for (x, y, dir)
struct StateHash {
	size_t operator()(const std::tuple<int,int,Direction>& k) const {
		auto h1 = std::hash<int>{}(std::get<0>(k));
		auto h2 = std::hash<int>{}(std::get<1>(k));
		auto h3 = std::hash<uint8_t>{}((uint8_t)std::get<2>(k));
		return h1 ^ (h2 * 2654435761u) ^ (h3 * 40503u);
	}
};

// Converts a relative direction (front=1, right=2, left=3) into an absolute direction
static Direction applyRelative(Direction current, int relative) {
	// relative: 1=front, 2=right (of current), 3=left (of current)
	switch (relative) {
		case 1: return current;                          // front = same direction
		case 2: return Direction_getRight(current);      // turn right
		case 3: return Direction_getLeft(current);       // turn left
		default: return current;
	}
}



bool makeCarPath(
	const Map& map, PathHandler<true>& path,
	int startX, int startY, int destX, int destY,
	Direction startDir
) {
	using Key = std::tuple<int, int, Direction>;
	using State = std::pair<int, Key>; // (cost, key)

	std::unordered_map<Key, int, StateHash> dist;
	std::unordered_map<Key, std::pair<Key, Direction>, StateHash> prev;
	// prev[key] = { previousKey, directionUsedToArrive }

	std::priority_queue<State, std::vector<State>, std::greater<State>> pq;

	Key startKey = {startX, startY, startDir};
	dist[startKey] = 0;
	pq.push({0, startKey});

	Key destKey = {-1, -1, Direction::RIGHT}; // will be updated when reached
	bool found = false;

	while (!pq.empty()) {
		// Extract current state (avoid structured bindings issues)
		State top = pq.top();
		pq.pop();

		int cost = top.first;
		Key curKey = top.second;

		int x = std::get<0>(curKey);
		int y = std::get<1>(curKey);
		Direction dir = std::get<2>(curKey);

		// Skip if we already found a better path
		if (dist.count(curKey) && dist[curKey] < cost)
			continue;

		// Destination reached
		if (x == destX && y == destY) {
			destKey = curKey;
			found = true;
			break;
		}

		const Cell* cell = map.getCell(x, y);
		if (!cell)
			continue;

		CellType type = cell->getType();

		// Lambda to try a neighbor transition
		auto tryNeighbor = [&](Direction outDir, int extraWeight) {
			auto dv = DIRECTION_VECTORS[(int)outDir];

			int nx = x + dv.x;
			int ny = y + dv.y;

			const Cell* ncell = map.getCell(nx, ny);
			if (!ncell)
				return;

			CellType ntype = ncell->getType();

			// Only traversable cell types
			if (ntype != CellType::ROAD &&
				ntype != CellType::DIRECTION &&
				ntype != CellType::PARKING)
				return;

			Key nextKey = {nx, ny, outDir};

			if (ntype == CellType::ROAD) {
				// Check connectivity (can we enter from opposite direction)
				Direction inDir = Direction_getOpposite(outDir);
				int bit = 4 + (int)inDir;

				if (!((ncell->data >> bit) & 1))
					return;

				int roadWeight = (ncell->data >> 8) & 0x7F;
				int newCost = cost + 1 + extraWeight + roadWeight;

				if (!dist.count(nextKey) || dist[nextKey] > newCost) {
					dist[nextKey] = newCost;
					prev[nextKey] = {curKey, outDir};
					pq.push({newCost, nextKey});
				}
			}
			else {
				// DIRECTION or PARKING: no connectivity constraint
				int newCost = cost + 1 + extraWeight;

				if (!dist.count(nextKey) || dist[nextKey] > newCost) {
					dist[nextKey] = newCost;
					prev[nextKey] = {curKey, outDir};
					pq.push({newCost, nextKey});
				}
			}
		};

		if (type == CellType::ROAD || type == CellType::PARKING) {
			// Move forward only
			tryNeighbor(dir, 0);
		}
		else if (type == CellType::DIRECTION) {
			// Determine allowed relative directions
			int side = direction::getSide(cell->data, (int)dir);

			// Decomposition of side into relative moves
			static const int SIDE_DECOMP[8][3] = {
				{0, 0, 0},      // 0: nothing
				{1, 0, 0},      // 1: front
				{2, 0, 0},      // 2: right
				{3, 0, 0},      // 3: left
				{1, 2, 0},      // 4: front-right
				{1, 3, 0},      // 5: front-left
				{2, 3, 0},      // 6: left-right
				{1, 2, 3},      // 7: front-left-right
			};

			for (int i = 0; i < 3; i++) {
				int rel = SIDE_DECOMP[side][i];
				if (rel == 0)
					break;

				Direction outDir = applyRelative(dir, rel);
				tryNeighbor(outDir, 0);
			}
		}
		else {
			// Non-walkable cell
			continue;
		}
	}

	if (!found)
		return false;

	// Reconstruct path from destination to start
	std::vector<Key> keyPath;
	std::vector<Direction> dirPath;

	Key cur = destKey;

	while (cur != startKey) {
		keyPath.push_back(cur);

		auto it = prev.find(cur);
		if (it == prev.end())
			break;

		Key parentKey = it->second.first;
		Direction usedDir = it->second.second;

		dirPath.push_back(usedDir);
		cur = parentKey;
	}

	keyPath.push_back(startKey);

	std::reverse(keyPath.begin(), keyPath.end());
	std::reverse(dirPath.begin(), dirPath.end());

	int len = (int)keyPath.size();

	// Allocate arrays
	Vector<int>* posArray = new Vector<int>[len];

	// Compact direction array (2 bits per entry)
	int byteCount = (2 * len + 7) / 8;
	uint8_t* compactDirs = new uint8_t[byteCount]();

	for (int i = 0; i < len; i++) {
		int px = std::get<0>(keyPath[i]);
		int py = std::get<1>(keyPath[i]);

		posArray[i] = {px, py};
	}

	// Encode directions
	for (int i = 0; i < (int)dirPath.size(); i++) {
		int bitIndex = 2 * i;
		int byteIndex = bitIndex / 8;
		int bitInByte = bitIndex % 8;

		compactDirs[byteIndex] |= uint8_t(((uint8_t)dirPath[i] & 0b11) << bitInByte);
	}

	// Last direction = direction at destination
	{
		Direction ddir = std::get<2>(destKey);

		int bitIndex = 2 * (len - 1);
		int byteIndex = bitIndex / 8;
		int bitInByte = bitIndex % 8;

		compactDirs[byteIndex] |= uint8_t(((uint8_t)ddir & 0b11) << bitInByte);
	}

	path.fill(posArray, compactDirs, len);
	return true;
}