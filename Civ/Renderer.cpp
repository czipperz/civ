#include "stdafx.h"
#include "Renderer.h"
#include <stdlib.h>
#include <stdio.h>
#include <SDL_image.h>
#include <SDL_ttf.h>
#include "SDL_FontCache.h"
#include "Unit.h"
#include "State.h"
#include <assert.h>
#include "Resources.h"
#include "Player.h"

int window_width = 640;
int window_height = 480;
int top_bar_height = 26;

static void save_texture(const char* file_name, SDL_Renderer* renderer, SDL_Texture* texture) {
	SDL_Texture* target = SDL_GetRenderTarget(renderer);
	SDL_SetRenderTarget(renderer, texture);
	int width, height;
	SDL_QueryTexture(texture, NULL, NULL, &width, &height);
	SDL_Surface* surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
	SDL_RenderReadPixels(renderer, NULL, surface->format->format, surface->pixels, surface->pitch);
	IMG_SavePNG(surface, file_name);
	SDL_FreeSurface(surface);
	SDL_SetRenderTarget(renderer, target);
}

Renderer::Renderer(int width, int height)
	: frame_width((dim + border) * width + border)
	, frame_height((dim + border) * height + border)
{
	window = SDL_CreateWindow("Civ",
		0 /*SDL_WINDOWPOS_UNDEFINED*/, SDL_WINDOWPOS_UNDEFINED,
		window_width, window_height, SDL_WINDOW_SHOWN);
	//SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN_DESKTOP);
	//SDL_GetWindowSize(window, &window_width, &window_height);
	if (!window) {
		fprintf(stderr, "SDL window creation error: %s\n", SDL_GetError());
		exit(1);
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_TARGETTEXTURE);
	if (!renderer) {
		fprintf(stderr, "SDL renderer creation error: %s\n", SDL_GetError());
		exit(1);
	}
	//SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	minimap = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		window_width / 8, frame_height * window_width / 8 / frame_width);

	plains = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim, dim);
	if (!plains) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetTextureBlendMode(plains, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, plains);
	SDL_SetRenderDrawColor(renderer, 0xE5, 0xD3, 0x9C, 0xFF);
	SDL_RenderClear(renderer);

	desert = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim, dim);
	if (!desert) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetTextureBlendMode(desert, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, desert);
	SDL_SetRenderDrawColor(renderer, 0xF5, 0xFF, 0x70, 0xFF);
	SDL_RenderClear(renderer);

	forest = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim, dim);
	if (!forest) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetTextureBlendMode(forest, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, forest);
	SDL_SetRenderDrawColor(renderer, 0x3B, 0x93, 0x35, 0xFF);
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			if ((x - y) % 2 == 0) {
				SDL_Rect rect;
				rect.x = (x + 1) * dim / 5 - dim / 10 + 2;
				rect.y = (y + 1) * dim / 5 - dim / 10 + 2;
				rect.w = dim / 5 - 4;
				rect.h = dim / 5 - 4;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}

	jungle = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim, dim);
	if (!jungle) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetTextureBlendMode(jungle, SDL_BLENDMODE_BLEND);
	SDL_SetRenderTarget(renderer, jungle);
	SDL_SetRenderDrawColor(renderer, 0x25, 0x4E, 0x2D, 0xFF);
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			if ((x - y) % 2 == 0) {
				SDL_Rect rect;
				rect.x = (x + 1) * dim / 5 - dim / 10 + 2;
				rect.y = (y + 1) * dim / 5 - dim / 10 + 2;
				rect.w = dim / 5 - 4;
				rect.h = dim / 5 - 4;
				SDL_RenderFillRect(renderer, &rect);
			}
		}
	}

	hill = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/Hill.png");

	movement_outline = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim + 2 * border, dim + 2 * border);
	if (!movement_outline) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetRenderTarget(renderer, movement_outline);
	SDL_SetRenderDrawColor(renderer, 0x60, 0xE2, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	military_move_attack = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim, dim);
	SDL_SetTextureBlendMode(military_move_attack, SDL_BLENDMODE_BLEND);
	if (!military_move_attack) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetRenderTarget(renderer, military_move_attack);
	SDL_SetRenderDrawColor(renderer, 0xAA, 0, 0, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	{
		SDL_Rect middle = { border / 4, border / 4, dim - border / 2, dim - border / 2 };
		SDL_RenderFillRect(renderer, &middle);
	}

	military_move_only = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET,
		dim, dim);
	SDL_SetTextureBlendMode(military_move_only, SDL_BLENDMODE_BLEND);
	if (!military_move_only) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}
	SDL_SetRenderTarget(renderer, military_move_only);
	SDL_SetRenderDrawColor(renderer, 0x0B, 0xB2, 0xE2, 0xFF);
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
	{
		SDL_Rect middle = { border / 4, border / 4, dim - border / 2, dim - border / 2 };
		SDL_RenderFillRect(renderer, &middle);
	}

	military_background = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/MilitaryOutline.png");
	military_background_selected = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/MilitaryOutlineSelected.png");
	military_background_selected_no_attack = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/MilitaryOutlineSelectedNoAttack.png");
	civilian_background_selected_no_military = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/CivilianOutlineSelectedNoMilitary.png");
	city_overlay = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/CityOverlay.png");
	rock_slinger = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/RockSlinger.png");
	clubber = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/Clubber.png");
	worker = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/Worker.png");
	settler = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/Settler.png");
	food = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/Food.png");
	production = IMG_LoadTexture(renderer, "C:/Users/gregorcj/Pictures/Production.png");

	frame = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, frame_width, frame_height);
	if (!frame) {
		fprintf(stderr, "SDL texture creation error: %s\n", SDL_GetError());
		exit(1);
	}

	font = FC_CreateFont();
	FC_LoadFont(font, renderer, "C:/Windows/Fonts/Century.ttf", 20, SDL_Color {0, 0, 0, 0xFF}, TTF_STYLE_NORMAL);
	if (!font) {
		fprintf(stderr, "Font loading error: %s\n", TTF_GetError());
		exit(1);
	}
}

