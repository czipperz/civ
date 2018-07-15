#include "stdafx.h"
#include "State.h"
#include "Renderer.h"
#include <SDL.h>
#include <assert.h>
#include "Unit.h"
#include "Player.h"
#include <algorithm>

State::State(int width, int height)
	: xrel(Renderer::border)
	, yrel(Renderer::border)
	, zoom(.5)
	, mouse_down_unmoved(false)
	, width(width)
	, height(height)
	, selected_point({ -1, -1 })
	, render_mode(RenderCivilians)
	, render_resources(true)
{
	generate_tiles(tiles, width, height);
}

State::~State()
{
}

int State::advance_state()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN) {
			switch (event.key.keysym.sym) {
			case 'q':
				return 1;
			case 'm':
				render_mode = RenderMilitary;
				render_resources = false;
				break;
			case 'c':
				render_mode = RenderCivilians;
				render_resources = !render_resources;
				break;
			case 'i':
				if (selected_point.y != -1) {
					Tile& selected_tile = tile(selected_point);
					if (render_civilians() && selected_tile.civilian) {
						if (selected_tile.civilian->type == Worker && selected_tile.player == selected_tile.civilian->player) {
							if (selected_tile.cover == Forest) {
								selected_tile.improvement = Mill;
							}
							else if (selected_tile.cover == Jungle) {
							}
							else if (selected_tile.height == Hill) {
								selected_tile.improvement = Mine;
							}
							else if (selected_tile.type == Plains) {
								selected_tile.improvement = Farm;
							}
							else if (selected_tile.type == Desert) {
								selected_tile.improvement = Farm;
							}
						}
						else if (selected_tile.civilian->type == Settler) {
							bool has_city = false;
							for (int y = selected_point.y - 4; y <= selected_point.y + 4; ++y) {
								if (y < 0 || y >= height) { continue; }
								for (int x = selected_point.x - 4; x <= selected_point.x + 4; ++x) {
									if (x < 0 || x >= width) { continue; }
									if (std::abs(selected_point.y - y) + std::abs(selected_point.x - x) > 4) { continue; }
									if (tiles[y][x].city) {
										has_city = true;
									}
								}
							}
							if (!has_city) {
								selected_tile.city = selected_tile.civilian->player->create_city(*this, selected_point.x, selected_point.y);
								selected_tile.civilian->kill();
								selected_tile.civilian = NULL;
							}
						}
					}
				}
				break;
			}
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			mouse_down_unmoved = true;
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			if (mouse_down_unmoved) {
				printf("Grid (%.0lf, %.0lf)\n", (event.button.x - xrel) / zoom, (event.button.y - yrel) / zoom);
				Point pressed_point(
					((event.button.x - xrel) / zoom - Renderer::border) / (Renderer::dim + Renderer::border),
					((event.button.y - yrel) / zoom - Renderer::border) / (Renderer::dim + Renderer::border));
				int px = ((pressed_point.x + 1) * (Renderer::dim + Renderer::border)) * zoom + xrel;
				int py = ((pressed_point.y + 1) * (Renderer::dim + Renderer::border)) * zoom + yrel;
				if (event.button.x < px && event.button.y < py) {
					printf("Button press (%d, %d) (%d, %d)\n", pressed_point.x, pressed_point.y, event.button.x, event.button.y);
					if (event.button.button == SDL_BUTTON_LEFT) {
						selected_point = pressed_point;
					}
					if (event.button.button == SDL_BUTTON_RIGHT) {
						if (selected_point.y != -1 && selected_point.x != -1) {
							handle_unit_attack_move(pressed_point);
						}
					}
				}
				else {
					selected_point = { -1, -1 };
				}
			}
		}
		if (event.type == SDL_MOUSEMOTION) {
			mouse_down_unmoved = false;
			if (event.motion.state & SDL_BUTTON_LMASK) {
				xrel += event.motion.xrel;
				yrel += event.motion.yrel;
			}
		}
		if (event.type == SDL_MOUSEWHEEL) {
			if (event.wheel.y > 0) {
				if (zoom < 1) {
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

void State::handle_unit_attack_move(Point pressed_point)
{
	Tile& pressed_tile = tile(pressed_point);
	Tile* selected_tile = &tile(selected_point);
	const auto mtiles = movement_tiles(selected_point);
	auto it = mtiles.find(pressed_point);
	if (it != mtiles.end()) {
		if (render_military() && selected_tile->military) {
			// TODO: implement "fast movement" where ranged attack range != movement
			if (pressed_tile.military) {
				if (selected_tile->military->player != pressed_tile.military->player && selected_tile->military->attacks > 0) {
					if (selected_tile->military->is_melee()) {
						MilitaryUnit*& move_dest = tile(it->second.second).military;
						selected_tile->military->movement = it->second.first;
						if (move_dest != selected_tile->military) {
							assert(!move_dest);
							move_dest = selected_tile->military;
							selected_tile->military = NULL;
							selected_point = it->second.second;
							selected_tile = &tile(selected_point);
						}
					}
					--selected_tile->military->attacks;
					printf("Before: %d, %d ", selected_tile->military->health, pressed_tile.military->health);
					int selected_damage = selected_tile->military->max_attack() * selected_tile->military->health / selected_tile->military->max_health();
					int pressed_damage = pressed_tile.military->max_attack() * pressed_tile.military->health / pressed_tile.military->max_health();
					pressed_tile.military->health -= selected_damage;
					if (selected_tile->military->is_melee()) {
						selected_tile->military->health -= pressed_damage;
						if (pressed_tile.military->health > 0) {
							if (selected_tile->military->health <= 0) {
								selected_tile->military->kill();
								selected_tile->military = NULL;
								printf("After: dead, %d\n", pressed_tile.military->health);
								selected_point = { -1, -1 };
								return;
							}
							else {
								printf("After: %d, %d\n", selected_tile->military->health, pressed_tile.military->health);
							}
						}
						else {
							selected_tile->military->movement -= pressed_tile.terrain_cost();
							pressed_tile.military->kill();
							pressed_tile.military = selected_tile->military;
							selected_tile->military = NULL;
							printf("After: %d, dead\n", pressed_tile.military->health);
							selected_point = pressed_point;
							return;
						}
					}
					else {
						if (pressed_tile.military->health <= 0) {
							pressed_tile.military->kill();
							pressed_tile.military = NULL;
							printf("After: %d, dead\n", selected_tile->military->health);
						}
						else {
							printf("After: %d, %d\n", selected_tile->military->health, pressed_tile.military->health);
						}
					}
				}
			}
			else {
				selected_tile->military->movement = it->second.first;
				pressed_tile.military = selected_tile->military;
				selected_tile->military = NULL;
				selected_point = pressed_point;
			}
		}
		else if (render_civilians() && selected_tile->civilian) {
			if (!pressed_tile.civilian) {
				selected_tile->civilian->movement = it->second.first;
				pressed_tile.civilian = selected_tile->civilian;
				selected_tile->civilian = 0;
				selected_point = pressed_point;
			}
		}
	}
}

std::map<Point, std::pair<int, Point>> State::movement_tiles(const Point& tile) const
{
	if (tile.y != -1 && tile.x != -1) {
		Unit* u = NULL;
		if (render_civilians()) { u = tiles[tile.y][tile.x].civilian; }
		if (render_military()) { u = tiles[tile.y][tile.x].military; }

		if (u) {
			return u->movement_tiles(tile, tiles);
		}
	}
	return {};
}

const Tile& State::tile(Point t) const
{
	return tiles[t.y][t.x];
}

Tile& State::tile(Point t)
{
	return tiles[t.y][t.x];
}

bool State::render_military() const
{
	return render_mode == RenderMilitary;
}

bool State::render_civilians() const
{
	return render_mode == RenderCivilians;
}
