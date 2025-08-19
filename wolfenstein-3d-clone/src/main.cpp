#include <iostream>
#include <SDL3/SDL.h>

#define TILE_SIZE 64

#define TILES_COL_NUM 20
#define TILE_ROW_NUM 13

#define WINDOW_WIDTH (TILES_COL_NUM * TILE_SIZE)
#define WINDOW_HEIGHT (TILE_ROW_NUM * TILE_SIZE)

#define MAP_SCALING_FACTOR 0.3f

#define MAP_OFFSET_X 20
#define MAP_OFFSET_Y 20


//////////////////// Map //////////////////////////////

const int map[TILE_ROW_NUM][TILES_COL_NUM] = 
{
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};

///////////////////////////////////////////////////////


//////////////////// Color /////////////////////////////
struct COLOR
{
	uint8_t r, g, b, a = 255;
};

COLOR WHITE_COLOR = { 255, 255, 255, 255 };
COLOR BLACK_COLOR = { 0, 0, 0, 255 };
COLOR RED_COLOR = { 255, 0, 0, 255 };
COLOR GREEN_COLOR = { 0, 255, 0, 255 };
COLOR BLUE_COLOR = { 0, 0, 255, 255 };
COLOR YELLOW_COLOR = { 255, 255, 0, 255 };
COLOR CYAN_COLOR = { 0, 255, 255, 255 };
COLOR MAGENTA_COLOR = { 255, 0, 255, 255 };
COLOR MAP_LINES_COLOR = { 87, 87, 87, 120 };

//////////////////////////////////////////////////////


//////////////////// Renderer ////////////////////////


void DrawOutlinedRect(SDL_Renderer* renderer,
	float x, float y,
	float w, float h,
	COLOR color, COLOR outline_color)
{
	SDL_FRect rect = { x, y, w, h };

	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
	SDL_RenderFillRect(renderer, &rect);

	SDL_SetRenderDrawColor(renderer, outline_color.r, outline_color.g, outline_color.b, outline_color.a);
	SDL_RenderRect(renderer, &rect);
}

//////////////////////////////////////////////////////



int main(int argc, char** argv)
{
	// init sdl3
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		std::cout << "Failed To Init SDL3!\n";
		__debugbreak();
	}

	// create sdl window
	auto window_falgs =
		SDL_WINDOW_RESIZABLE |
		SDL_WINDOW_INPUT_FOCUS;

	SDL_Window* window = SDL_CreateWindow("wolf 3d", WINDOW_WIDTH, WINDOW_HEIGHT, window_falgs);

	if (!window)
	{
		std::cout << "Failed To Create SDL3 Widnow!\n";
		__debugbreak();
	}

	// create renderer
	SDL_Renderer* renderer = SDL_CreateRenderer(window, NULL);

	if (!renderer)
	{
		std::cout << "Failed To Create SDL3 renderer!\n";
		__debugbreak();
	}

	// alpha blending
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	// game loop

	SDL_Event event;
	bool is_window_running = true;
	while (is_window_running)
	{
		// poll events
		while (SDL_PollEvent(&event))
		{
			switch (event.type)
			{
			case SDL_EVENT_MOUSE_MOTION:
			{
			}
			break;
			case SDL_EVENT_MOUSE_WHEEL:
			{
			}
			case SDL_EVENT_KEY_DOWN:
			{
			}
			break;
			case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
			{
				is_window_running = false;
			}
			break;
			default:
				break;
			}
		}

		// render
		SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);  // black
		SDL_RenderClear(renderer);
		
		
		// draw map
		for (size_t i = 0; i < TILE_ROW_NUM; i++)
		{
			for (size_t j = 0; j < TILES_COL_NUM; j++)
			{
				auto tile_color = map[i][j] == 1 ? WHITE_COLOR : BLACK_COLOR;

				DrawOutlinedRect(renderer,
					j * TILE_SIZE * MAP_SCALING_FACTOR + MAP_OFFSET_X,
					i * TILE_SIZE * MAP_SCALING_FACTOR + MAP_OFFSET_Y,
					TILE_SIZE * MAP_SCALING_FACTOR, TILE_SIZE * MAP_SCALING_FACTOR,
					tile_color, MAP_LINES_COLOR);
			}
		}

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
