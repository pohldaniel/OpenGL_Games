#include <list>
#include <limits>
#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdlib>
#include <cmath>
#include <array>

#include "Astar.h"
#include "Enums.h"
#include "Zone.h"


struct SearchListStruct {
	std::array<int,2> p;
	int value;
	int estimatedF;
	Enums::Direction predDir;

	SearchListStruct(const std::array<int,2> &p_, int value_, int estimatedF_, Enums::Direction predDir_)
		: p(p_),
		value(value_),
		estimatedF(estimatedF_),
		predDir(predDir_)
	{
	}

	bool operator == (const SearchListStruct &other) const
	{
		return (p[0] == other.p[0] && p[1] == other.p[1]);
	}

	bool operator == (const std::array<int, 2> &point) const
	{
		return (p[0] == point[0] && p[1] == point[1]);
	}
};

std::list< SearchListStruct > openList;
std::list< SearchListStruct > closedList;
int offsetX[] = { 0,  0,  1,  1,  1,  0, -1, -1, -1 };
int offsetY[] = { 0,  1,  1,  0, -1, -1, -1,  0,  1 };
int movePoints[] = { 0, 10, 14, 10, 14, 10, 14, 10, 14 };
Enums::Direction dirToPredecessor[] = { Enums::STOP, Enums::S, Enums::SW, Enums::W, Enums::NW, Enums::N, Enums::NE, Enums::E, Enums::SE };
int counter = 0;
int maxValue = 0;

/* this is the step-width for neighbours in pixels.
a bigger value makes the value slightly suboptimal
(and no path may be found through very narrow passages)
but the heuristics gets *much* faster. */
int distanceSkip = 10;

SearchListStruct popMinimumElement(std::list<SearchListStruct> & list) {
	assert(list.size() > 0);
	// note: This can be drastically improved if the list is kept sorted
	// inserting will not be very expensive in that case either
	int minEstimate = std::numeric_limits<int>::max();
	std::list<SearchListStruct>::iterator minIt = list.begin();
	for (std::list<SearchListStruct>::iterator it = list.begin(); it != list.end(); ++it) {
		if (it->estimatedF < minEstimate){
			minIt = it;
			minEstimate = it->estimatedF;
		}
	}

	SearchListStruct minPoint = *minIt;
	list.erase(minIt);
	return minPoint;
}

bool hasIntersection(int r1_l, int r1_r, int r1_b, int r1_t, int r2_l, int r2_r, int r2_b, int r2_t){
	return (!((r1_t < r2_b) || (r1_b > r2_t) || (r1_l > r2_r) || (r1_r < r2_l)));
}

bool isFree(int px, int py, int w, int h) {
	//Zone *curZone = 

	std::vector<CollisionRect>& collisionMap = ZoneManager::Get().getCurrentZone()->getCollisionMap();

	for (unsigned int t = 0; t < collisionMap.size(); t++) {
		int other_l = collisionMap[t].x, other_r = collisionMap[t].x + collisionMap[t].w;
		int other_b = collisionMap[t].y, other_t = collisionMap[t].y + collisionMap[t].h;
		if (hasIntersection(other_l, other_r, other_b, other_t,
			px, px + w, py, py + h)) {
			return false;
		}
	}

	return true;
}

SearchListStruct closestNodeFound(std::array<int, 2>{0, 0}, 0, 0, Enums::STOP);
int numIterations;

void expandNode(SearchListStruct& currentNode, const std::array<int, 2>& end, int width, int height) {
	++numIterations;
	/* std::cout << "expanding node ( (" << currentNode.p.x<< ","
	<< currentNode.p.y << "), v: " << currentNode.value  << ",
	f: " << currentNode.estimatedF << ", dir: " << currentNode.
	predDir << ")" << std::endl; */

	for (int dir = 1; dir <= 8; ++dir) {
		SearchListStruct successor(std::array<int, 2>{currentNode.p[0] + offsetX[dir] * distanceSkip, currentNode.p[1] + offsetY[dir] * distanceSkip}, 0, 0, Enums::STOP);

		if (!isFree(successor.p[0], successor.p[1], width, height)) {
			continue;
		}

		if (std::find(closedList.begin(), closedList.end(), successor) != closedList.end()) {
			continue;
		}

		int tentative_g = currentNode.value + movePoints[dir] * distanceSkip;

		std::list<SearchListStruct>::iterator sucOpenIt = std::find(openList.begin(),
			openList.end(),
			successor);
		if (sucOpenIt != openList.end() && tentative_g >= sucOpenIt->value) {
			continue;
		}
		successor.predDir = dirToPredecessor[dir];
		successor.value = tentative_g;

		int oddMoves = std::min(abs(end[0] - currentNode.p[0]), abs(end[1] - currentNode.p[1]));
		int evenMoves = std::abs(end[0] - currentNode.p[0]) + std::abs(end[1] - currentNode.p[1]) - 2 * oddMoves;
		int f = tentative_g + 10 * evenMoves + 14 * oddMoves;

		if (sucOpenIt != openList.end()) {
			sucOpenIt->predDir = successor.predDir;
			sucOpenIt->value = successor.value;
			sucOpenIt->estimatedF = f;
			if (std::pow(closestNodeFound.p[0] - end[0], 2) +
				std::pow(closestNodeFound.p[0] - end[0], 2) >
				std::pow(sucOpenIt->p[0] - end[0], 2) +
				std::pow(sucOpenIt->p[1] - end[1], 2)) {
				closestNodeFound = *sucOpenIt;
			}

		} else{
			successor.estimatedF = f;
			openList.push_front(successor);
		}
	}
}

