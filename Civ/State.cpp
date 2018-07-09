#include "stdafx.h"
#include "State.h"
#include "Renderer.h"
#include <SDL.h>
#include <assert.h>
#include "Unit.h"

State::State(int width, int height)
	: xrel(Renderer::border)
	, yrel(Renderer::border)
	, zoom(.5)
	, mouse_down(false)
	, width(width)
	, height(height)
	, selected_tile({ -1, -1 })
	, render_military(true)
	, render_civilians(true)
	, render_resources(false)
{
	generate_tiles(tiles, width, height);
}

int State::advance_state()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			if (event.key.keysym.sym == 'q') {
				return 1;
			}
			if (event.key.keysym.sym == 'm') {
				render_military = !render_military;
			}
			if (event.key.keysym.sym == 'c') {
				render_civilians = !render_civilians;
			}
			if (event.key.keysym.sym == 'r') {
				render_resources = !render_resources;
			}
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			mouse_down = true;
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			if (mouse_down) {
				printf("Grid (%.0lf, %.0lf)\n", (event.button.x - xrel) / zoom, (event.button.y - yrel) / zoom);
				Point pressed_tile(
					((event.button.x - xrel) / zoom - Renderer::border) / (Renderer::dim + Renderer::border),
					((event.button.y - yrel) / zoom - Renderer::border) / (Renderer::dim + Renderer::border));
				int px = ((pressed_tile.x + 1) * (Renderer::dim + Renderer::border)) * zoom + xrel;
				int py = ((pressed_tile.y + 1) * (Renderer::dim + Renderer::border)) * zoom + yrel;
				if (event.button.x < px && event.button.y < py) {
					printf("Button press (%d, %d) (%d, %d)\n", pressed_tile.x, pressed_tile.y, event.button.x, event.button.y);
					if (event.button.button == SDL_BUTTON_LEFT) {
						selected_tile = pressed_tile;
					}
					if (event.button.button == SDL_BUTTON_RIGHT) {
						if (selected_tile.y != -1 && selected_tile.x != -1) {
							const auto mtiles = movement_tiles(selected_tile);
							auto it = mtiles.find(pressed_tile);
							if (it != mtiles.end()) {
								Unit** selected_unit = &tile(selected_tile).unit;
								assert(*selected_unit);
								Unit*& pressed_unit = tile(pressed_tile).unit;
								// TODO: implement "fast movement" where ranged attack range != movement
								if (pressed_unit) {
									if ((*selected_unit)->attacks > 0) {
										if ((*selected_unit)->is_melee()) {
											Unit*& move_dest = tile(it->second.second).unit;
											if (move_dest != *selected_unit) {
												assert(!move_dest);
												(*selected_unit)->movement = mtiles.find(it->second.second)->second.first;
												move_dest = *selected_unit;
												*selected_unit = NULL;
												selected_tile = it->second.second;
											}
											selected_unit = &move_dest;
										}
										switch (unit_attack(*selected_unit, pressed_unit, tile(pressed_tile).terrain_cost())) {
										case 0:
											break;
										case 1:
											selected_tile = pressed_tile;
											break;
										case 2:
											selected_tile = { -1, -1 };
											break;
										}
									}
								}
								else {
									(*selected_unit)->movement = it->second.first;
									pressed_unit = *selected_unit;
									*selected_unit = NULL;
									selected_tile = pressed_tile;
								}
							}
						}
					}
				}
				else {
					selected_tile = { -1, -1 };
				}
			}
		}
		if (event.type == SDL_MOUSEMOTION) {
			mouse_down = false;
			if (event.motion.state & SDL_BUTTON_LMASK) {
				xrel += event.motion.xrel;
				yrel += event.motion.yrel;
				//printf("movement: (%lf, %lf)\n", event.motion.xrel / zoom, event.motion.yrel);
			}
		}
		if (event.type == SDL_MOUSEWHEEL) {
			if (event.wheel.y > 0) {
				if (zoom < .75) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					double new_zoom = zoom + .125;
					xrel -= x * zoom;
					yrel -= y * zoom;
					zoom = new_zoom;
				}
			}
			if (event.wheel.y < 0) {
				if (zoom > .375) {
					int x, y;
					SDL_GetMouseState(&x, &y);
					double new_zoom = zoom - .125;
					xrel += x * zoom;
					yrel += y * zoom;
					zoom = new_zoom;
				}
			}
		}
	}
	return 0;
}

std::map<Point, std::pair<int, Point>> State::movement_tiles(const Point& tile) const
{
	if (tile.y != -1 && tile.x != -1) {
		Unit* unit = tiles[tile.y][tile.x].unit;
		if (unit) {
			return unit->movement_tiles(tile, tiles);
		}
	}
	return {};
}

const Tile & State::tile(Point t) const
{
	return tiles[t.y][t.x];
}

Tile & State::tile(Point t)
{
	return tiles[t.y][t.x];
}
