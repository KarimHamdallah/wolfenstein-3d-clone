#include <iostream>
#include <SDL3/SDL.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl3.h>
#include <backends/imgui_impl_sdlrenderer3.h>

#define TILE_SIZE 64

#define TILES_COL_NUM 20
#define TILE_ROW_NUM 13

#define WINDOW_WIDTH (TILES_COL_NUM * TILE_SIZE)
#define WINDOW_HEIGHT (TILE_ROW_NUM * TILE_SIZE)

#define MAP_SCALING_FACTOR 1.0f

#define MAP_OFFSET_X 0
#define MAP_OFFSET_Y 0

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


//////////////////// Player /////////////////////////////

#define PI 3.14159265359
#define TORAD 0.01745329251

struct Player
{
	float x = WINDOW_WIDTH * 0.5f;
	float y = WINDOW_HEIGHT * 0.5f;
	float size = 10.0f;
	float rotation_angle = PI / 2.0f;
	float walk_direction = 0; // 1 or -1 walk forward, backward
	float turn_direction = 0; // 1 or -1 turn right, left
	float wlak_speed = 200.0f;
	float turn_speed = 90.0f * TORAD;

	void Update(float dt)
	{
		rotation_angle += turn_speed * turn_direction * dt;

		float new_x = x + cosf(rotation_angle) * wlak_speed * walk_direction * dt;
		float new_y = y + sinf(rotation_angle) * wlak_speed * walk_direction * dt;

		// collision detection
		int player_pos_at_map_col = floor((new_x + 0.5f * size) / TILE_SIZE);
		int player_pos_at_map_raw = floor((new_y + 0.5f * size) / TILE_SIZE);

		if (map[player_pos_at_map_raw][player_pos_at_map_col] != 1)
		{
			x = new_x;
			y = new_y;
		}
	}

	void Render(SDL_Renderer* renderer)
	{
		DrawOutlinedRect(renderer,
			x * MAP_SCALING_FACTOR,
			y * MAP_SCALING_FACTOR,
			size * MAP_SCALING_FACTOR, size * MAP_SCALING_FACTOR,
			RED_COLOR, WHITE_COLOR);


		SDL_SetRenderDrawColor(renderer, 255, 0, 0, 120);

		SDL_RenderLine(renderer,
			MAP_SCALING_FACTOR * (x + 0.5f * size),
			MAP_SCALING_FACTOR * (y + 0.5f * size),
			MAP_SCALING_FACTOR * (x + cosf(rotation_angle) * 100),
			MAP_SCALING_FACTOR * (y + sinf(rotation_angle) * 100));
	}
};

Player player;


/////////////////////////////////////////////////////////

uint64_t lastTime = SDL_GetTicks();
float deltaTime = 0.0f;


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
		uint64_t currentTime = SDL_GetTicks();
		deltaTime = (currentTime - lastTime) / 1000.0f;
		lastTime = currentTime;



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
				if (event.key.key == SDLK_W)
					player.walk_direction = +1;
				if (event.key.key == SDLK_S)
					player.walk_direction = -1;
				if (event.key.key == SDLK_D)
					player.turn_direction = +1;
				if (event.key.key == SDLK_A)
					player.turn_direction = -1;
			}
			break;
			case SDL_EVENT_KEY_UP:
			{
				if (event.key.key == SDLK_W)
					player.walk_direction = 0;
				if (event.key.key == SDLK_S)
					player.walk_direction = 0;
				if (event.key.key == SDLK_D)
					player.turn_direction = 0;
				if (event.key.key == SDLK_A)
					player.turn_direction = 0;
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

		// update
		player.Update(deltaTime);

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
		// draw player
		player.Render(renderer);

		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
