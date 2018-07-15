#pragma once

#include <vector>
#include "Tile.h"
#include <SDL.h>
#include <map>
#include "Point.h"
#include <memory>

class Player;

enum RenderMode {
	RenderMilitary, RenderCivilians,
};

class State
{
	void handle_unit_attack_move(Point pressed_point);
	void zoom_in();
	void zoom_out();
	Point to_grid(Point mouse_point);
public:
	double xrel;
	double yrel;
	double zoom;
	bool mouse_down_unmoved;

	int width;
	int height;

	std::vector<std::vector<Tile>> tiles;
	Point selected_point;

	std::vector<std::unique_ptr<Player>> players;

	RenderMode render_mode;
	bool render_resources;

	int turn;

	State(int width, int height);
	~State();

	int advance_state();

	std::map<Point, std::pair<int, Point>> movement_tiles(const Point& tile) const;
	std::map<Point, std::pair<int, Point>> attack_tiles(const Point& tile) const;

	const Tile& tile(Point t) const;
	Tile& tile(Point t);

	bool render_military() const;
	bool render_civilians() const;
};

