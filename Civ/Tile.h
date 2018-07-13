#pragma once

#include <vector>

enum TileType {
	Plains, Desert,
};
enum TileCover {
	NoCover, Forest, Jungle,
};
enum TileHeight {
	Flat, Hill,
};
enum TileImprovement {
	Unimproved, Farm, Mine, Mill,
};

class MilitaryUnit;
class CivilianUnit;
class City;
class Player;
class Resources;

class Tile
{
public:
	TileType type = Plains;
	TileCover cover = NoCover;
	TileHeight height = Flat;
	TileImprovement improvement = Unimproved;
	MilitaryUnit* military = 0;
	CivilianUnit* civilian = 0;
	City* city = 0;
	Player* player = 0;

	bool add_forest();
	bool add_jungle();
	int terrain_cost() const;
	Resources resources() const;
};

void generate_tiles(std::vector<std::vector<Tile>>& tiles, int width, int height);
