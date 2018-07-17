#include "stdafx.h"
#include "City.h"
#include "Resources.h"
#include "State.h"

City::City(Player* player, Point city_point, int width, int height)
	: player(player)
{
	owned_points.insert(owned_points.begin(), height, std::vector<bool>(width, false));
	workers.push_back(CityCitizen { city_point });
	owned_points[city_point.y][city_point.x] = true;
}

Resources City::resources_per_turn(const State& state) const
{
	Resources r;
	r.food = 0;
	r.production = 0;
	for (auto& c : workers) {
		if (c.working) {
			const Tile& t = state.tile(c.working_point);
			r += t.resources();
		}
		else {
			r.production += 2;
		}
	}
	return r;
}

CityCitizen::CityCitizen()
	: working(false)
	, working_point({ -1, -1 })
{
}

CityCitizen::CityCitizen(Point p)
	: working(true)
	, working_point(p)
{
}
