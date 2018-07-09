#pragma once
#include <SDL.h>
#include <vector>
#include "Tile.h"

class State;

extern int window_width;
extern int window_height;

class Renderer
{
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* plains;
	SDL_Texture* desert;
	SDL_Texture* forest;
	SDL_Texture* jungle;
	SDL_Texture* hill;
	SDL_Texture* movement_outline;
	SDL_Texture* military_move_attack;
	SDL_Texture* military_move_only;
	SDL_Texture* military_background;
	SDL_Texture* military_background_selected;
	SDL_Texture* military_background_selected_no_attack;
	SDL_Texture* rock_slinger;
	SDL_Texture* clubber;
	SDL_Texture* food;
	SDL_Texture* production;
	SDL_Texture* frame;
	int frame_width;
	int frame_height;
	static const int dim = 128;
	static const int border = 16;

	Renderer(int width, int height);
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void render(const State & state);
};

