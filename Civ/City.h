#pragma once

#include <vector>
#include "Point.h"

class Player;
class Resources;
class State;

class CityCitizen
{
public:
	bool working;
	Point working_point;

	CityCitizen();
	CityCitizen(Point p);
};

class City
{
public:
	Player* player;
	std::vector<CityCitizen> workers;
	std::vector<std::vector<bool>> owned_points;

	City(Player* player, Point city_point, int width, int height);

	Resources resources_per_turn(const State&) const;
};
