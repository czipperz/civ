// Civ.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <SDL.h>
#include <SDL_image.h>
#include <stdlib.h>
#include <time.h>
#include <vector>
#include "Tile.h"
#include "Renderer.h"
#include "Unit.h"
#include "State.h"
#include <algorithm>
#include "Player.h"

#ifdef __cplusplus
extern "C" {
#endif
	int main(int argc, char** argv)
	{
		srand(time(0));

		if (SDL_Init(SDL_INIT_VIDEO) < 0) {
			fprintf(stderr, "SDL not initialized: %s\n", SDL_GetError());
			return 1;
		}

		if ((IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG) != IMG_INIT_PNG) {
			fprintf(stderr, "IMG not initialized: %s\n", IMG_GetError());
			return 1;
		}

		window_width = 1080;
		window_height = 720;

		State state(30, 30);
		Renderer renderer(state.width, state.height);
		state.players.push_back(std::make_unique<Player>(America));
		state.players.push_back(std::make_unique<Player>(Britain));

		for (int p = 0; p < 2; ++p) {
			for (int i = 0; i < 5; ++i) {
				state.tiles[rand() % state.height][rand() % state.width].military = create_unit(state.players[p].get(), RockSlinger);
				state.tiles[rand() % state.height][rand() % state.width].military = create_unit(state.players[p].get(), Clubber);
				state.tiles[rand() % state.height][rand() % state.width].civilian = create_unit(state.players[p].get(), Worker);
				state.tiles[rand() % state.height][rand() % state.width].civilian = create_unit(state.players[p].get(), Settler);
			}
		}

		printf("Delay: %dms\n", (int)SDL_GetTicks());

		Uint32 beginning_second = SDL_GetTicks();
		int frames = 0;
		while (1) {
			++frames;
			Uint32 start = SDL_GetTicks();
			if (state.advance_state()) { break; }
			state.xrel = std::min(std::max(state.xrel, -renderer.frame_width * state.zoom + window_width), 0.);
			state.yrel = std::min(std::max(state.yrel, -renderer.frame_height * state.zoom + window_height), 0.);
			renderer.render(state);
			Uint32 end = SDL_GetTicks();
			if (end - beginning_second >= 1000) {
				printf("fps: %d\n", frames);
				frames = 0;
				beginning_second = end;
			}
			/*
			if (end - start < 1000 / 30) {
				SDL_Delay(1000 / 30 - (end - start));
			}
			*/
		}
		IMG_Quit();
		SDL_Quit();
		return 0;
	}
#ifdef __cplusplus
}
#endif
