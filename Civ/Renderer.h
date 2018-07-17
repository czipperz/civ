#pragma once
#include <SDL.h>
#include <SDL_ttf.h>
#include <vector>
#include "Tile.h"

class State;

extern int window_width;
extern int window_height;
extern int top_bar_height;

class Renderer
{
	void render_frame(const State&);
	void present_screen(const State&);
public:
	SDL_Window* window;
	SDL_Renderer* renderer;
	SDL_Texture* minimap;
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
	SDL_Texture* civilian_background_selected_no_military;
	SDL_Texture* city_overlay;
	SDL_Texture* rock_slinger;
	SDL_Texture* clubber;
	SDL_Texture* worker;
	SDL_Texture* settler;
	SDL_Texture* food;
	SDL_Texture* production;
	SDL_Texture* frame;
	TTF_Font* font;
	int frame_width;
	int frame_height;
	static const int dim = 128;
	static const int border = 32;

	Renderer(int width, int height);
	~Renderer();
	Renderer(const Renderer&) = delete;
	Renderer& operator=(const Renderer&) = delete;

	void render(const State & state);
};

