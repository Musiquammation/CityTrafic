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


















// Hash pour (x, y, dir)
struct StateHash {
	size_t operator()(const std::tuple<int,int,Direction>& k) const {
		auto h1 = std::hash<int>{}(std::get<0>(k));
		auto h2 = std::hash<int>{}(std::get<1>(k));
		auto h3 = std::hash<uint8_t>{}((uint8_t)std::get<2>(k));
		return h1 ^ (h2 * 2654435761u) ^ (h3 * 40503u);
	}
};

// Convertit une direction relative (front=1, right=2, left=3) en direction absolue
static Direction applyRelative(Direction current, int relative) {
	// relative: 1=front, 2=right(of current), 3=left(of current)
	switch (relative) {
		case 1: return current;                          // front = same dir
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

	Key destKey = {-1, -1, Direction::RIGHT}; // sera mis à jour à l'arrivée
	bool found = false;

	while (!pq.empty()) {
		auto [cost, curKey] = pq.top(); pq.pop();
		auto [x, y, dir] = curKey;

		// Skip si on a déjà trouvé mieux
		if (dist.count(curKey) && dist[curKey] < cost) continue;

		if (x == destX && y == destY) {
			destKey = curKey;
			found = true;
			break;
		}

		const Cell* cell = map.getCell(x, y);
		if (!cell) continue;

		CellType type = cell->getType();

		auto tryNeighbor = [&](Direction outDir, int extraWeight) {
			auto dv = DIRECTION_VECTORS[(int)outDir];
			int nx = x + dv.x;
			int ny = y + dv.y;

			const Cell* ncell = map.getCell(nx, ny);
			if (!ncell) return;

			CellType ntype = ncell->getType();
			if (ntype != CellType::ROAD &&
				ntype != CellType::DIRECTION &&
				ntype != CellType::PARKING) return;

			// Vérifier que la cellule voisine accepte l'entrée depuis outDir
			// (la cellule ROAD a des bits de connectivité +04..+07)
			if (ntype == CellType::ROAD) {
				// bit +04=right +05=up +06=left +07=down
				// On vérifie que la direction OPPOSÉE est ouverte sur la cellule voisine
				// (i.e. on peut entrer par la face opposée à outDir)
				Direction inDir = Direction_getOpposite(outDir);
				int bit = 4 + (int)inDir;
				if (!((ncell->data >> bit) & 1)) return;

				int roadWeight = (ncell->data >> 8) & 0x7F;
				int newCost = cost + 1 + extraWeight + roadWeight;
				Key nextKey = {nx, ny, outDir};
				if (!dist.count(nextKey) || dist[nextKey] > newCost) {
					dist[nextKey] = newCost;
					prev[nextKey] = {curKey, outDir};
					pq.push({newCost, nextKey});
				}
			} else {
				// DIRECTION ou PARKING : pas de vérification de connectivité entrante
				int newCost = cost + 1 + extraWeight;
				Key nextKey = {nx, ny, outDir};
				if (!dist.count(nextKey) || dist[nextKey] > newCost) {
					dist[nextKey] = newCost;
					prev[nextKey] = {curKey, outDir};
					pq.push({newCost, nextKey});
				}
			}
		};

		if (type == CellType::ROAD || type == CellType::PARKING) {
			// Avance uniquement dans la direction courante
			tryNeighbor(dir, 0);

		} else if (type == CellType::DIRECTION) {
			// getSide(data, d) avec d = direction courante du véhicule
			int side = direction::getSide(cell->data, (int)dir);
			// side : 0=nothing, 1=front, 2=right, 3=left,
			//        4=front-right, 5=front-left, 6=left-right, 7=front-left-right
			// Décompose en directions relatives autorisées
			static const int SIDE_DECOMP[8][3] = {
				{0, 0, 0},      // 0: nothing → treat as front
				{1, 0, 0},      // 1: front
				{2, 0, 0},      // 2: right
				{3, 0, 0},      // 3: left
				{1, 2, 0},      // 4: front-right
				{1, 3, 0},      // 5: front-left
				{2, 3, 0},      // 6: left-right (no front)
				{1, 2, 3},      // 7: front-left-right
			};
			for (int i = 0; i < 3; i++) {
				int rel = SIDE_DECOMP[side][i];
				if (rel == 0) break;
				Direction outDir = applyRelative(dir, rel);
				tryNeighbor(outDir, 0);
			}
		} else {
			// NONE, BUILDING, LINK : interdit
			continue;
		}
	}

	if (!found) return false;

	// Reconstruction du chemin (de destKey vers startKey)
	std::vector<Key> keyPath;
	std::vector<Direction> dirPath;
	Key cur = destKey;

	while (cur != startKey) {
		auto [px, py, pdir] = cur;
		keyPath.push_back(cur);
		auto& [parentKey, usedDir] = prev[cur];
		dirPath.push_back(usedDir);
		cur = parentKey;
	}
	keyPath.push_back(startKey);

	std::reverse(keyPath.begin(), keyPath.end());
	std::reverse(dirPath.begin(), dirPath.end());

	int len = (int)keyPath.size();

	// Allouer les tableaux
	Vector<int>* posArray = new Vector<int>[len];
	// dirArray compact : 2 bits par entrée
	int byteCount = (2 * len + 7) / 8;
	uint8_t* compactDirs = new uint8_t[byteCount]();

	for (int i = 0; i < len; i++) {
		auto [px, py, pd] = keyPath[i];
		posArray[i] = {px, py};
	}
	// dirPath[i] = direction utilisée pour QUITTER keyPath[i] vers keyPath[i+1]
	// On stocke len directions (la dernière = direction à l'arrivée, on peut mettre dir de destKey)
	for (int i = 0; i < (int)dirPath.size(); i++) {
		int bitIndex = 2 * i;
		int byteIndex = bitIndex / 8;
		int bitInByte = bitIndex % 8;
		compactDirs[byteIndex] |= uint8_t(((uint8_t)dirPath[i] & 0b11) << bitInByte);
	}
	// Dernière entrée : direction du véhicule à destination
	{
		auto [dx2, dy2, ddir] = destKey;
		int bitIndex = 2 * (len - 1);
		int byteIndex = bitIndex / 8;
		int bitInByte = bitIndex % 8;
		compactDirs[byteIndex] |= uint8_t(((uint8_t)ddir & 0b11) << bitInByte);
	}

	path.fill(posArray, compactDirs, len);
	return true;
}