Renderer::~Renderer()
{
	FC_FreeFont(font);
	SDL_DestroyTexture(frame);
	SDL_DestroyTexture(production);
	SDL_DestroyTexture(food);
	SDL_DestroyTexture(clubber);
	SDL_DestroyTexture(settler);
	SDL_DestroyTexture(worker);
	SDL_DestroyTexture(rock_slinger);
	SDL_DestroyTexture(city_overlay);
	SDL_DestroyTexture(civilian_background_selected_no_military);
	SDL_DestroyTexture(military_background_selected_no_attack);
	SDL_DestroyTexture(military_background_selected);
	SDL_DestroyTexture(military_background);
	SDL_DestroyTexture(military_move_only);
	SDL_DestroyTexture(military_move_attack);
	SDL_DestroyTexture(movement_outline);
	SDL_DestroyTexture(hill);
	SDL_DestroyTexture(jungle);
	SDL_DestroyTexture(forest);
	SDL_DestroyTexture(desert);
	SDL_DestroyTexture(plains);
	SDL_DestroyTexture(minimap);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
}

void Renderer::render(const State& state)
{
	render_frame(state);
	present_screen(state);
}

void Renderer::render_frame(const State& state) {
	SDL_SetRenderTarget(renderer, frame);
	SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xFF);
	SDL_RenderClear(renderer);

	for (int y = 0; y < state.height; ++y) {
		for (int x = 0; x < state.width; ++x) {
			SDL_Rect dest;
			dest.x = (dim + border) * x + border / 2;
			dest.y = (dim + border) * y + border / 2;
			dest.w = dim + border;
			dest.h = dim + border;
			Player* player = state.tiles[y][x].player;
			if (player) {
				SDL_Color color;
				switch (player->civilization) {
				case America:
					color.r = 0x00;
					color.g = 0x00;
					color.b = 0x99;
					break;
				case Britain:
					color.r = 0x77;
					color.g = 0;
					color.b = 0;
					break;
				}
				SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xFF);
				SDL_RenderFillRect(renderer, &dest);
			}
		}
	}

	for (int y = 0; y < state.height; ++y) {
		for (int x = 0; x < state.width; ++x) {
			SDL_Rect dest;
			dest.x = (dim + border) * x + border * 3 / 4;
			dest.y = (dim + border) * y + border * 3 / 4;
			dest.w = dim + border / 2;
			dest.h = dim + border / 2;
			const Tile& tile = state.tiles[y][x];
			Player* player = NULL;
			if (state.render_military() && tile.military) {
				player = tile.military->player;
			}
			else if (state.render_civilians() && tile.civilian) {
				player = tile.civilian->player;
			}
			if (player) {
				SDL_Color color;
				switch (player->civilization) {
				case America:
					color.r = 0x44;
					color.g = 0x77;
					color.b = 0xFF;
					break;
				case Britain:
					color.r = 0xDD;
					color.g = 0;
					color.b = 0;
					break;
				}
				SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 0xFF);
				SDL_RenderFillRect(renderer, &dest);
			}
		}
	}

	for (int y = 0; y < state.height; ++y) {
		for (int x = 0; x < state.width; ++x) {
			SDL_Rect dest;
			dest.x = (dim + border) * x + border;
			dest.y = (dim + border) * y + border;
			dest.w = dim;
			dest.h = dim;
			const Tile& tile = state.tiles[y][x];
			if (tile.type == Plains) {
				SDL_RenderCopy(renderer, plains, NULL, &dest);
			}
			if (tile.type == Desert) {
				SDL_RenderCopy(renderer, desert, NULL, &dest);
			}
			if (tile.cover == Forest) {
				SDL_RenderCopy(renderer, forest, NULL, &dest);
			}
			if (tile.cover == Jungle) {
				SDL_RenderCopy(renderer, jungle, NULL, &dest);
			}
			if (tile.height == Hill) {
				SDL_RenderCopy(renderer, hill, NULL, &dest);
			}
		}
	}

	if (state.render_military() && state.selected_point.y != -1 && state.tile(state.selected_point).military) {
		if (state.tile(state.selected_point).military->attacks > 0) {
			const auto tiles = state.attack_tiles(state.selected_point);
			for (const auto& tile : tiles) {
				SDL_Rect dest;
				dest.x = (dim + border) * tile.first.x + border;
				dest.y = (dim + border) * tile.first.y + border;
				dest.w = dim;
				dest.h = dim;
				SDL_RenderCopy(renderer, military_move_attack, NULL, &dest);
			}
		}
		const auto tiles = state.movement_tiles(state.selected_point);
		for (const auto& tile : tiles) {
			SDL_Rect dest;
			dest.x = (dim + border) * tile.first.x + border;
			dest.y = (dim + border) * tile.first.y + border;
			dest.w = dim;
			dest.h = dim;
			if (state.tile(tile.first).military) {
				const Tile& selected_tile = state.tile(state.selected_point);
				if (selected_tile.military->player != state.tile(tile.first).military->player && selected_tile.military->attacks > 0) {
					SDL_RenderCopy(renderer, military_move_attack, NULL, &dest);
					if (selected_tile.military->is_melee()) {
						SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
						SDL_SetRenderDrawColor(renderer, 0xFF, 0, 0, 0xFF);
						SDL_Rect dest;
						dest.x = (dim + border) * tile.first.x + border;
						dest.y = (dim + border) * tile.first.y + border;
						if (tile.second.second.x != tile.first.x) {
							dest.y += border / 2;
							dest.w = border / 2;
							dest.h = dim - border;
						}
						if (tile.second.second.x > tile.first.x) {
							dest.x += dim - border / 2;
						}
						if (tile.second.second.y != tile.first.y) {
							dest.x += border / 2;
							dest.w = dim - border;
							dest.h = border / 2;
						}
						if (tile.second.second.y > tile.first.y) {
							dest.y += dim - border / 2;
						}
						SDL_RenderFillRect(renderer, &dest);
					}
				}
			}
			else {
				SDL_RenderCopy(renderer, military_move_only, NULL, &dest);
			}
		}
	}
	else if (state.render_civilians() && state.selected_point.y != -1 && state.tile(state.selected_point).civilian) {
		const auto tiles = state.movement_tiles(state.selected_point);
		for (const auto& tile : tiles) {
			SDL_Rect dest;
			dest.x = (dim + border) * tile.first.x + border;
			dest.y = (dim + border) * tile.first.y + border;
			dest.w = dim;
			dest.h = dim;
			if (!state.tile(tile.first).civilian) {
				SDL_RenderCopy(renderer, military_move_only, NULL, &dest);
			}
		}
	}

	if (state.render_cities_workers() && state.selected_point.y != -1 && state.tile(state.selected_point).city) {
		City* city = state.tile(state.selected_point).city;
		for (int y = 0; y < state.height; ++y) {
			for (int x = 0; x < state.width; ++x) {
				if (city->owned_points[y][x]) {
					SDL_Rect dest;
					dest.x = (dim + border) * x + border;
					dest.y = (dim + border) * y + border;
					dest.w = dim;
					dest.h = dim;
					SDL_RenderCopy(renderer, military_move_only, NULL, &dest);
				}
			}
		}
		for (auto& worker : city->workers) {
			if (worker.working) {
				SDL_Rect dest;
				dest.x = (dim + border) * worker.working_point.x + border;
				dest.y = (dim + border) * worker.working_point.y + border;
				dest.w = dim;
				dest.h = dim;
				SDL_RenderCopy(renderer, military_move_attack, NULL, &dest);
			}
		}
	}

	for (int y = 0; y < state.height; ++y) {
		for (int x = 0; x < state.width; ++x) {
			SDL_Rect dest;
			dest.x = (dim + border) * x + border;
			dest.y = (dim + border) * y + border;
			dest.w = dim;
			dest.h = dim;
			const Tile& tile = state.tiles[y][x];
			if (tile.city) {
				SDL_RenderCopy(renderer, city_overlay, NULL, &dest);
			}
			if (state.render_civilians() && tile.civilian) {
				if (state.selected_point.x == x && state.selected_point.y == y) {
					if (tile.military) {
						SDL_RenderCopy(renderer, military_background_selected_no_attack, NULL, &dest);
					}
					else {
						SDL_RenderCopy(renderer, civilian_background_selected_no_military, NULL, &dest);
					}
				}
				if (tile.military) {
					SDL_RenderCopy(renderer, military_background, NULL, &dest);
				}
				switch (tile.civilian->type) {
				case Worker:
					SDL_RenderCopy(renderer, worker, NULL, &dest);
					break;
				case Settler:
					SDL_RenderCopy(renderer, settler, NULL, &dest);
					break;
				}
			}
			if (state.render_military() && tile.military) {
				if (state.selected_point.x == x && state.selected_point.y == y) {
					if (tile.military->attacks > 0) {
						SDL_RenderCopy(renderer, military_background_selected, NULL, &dest);
					}
					else {
						SDL_RenderCopy(renderer, military_background_selected_no_attack, NULL, &dest);
					}
				}
				SDL_RenderCopy(renderer, military_background, NULL, &dest);
				switch (tile.military->type) {
				case RockSlinger:
					SDL_RenderCopy(renderer, rock_slinger, NULL, &dest);
					break;
				case Clubber:
					SDL_RenderCopy(renderer, clubber, NULL, &dest);
					break;
				}
			}

			if (state.render_resources) {
				Resources r = tile.resources();
				std::vector<SDL_Texture*> textures;
				for (int f = 0; f < r.food; ++f) { textures.push_back(food); }
				for (int p = 0; p < r.production; ++p) { textures.push_back(production); }
				int i = 0;
				if (!textures.empty()) {
					SDL_Rect res_dest;
					res_dest.y = dest.y + dim / 2 - ((textures.size() - 1) / 4 + 1) * 24 / 2;
					res_dest.w = 24;
					res_dest.h = 24;
					for (int row = 1; row <= (textures.size() - 1) / 4; ++row) {
						res_dest.x = dest.x - 24 / 2;
						res_dest.x += dim / (4 + 1);
						for (; i < row * 4; ++i) {
							SDL_RenderCopy(renderer, textures[i], NULL, &res_dest);
							res_dest.x += dim / (4 + 1);
						}
						res_dest.y += 24;
					}
					// final row
					{
						int remaining = textures.size() % 4;
						if (remaining == 0 && !textures.empty()) { remaining = 4; }
						res_dest.x = dest.x - 24 / 2;
						res_dest.x += dim / (remaining + 1);
						for (; i < textures.size(); ++i) {
							SDL_RenderCopy(renderer, textures[i], NULL, &res_dest);
							res_dest.x += dim / (remaining + 1);
						}
					}
				}
			}
		}
	}
}

