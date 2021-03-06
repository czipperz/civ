#include "stdafx.h"
#include "State.h"
#include "Renderer.h"
#include <SDL.h>
#include <assert.h>
#include "Unit.h"
#include "Player.h"
#include <algorithm>
#include <math.h>

Point State::to_grid(Point mouse_point) {
	if (mouse_point.y < top_bar_height) {
		return { -1, -1 };
	}
	SDL_Rect minimap_dest;
	minimap_dest.w = window_width / 8;
	minimap_dest.h = minimap_dest.w * height / width;
	minimap_dest.x = 0;
	minimap_dest.y = window_height - minimap_dest.h;
	if (mouse_point.inside(minimap_dest)) {
		return { -1, -1 };
	}
	Point pressed_point(
		floor((mouse_point.x / zoom + xrel - Renderer::border) / (Renderer::dim + Renderer::border)),
		floor((mouse_point.y / zoom + yrel - Renderer::border) / (Renderer::dim + Renderer::border)));
	if (pressed_point.x < 0 || pressed_point.y < 0 || pressed_point.x >= width || pressed_point.y >= height) {
		return { -1, -1 };
	}
	int max_x = ((pressed_point.x + 1) * (Renderer::dim + Renderer::border) - xrel) * zoom;
	int max_y = ((pressed_point.y + 1) * (Renderer::dim + Renderer::border) - yrel) * zoom;
	if (mouse_point.x < max_x && mouse_point.y < max_y) {
		return pressed_point;
	}
	else {
		return { -1, -1 };
	}
}

Point State::selected_point() const
{
	if (render_cities_workers()) { return selected_point_city; }
	if (render_military()) { return selected_point_military; }
	if (render_civilians()) { return selected_point_civilian; }
}

void State::set_selected_point(Point p)
{
	if (render_cities_workers()) { selected_point_city = p; }
	if (render_military()) { selected_point_military = p; }
	if (render_civilians()) { selected_point_civilian = p; }
}

void State::unset_selected_point()
{
	set_selected_point({ -1, -1 });
}

State::State(int width, int height)
	: xrel(0)
	, yrel(-top_bar_height / .5)
	, zoom(.5)
	, mouse_down_unmoved(false)
	, width(width)
	, height(height)
	, selected_point_military({ -1, -1 })
	, selected_point_civilian({ -1, -1 })
	, selected_point_city({ -1, -1 })
	, render_mode(RenderCivilians)
	, render_resources(true)
	, turn(1)
{
	generate_tiles(tiles, width, height);
}

State::~State()
{
}

/// The goal of zooming is to have the point the cursor is at remain stationary.
/// Thus we find the point of the cursor, x / zoom, and offset it
/// by the point in the new frame, x / new_zoom.
void State::zoom_in() {
	if (zoom < 1) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		double new_zoom = zoom + .125;
		xrel += x / zoom - x / new_zoom;
		yrel += y / zoom - y / new_zoom;
		zoom = new_zoom;
	}
}

void State::zoom_out() {
	if (zoom > .375) {
		int x, y;
		SDL_GetMouseState(&x, &y);
		double new_zoom = zoom - .125;
		xrel += x / zoom - x / new_zoom;
		yrel += y / zoom - y / new_zoom;
		zoom = new_zoom;
	}
}

void State::end_turn() {
	printf("Ending turn %d\n", turn);
	for (auto& p : players) {
		p->end_turn();
	}
	++turn;
}

