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
class Resources;

class Tile
{
public:
	TileType type;
	TileCover cover;
	TileHeight height;
	TileImprovement improvement;
	MilitaryUnit* military;
	CivilianUnit* civilian;

	bool add_forest();
	bool add_jungle();
	int terrain_cost() const;
	Resources resources() const;
};

void generate_tiles(std::vector<std::vector<Tile>>& tiles, int width, int height);
