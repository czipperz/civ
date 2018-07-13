#include "stdafx.h"
#include "Unit.h"
#include <SDL.h>
#include <utility>
#include <map>
#include "Tile.h"
#include "Player.h"
#include <algorithm>

std::map<Point, std::pair<int, Point>> Unit::movement_tiles(Point p, const std::vector<std::vector<Tile>>& tiles)
{
	std::map<Point, std::pair<int, Point>> paths1;
	std::map<Point, std::pair<int, Point>> paths2;
	auto* paths_in = &paths1;
	auto* paths_out = &paths2;

	paths_out->insert(std::make_pair(p, std::make_pair(movement, Point{})));

	bool force_continue;
	do {
		force_continue = false;
		std::swap(paths_in, paths_out);
		paths_out->clear();
		for (const auto& path : *paths_in) {
			auto it = paths_out->find(path.first);
			if (it == paths_out->end()) {
				paths_out->insert(path);
			}
			else if (it->second.first < path.second.first) {
				// replace only if have higher movement points left
				it->second = path.second;
			}
			paths_out->insert(path);
			if (path.second.first > 0) {
				for (int i = 0; i < 4; ++i) {
					auto p = path.first;
					if (i == 0 && p.x >= 1) { --p.x; }
					else if (i == 1 && p.y >= 1) { --p.y; }
					else if (i == 2 && p.x + 1 < tiles[0].size()) { ++p.x; }
					else if (i == 3 && p.y + 1 < tiles.size()) { ++p.y; }
					else { continue; }
					auto movement_points = path.second.first - tiles[p.y][p.x].terrain_cost();
					auto it = paths_out->find(p);
					if (it == paths_out->end()) {
						paths_out->insert(std::make_pair(p, std::make_pair(movement_points, path.first)));
					}
					else if (it->second.first < movement_points) {
						// replace only if have higher movement points left
						it->second = std::make_pair(movement_points, path.first);
						force_continue = true;
					}
				}
			}
		}
	} while (force_continue || paths_in->size() != paths_out->size());

	paths_out->erase(p);
	return *paths_out;
}

MilitaryUnit::MilitaryUnit(Player* p, MilitaryUnitType type)
	: type(type)
	, attacks(1)
{
	player = p;
	health = max_health();
	movement = max_movement();
}

int MilitaryUnit::max_health()
{
	switch (type) {
	case RockSlinger:
		return 10;
	case Clubber:
		return 13;
	}
}

int MilitaryUnit::max_movement()
{
	switch (type) {
	case RockSlinger:
	case Clubber:
		return 8;
	}
}

int MilitaryUnit::max_attack()
{
	switch (type) {
	case RockSlinger:
		return 4;
	case Clubber:
		return 5;
	}
}

bool MilitaryUnit::is_melee()
{
	switch (type) {
	case RockSlinger:
		return false;
	case Clubber:
		return true;
	}
}

void MilitaryUnit::kill()
{
	player->military_units.erase(std::find_if(
		player->military_units.begin(),
		player->military_units.end(),
		[&](const std::unique_ptr<MilitaryUnit>& civilian) {
			return civilian.get() == this;
		}));
}

CivilianUnit::CivilianUnit(Player* p, CivilianUnitType type)
	: type(type)
{
	player = p;
	health = max_health();
	movement = max_movement();
}

int CivilianUnit::max_health()
{
	switch (type) {
	case Worker:
		return 8;
	case Settler:
		return 8;
	}
}

int CivilianUnit::max_movement()
{
	switch (type) {
	case Worker:
		return 8;
	case Settler:
		return 8;
	}
}

void CivilianUnit::kill()
{
	player->civilian_units.erase(std::find_if(
		player->civilian_units.begin(),
		player->civilian_units.end(),
		[&](const std::unique_ptr<CivilianUnit>& civilian) {
			return civilian.get() == this;
		}));
}