int State::advance_state()
{
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_KEYDOWN && !event.key.repeat) {
			switch (event.key.keysym.sym) {
			case 's':
				if (selected_point().y != -1 && !tile(selected_point()).military) {
					tile(selected_point()).military = players[0]->create_military_unit(RockSlinger);
				}
				break;
			case 'a':
				zoom_in();
				break;
			case 'o':
				zoom_out();
				break;
			case 'q':
				return 1;
			case 'm':
				render_mode = RenderMilitary;
				render_resources = false;
				break;
			case 'c':
				if (render_mode != RenderCivilians) {
					render_mode = RenderCivilians;
					render_resources = true;
				}
				else {
					render_resources = !render_resources;
				}
				break;
			case 't':
				render_mode = RenderCities;
				render_resources = true;
				break;
			case 'i':
				if (selected_point().y != -1) {
					Tile& selected_tile = tile(selected_point());
					if (render_civilians() && selected_tile.civilian && selected_tile.civilian->movement > 0) {
						selected_tile.civilian->movement = 0;
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
							for (int y = selected_point().y - 4; y <= selected_point().y + 4; ++y) {
								if (y < 0 || y >= height) { continue; }
								for (int x = selected_point().x - 4; x <= selected_point().x + 4; ++x) {
									if (x < 0 || x >= width) { continue; }
									if (std::abs(selected_point().y - y) + std::abs(selected_point().x - x) > 4) { continue; }
									if (tiles[y][x].city) {
										has_city = true;
									}
								}
							}
							if (!has_city) {
								selected_tile.city = selected_tile.civilian->player->create_city(*this, selected_point().x, selected_point().y);
								selected_tile.city->workers.emplace_back();
								selected_tile.civilian->kill();
								selected_tile.civilian = NULL;
								selected_tile.cover = NoCover;
								selected_point_city = selected_point();
							}
						}
					}
				}
				break;
			case SDLK_RETURN:
				end_turn();
				break;
			}
		}
		if (event.type == SDL_MOUSEBUTTONDOWN) {
			mouse_down_unmoved = true;
		}
		if (event.type == SDL_MOUSEBUTTONUP) {
			if (mouse_down_unmoved) {
				Point pressed_point = to_grid({ event.button.x, event.button.y });
				if (event.button.button == SDL_BUTTON_LEFT) {
					if (pressed_point.y != -1) {
						auto& t = tile(pressed_point);
						if (render_cities_workers() && !t.city) {
							pressed_point = { -1, -1 };
						}
						if (render_military() && !t.military) {
							pressed_point = { -1, -1 };
						}
						if (render_civilians() && !t.civilian) {
							pressed_point = { -1, -1 };
						}
					}
					if (pressed_point.y != -1) {
						printf("Select (%d, %d) (%d, %d)\n", pressed_point.x, pressed_point.y, event.button.x, event.button.y);
						set_selected_point(pressed_point);
					}
					else if (!render_cities_workers()) {
						printf("Deselect\n");
						unset_selected_point();
					}
				}
				if (event.button.button == SDL_BUTTON_RIGHT) {
					if (pressed_point.y != -1 && selected_point().y != -1) {
						if (render_cities_workers()) {
							City* city = tile(selected_point()).city;
							if (city) {
								printf("Toggle working (%d, %d)\n", pressed_point.x, pressed_point.y);
								if (city->owned_points[pressed_point.y][pressed_point.x]) {
									bool finished = false;
									for (size_t i = 0; i < city->workers.size(); ++i) {
										auto& worker = city->workers[i];
										if (worker.working && worker.working_point == pressed_point) {
											if (i != 0) {
												worker.working = false;
											}
											finished = true;
											break;
										}
									}
									if (!finished) {
										for (auto& worker : city->workers) {
											if (!worker.working) {
												worker.working = true;
												worker.working_point = pressed_point;
												break;
											}
										}
									}
								}
							}
							else {
								printf("No city selected\n");
							}
						}
						else {
							printf("Move or attack (%d, %d) (%d, %d)\n", pressed_point.x, pressed_point.y, event.button.x, event.button.y);
							handle_unit_attack_move(pressed_point);
						}
					}
				}
			}
		}
		if (event.type == SDL_MOUSEMOTION) {
			mouse_down_unmoved = false;
			if (event.motion.state & SDL_BUTTON_LMASK) {
				xrel -= event.motion.xrel / zoom;
				yrel -= event.motion.yrel / zoom;
			}
		}
		if (event.type == SDL_MOUSEWHEEL) {
			if (event.wheel.y > 0) {
				zoom_in();
			}
			if (event.wheel.y < 0) {
				zoom_out();
			}
		}
	}
	return 0;
}

void State::handle_unit_attack_move(Point pressed_point)
{
	Tile& pressed_tile = tile(pressed_point);
	Tile* selected_tile = &tile(selected_point());
	if (render_military() && selected_tile->military) {
		// TODO: implement "fast movement" where ranged attack range != movement
		if (pressed_tile.military) {
			const auto atiles = attack_tiles(selected_point());
			auto it = atiles.find(pressed_point);
			if (it != atiles.end()) {
				if (selected_tile->military->player != pressed_tile.military->player && selected_tile->military->attacks > 0) {
					if (selected_tile->military->is_melee()) {
						MilitaryUnit*& move_dest = tile(it->second.second).military;
						selected_tile->military->movement = it->second.first;
						if (move_dest != selected_tile->military) {
							assert(!move_dest);
							move_dest = selected_tile->military;
							selected_tile->military = NULL;
							set_selected_point(it->second.second);
							selected_tile = &tile(selected_point());
						}
					}
					--selected_tile->military->attacks;
					if (selected_tile->military->attacks == 0) {
						selected_tile->military->movement = 0;
					}
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
								unset_selected_point();
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
							set_selected_point(pressed_point);
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
		}
		else {
			const auto mtiles = movement_tiles(selected_point());
			auto it = mtiles.find(pressed_point);
			if (it != mtiles.end()) {
				selected_tile->military->movement = it->second.first;
				pressed_tile.military = selected_tile->military;
				selected_tile->military = NULL;
				set_selected_point(pressed_point);
			}
		}
	}
	else if (render_civilians() && selected_tile->civilian) {
		if (!pressed_tile.civilian) {
			const auto mtiles = movement_tiles(selected_point());
			auto it = mtiles.find(pressed_point);
			if (it != mtiles.end()) {
				selected_tile->civilian->movement = it->second.first;
				pressed_tile.civilian = selected_tile->civilian;
				selected_tile->civilian = 0;
				set_selected_point(pressed_point);
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

std::map<Point, std::pair<int, Point>> State::attack_tiles(const Point& tile) const
{
	if (tile.y != -1 && tile.x != -1) {
		if (render_military()) {
			MilitaryUnit* u = tiles[tile.y][tile.x].military;
			if (u) {
				return u->attack_tiles(tile, tiles);
			}
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

bool State::render_cities_workers() const
{
	return render_mode == RenderCities;
}