void Renderer::present_screen(const State& state) {
	SDL_SetRenderTarget(renderer, NULL);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
	SDL_RenderClear(renderer);
	{
		SDL_Rect src;
		src.x = state.xrel;
		src.y = state.yrel;
		src.w = window_width / state.zoom;
		src.h = window_height / state.zoom;
		// handle when the user has gone partially off the frame.
		SDL_Rect dest;
		if (state.xrel < 0) {
			dest.x = -state.xrel * state.zoom;
			dest.w = window_width - dest.x;
		}
		else if (state.xrel >= frame_width - window_width / state.zoom) {
			dest.x = 0;
			dest.w = window_width - (state.xrel - (frame_width - window_width / state.zoom)) * state.zoom;
		}
		else {
			dest.x = 0;
			dest.w = window_width;
		}
		if (state.yrel < 0) {
			dest.y = -state.yrel * state.zoom;
			dest.h = window_height - dest.y;
		}
		else if (state.yrel >= frame_height - window_height / state.zoom) {
			dest.y = 0;
			dest.h = window_height - (state.yrel - (frame_height - window_height / state.zoom)) * state.zoom;
		}
		else {
			dest.y = 0;
			dest.h = window_height;
		}
		SDL_RenderCopy(renderer, frame, &src, &dest);
	}
	{
		SDL_Rect top_bar = { 0, 0, window_width, top_bar_height };
		SDL_SetRenderDrawColor(renderer, 0xF2, 0xC6, 0xAB, 0xFF);
		SDL_RenderFillRect(renderer, &top_bar);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderDrawLine(renderer, 0, top_bar_height, window_width, top_bar_height);

		{
			char buffer[1024];
			snprintf(buffer, 1024, "Turn %d", state.turn);
			int width = FC_GetWidth(font, "%s", buffer);
			FC_Draw(font, renderer, window_width / 2 - width / 2, 0, "%s", buffer);
		}
		{
			const char* buffer;
			if (state.render_civilians()) { buffer = "Civilians"; }
			if (state.render_military()) { buffer = "Military"; }
			if (state.render_cities_workers()) { buffer = "City"; }
			FC_Draw(font, renderer, 3, 0, "%s", buffer);
		}
		if (state.render_cities_workers() && state.selected_point.y != -1) {
			City* selected_city = state.tile(state.selected_point).city;
			if (selected_city) {
				Resources r = selected_city->resources_per_turn(state);
				char buffer[1024];
				SDL_Rect dest;
				dest.x = window_width;
				SDL_QueryTexture(food, NULL, NULL, &dest.w, &dest.h);
				dest.w = dest.w * 2 / 4;
				dest.h = dest.h * 2 / 4;
				dest.y = top_bar_height / 2 - dest.h / 2;

				dest.x -= 4;
				snprintf(buffer, 1024, "%s%d", r.production >= 0 ? "+" : "-", abs(r.production));
				dest.x -= FC_GetWidth(font, "%s", buffer);
				FC_Draw(font, renderer, dest.x, 0, "%s", buffer);
				dest.x -= dest.w + 2;
				SDL_RenderCopy(renderer, production, NULL, &dest);

				dest.x -= 4;
				snprintf(buffer, 1024, "%s%d", r.food >= 0 ? "+" : "-", abs(r.food));
				dest.x -= FC_GetWidth(font, "%s", buffer);
				FC_Draw(font, renderer, dest.x, 0, "%s", buffer);
				dest.x -= dest.w + 2;
				SDL_RenderCopy(renderer, food, NULL, &dest);
			}
		}
	}
	{
		SDL_SetRenderTarget(renderer, minimap);
		SDL_SetRenderDrawColor(renderer, 0xAA, 0xAA, 0xAA, 0xFF);
		SDL_RenderClear(renderer);

		SDL_Rect minimap_dest;
		minimap_dest.w = window_width / 8;
		minimap_dest.h = minimap_dest.w * state.height / state.width;
		minimap_dest.x = 0;
		minimap_dest.y = window_height - minimap_dest.h;
		SDL_Rect rect;
		rect.x = state.xrel * minimap_dest.w / frame_width;
		rect.y = state.yrel * minimap_dest.h / frame_height;
		rect.w = window_width / state.zoom / frame_width * minimap_dest.w;
		rect.h = window_height / state.zoom / frame_height * minimap_dest.h;
		SDL_SetRenderDrawColor(renderer, 0xCC, 0xCC, 0xCC, 0xFF);
		SDL_RenderFillRect(renderer, &rect);
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0xFF);
		SDL_RenderDrawRect(renderer, &rect);

		SDL_SetRenderTarget(renderer, NULL);
		SDL_RenderCopy(renderer, minimap, NULL, &minimap_dest);

		SDL_RenderDrawLine(renderer, minimap_dest.x, minimap_dest.y - 1, minimap_dest.x + minimap_dest.w, minimap_dest.y - 1);
		SDL_RenderDrawLine(renderer, minimap_dest.x + minimap_dest.w, minimap_dest.y - 1, minimap_dest.x + minimap_dest.w, minimap_dest.y + minimap_dest.h);
	}
	SDL_RenderPresent(renderer);
}
