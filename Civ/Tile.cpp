#include "stdafx.h"
#include "Tile.h"
#include <stdlib.h>
#include <vector>
#include "Resources.h"

bool Tile::add_forest()
{
	if (this->type == Desert) {
		return false;
	}
	this->cover = Forest;
	return true;
}

bool Tile::add_jungle()
{
	if (this->type == Desert) {
		return false;
	}
	if (this->cover == Forest) {
		return false;
	}
	this->cover = Jungle;
	return true;
}

int Tile::terrain_cost() const
{
	if (cover == Forest) {
		return 8;
	}
	else if (cover == Jungle) {
		return 8;
	}
	else if (height == Hill) {
		return 8;
	}
	else {
		return 4;
	}
}

Resources Tile::resources() const
{
	Resources r;
	if (cover == Forest) {
		r.food = 1;
		r.production = 1;
	}
	else if (cover == Jungle) {
		r.food = 2;
		r.production = 0;
	}
	else if (height == Hill) {
		r.food = 0;
		r.production = 2;
	}
	else if (type == Plains) {
		r.food = 1;
		r.production = 1;
	}
	else if (type == Desert) {
		r.food = 0;
		r.production = 0;
	}
	switch (improvement) {
	case Unimproved:
		break;
	case Farm:
		++r.food;
		break;
	case Mine:
		++r.production;
		break;
	case Mill:
		++r.production;
		break;
	}
	return r;
}

void generate_tiles(std::vector<std::vector<Tile>>& tiles, int width, int height)
{
	for (int y = 0; y < height; ++y) {
		std::vector<Tile> v;
		v.insert(v.begin(), width, Tile());
		tiles.push_back(v);
	}

	for (int i = 0; i < 6; ++i) {
		tiles[rand() % height][rand() % width].type = Desert;
	}
	for (int i = 0; i < 10; ++i) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if (x >= 1 && tiles[y][x - 1].type == Desert ||
					y >= 1 && tiles[y - 1][x].type == Desert ||
					x + 1 < width && tiles[y][x + 1].type == Desert ||
					y + 1 < height && tiles[y + 1][x].type == Desert) {
					if (rand() % 100 < 13) {
						tiles[y][x].type = Desert;
					}
				}
			}
		}
	}

	for (int i = 0; i < 8; ++i) {
		tiles[rand() % height][rand() % width].height = Hill;
	}
	for (int i = 0; i < 8; ++i) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if (x >= 1 && tiles[y][x - 1].height == Hill ||
					y >= 1 && tiles[y - 1][x].height == Hill ||
					x + 1 < width && tiles[y][x + 1].height == Hill ||
					y + 1 < height && tiles[y + 1][x].height == Hill) {
					if (rand() % 100 < 13) {
						tiles[y][x].height = Hill;
					}
				}
			}
		}
	}

	for (int i = 0; i < 13; ++i) {
		while (!tiles[rand() % height][rand() % width].add_forest()) {}
	}
	for (int i = 0; i < 7; ++i) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if (x >= 1 && tiles[y][x - 1].cover == Forest ||
					y >= 1 && tiles[y - 1][x].cover == Forest ||
					x + 1 < width && tiles[y][x + 1].cover == Forest ||
					y + 1 < height && tiles[y + 1][x].cover == Forest) {
					if (rand() % 100 < 17) {
						tiles[y][x].add_forest();
					}
				}
			}
		}
	}

	for (int i = 0; i < 13; ++i) {
		while (!tiles[rand() % height][rand() % width].add_jungle()) {}
	}
	for (int i = 0; i < 7; ++i) {
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				if (x >= 1 && tiles[y][x - 1].cover == Jungle ||
					y >= 1 && tiles[y - 1][x].cover == Jungle ||
					x + 1 < width && tiles[y][x + 1].cover == Jungle ||
					y + 1 < height && tiles[y + 1][x].cover == Jungle) {
					if (rand() % 100 < 17) {
						tiles[y][x].add_jungle();
					}
				}
			}
		}
	}
}
