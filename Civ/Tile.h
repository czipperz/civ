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

class Unit;
class Civilian;
class Resources;

class Tile
{
public:
	TileType type;
	TileCover cover;
	TileHeight height;
	Unit* unit;
	Civilian* civilian;

	bool add_forest();
	bool add_jungle();
	int terrain_cost() const;
	Resources resources() const;
};

void generate_tiles(std::vector<std::vector<Tile>>& tiles, int width, int height);