std::vector<std::array<int, 2>> aStar(const std::array<int, 2>& start,
	const std::array<int, 2>& end,
	int width,
	int height,
	int granularity,
	int maxIterations) {
	openList.clear();
	closedList.clear();
	openList.push_back(SearchListStruct(start, 0, std::numeric_limits<int>::max(), Enums::STOP));
	counter = 0;
	closestNodeFound = SearchListStruct(start, 0, std::numeric_limits<int>::max(), Enums::STOP);
	numIterations = 0;
	distanceSkip = granularity;

	while (openList.size() > 0) {
		SearchListStruct currentNode = popMinimumElement(openList);

		if (numIterations >= maxIterations ||
			std::pow(currentNode.p[0] - end[0], 2) +
			std::pow(currentNode.p[1] - end[1], 2) <
			distanceSkip * 14) {
			if (numIterations >= maxIterations) {
				currentNode = closestNodeFound;
			}

			// Return NodeList
			std::vector<std::array<int,2>> nodeList;
			nodeList.push_back(currentNode.p);
			while (!(currentNode == start)) {
				std::array<int, 2> predNode{ currentNode.p[0] + offsetX[currentNode.predDir] * distanceSkip, currentNode.p[1] + offsetY[currentNode.predDir] * distanceSkip };
				nodeList.push_back(predNode);
				std::list<SearchListStruct>::iterator it = std::find(closedList.begin(), closedList.end(), SearchListStruct(predNode, 0, 0, Enums::STOP));
				assert(it != closedList.end());
				currentNode = *it;
			}
			nodeList.push_back(start);

			return nodeList;
		}

		expandNode(currentNode, end, width, height);
		closedList.push_back(currentNode);
	}

	// Return NoPathFound
	return std::vector<std::array<int, 2>>();
}


/*
// überprüft alle Nachfolgeknoten und fügt sie der Open List hinzu, wenn entweder
// - der Nachfolgeknoten zum ersten Mal gefunden wird oder
// - ein besserer Weg zu diesem Knoten gefunden wird
function expandNode(currentNode)
foreach successor of currentNode
// wenn der Nachfolgeknoten bereits auf der Closed List ist - tue nichts
if closedlist.contains(successor) then
continue
// g Wert für den neuen Weg berechnen: g Wert des Vorgängers plus
// die Kosten der gerade benutzten Kante
tentative_g = g(currentNode) + c(currentNode, successor)
// wenn der Nachfolgeknoten bereits auf der Open List ist,
// aber der neue Weg nicht besser ist als der alte - tue nichts
if openlist.contains(successor) and tentative_g >= g[successor] then
continue
// Vorgängerzeiger setzen und g Wert merken
successor.predecessor := currentNode
g[successor] = tentative_g
// f Wert des Knotens in der Open List aktualisieren
// bzw. Knoten mit f Wert in die Open List einfügen
f := tentative_g + h(successor)
if openlist.contains(successor) then
openlist.decreaseKey(successor, f)
else
openlist.enqueue(successor, f)
end
end


program a-star
// Initialisierung der Open List, die Closed List ist noch leer
// (die Priorität bzw. der f Wert des Startknotens ist unerheblich)
openlist.enqueue(startknoten, 0)
// diese Schleife wird durchlaufen bis entweder
// - die optimale Lösung gefunden wurde oder
// - feststeht, dass keine Lösung existiert
repeat
// Knoten mit dem geringsten f Wert aus der Open List entfernen
currentNode := openlist.removeMin()
// Wurde das Ziel gefunden?
if currentNode == zielknoten then
return PathFound
// Wenn das Ziel noch nicht gefunden wurde: Nachfolgeknoten
// des aktuellen Knotens auf die Open List setzen
expandNode(currentNode)
// der aktuelle Knoten ist nun abschließend untersucht
closedlist.add(currentNode)
until openlist.isEmpty()
// die Open List ist leer, es existiert kein Pfad zum Ziel
return NoPathFound
end
*/
