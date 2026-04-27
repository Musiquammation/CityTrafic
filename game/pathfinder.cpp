#include "pathfinder.hpp"

#include "PathHandler.hpp"
#include "Map.hpp"


#include <queue>
#include <vector>
#include <cmath>
#include <unordered_map>

struct Node {
    int x, y;
    int g, h;
    int parentX, parentY;
    char dir;

    int f() const { return g + h; }
    bool operator>(const Node& other) const { return f() > other.f(); }
};

// Helper to hash coordinates for the closed list
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
    // Direction vectors: 0:R, 1:UR, 2:U, 3:UL, 4:L, 5:DL, 6:D, 7:DR
    const int dx[] = {1,  1,  0, -1, -1, -1,  0,  1};
    const int dy[] = {0,  1,  1,  1,  0, -1, -1, -1};
    const int costs[] = {10, 14, 10, 14, 10, 14, 10, 14};

    // 1. Check if we start on a ROAD and find the nearest NONE cell (Exit strategy)
    int curX = startX;
    int curY = startY;
    const Cell* startCell = map.getCell(curX, curY);
    
    if (startCell && startCell->getType() == CellType::ROAD) {
        int bestDist = 9999;
        int targetX = curX, targetY = curY;
        
        // Simple radial search to find nearest NONE cell
        for (int r = 1; r < 5; ++r) {
            for (int i = -r; i <= r; ++i) {
                for (int j = -r; j <= r; ++j) {
                    const Cell* c = map.getCell(curX + i, curY + j);
                    if (c && c->getType() == CellType::NONE) {
                        int d = i*i + j*j;
                        if (d < bestDist) {
                            bestDist = d;
                            targetX = curX + i;
                            targetY = curY + j;
                        }
                    }
                }
            }
            if (bestDist < 9999) break;
        }
        curX = targetX;
        curY = targetY;
    }

    // 2. A* Pathfinding
    std::priority_queue<Node, std::vector<Node>, std::greater<Node>> openList;
    std::unordered_map<Pos, Node, PosHash> closedList;

    auto getH = [&](int x, int y) {
        int dX = std::abs(x - destX);
        int dY = std::abs(y - destY);
        return 10 * (dX + dY) + (14 - 20) * std::min(dX, dY);
    };

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

            // Constraint: Only walk on NONE or the Destination
            if (nextCell && (nextCell->getType() == CellType::NONE || (nx == destX && ny == destY))) {
                if (closedList.count({nx, ny})) continue;

                int gScore = current.g + costs[i];
                openList.push({nx, ny, gScore, getH(nx, ny), current.x, current.y, (char)i});
            }
        }
    }

    if (!found) return nullptr;

    // 3. Reconstruct path
    std::vector<char> tempPath;
    Pos backtrack = finalPos;
    while (backtrack.x != curX || backtrack.y != curY) {
        Node& n = closedList[backtrack];
        tempPath.push_back(n.dir);
        backtrack = {n.parentX, n.parentY};
    }

    // Allocate result (path + end marker 8)
    char* result = (char*)malloc(tempPath.size() + 1);
    for (size_t i = 0; i < tempPath.size(); ++i) {
        // Reverse because we backtracked
        result[i] = tempPath[tempPath.size() - 1 - i];
    }
    result[tempPath.size()] = 8;

    return result;
}





bool makeCarPath(const Map& map, int startX, int startY, int destX, int destY) {
    return false;
}

