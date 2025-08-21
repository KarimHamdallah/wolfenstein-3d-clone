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

struct IntersectionData
{
	bool hit;
	float x, y;
};

IntersectionData RayToLineIntersection(
	float rx, float ry, float rdx, float rdy,
	float x1, float y1, float x2, float y2)
{
	IntersectionData result{ false, 0.0f, 0.0f };

	float sdx = x2 - x1;
	float sdy = y2 - y1;

	float denom = rdx * sdy - rdy * sdx;

	if (fabs(denom) < 1e-6f)
	{
		return result;
	}

	float dx = x1 - rx;
	float dy = y1 - ry;

	float t = (dx * sdy - dy * sdx) / denom;

	if (t >= 0.0f) 
	{
		result.hit = true;
		result.x = rx + t * rdx;
		result.y = ry + t * rdy;
	}

	return result;
}



inline float Distance(float x1, float y1, float x2, float y2)
{
	float dx = x2 - x1;
	float dy = y2 - y1;
	return std::sqrt(dx * dx + dy * dy);
}

inline float NormalizeAngle(float angle)
{
	angle = fmodf(angle, 2.0f * PI);   // wrap within [-2π, 2π]
	if (angle < 0)
		angle += 2.0f * PI;            // shift to [0, 2π)
	return angle;
}

//////////////////// Ray ////////////////////////////////
struct Ray
{
	float x, y;
	float rotation_angle = PI / 2.0f;

	bool isRayFacingDown = 0;
	bool isRayFacingUp = 0;
	bool isRayFacingRight = 0;
	bool isRayFacingLeft = 0;

	float min_intersection_dist = INFINITY;
	float intersection_x = 0.0f;
	float intersection_y = 0.0f;

	void Cast()
	{
		min_intersection_dist = INFINITY;

		float normalized_angle = NormalizeAngle(rotation_angle);
		isRayFacingDown = normalized_angle > 0 && normalized_angle < PI;
		isRayFacingUp = !isRayFacingDown;

		isRayFacingRight = normalized_angle < 0.5 * PI || normalized_angle > 1.5 * PI;
		isRayFacingLeft = !isRayFacingRight;

		// horizontal intersections
		for (size_t i = 0; i < TILE_ROW_NUM; i++)
		{
			auto hit = RayToLineIntersection(
				x, y,
				cosf(rotation_angle),
				sinf(rotation_angle),
				0.0f, TILE_SIZE * i, WINDOW_WIDTH, TILE_SIZE * i);

			if (hit.hit)
			{
				int col = floor(hit.x / TILE_SIZE);
				int raw = i;

				if (isRayFacingUp)
					raw = i - 1;

				if (raw >= 0 && col >= 0 && raw < TILE_ROW_NUM && col < TILES_COL_NUM)
				{
					if (map[raw][col] != 0)
					{
						float dist = Distance(x, y, hit.x, hit.y);
						if (min_intersection_dist > dist)
						{
							min_intersection_dist = dist;
							intersection_x = hit.x;
							intersection_y = hit.y;
						}
					}
				}
			}
		}

		// vertical intersections
		for (size_t i = 0; i < TILES_COL_NUM; i++)
		{
			auto hit = RayToLineIntersection(
				x, y,
				cosf(rotation_angle),
				sinf(rotation_angle),
				TILE_SIZE * i, 0.0f, TILE_SIZE * i, WINDOW_HEIGHT);

			if (hit.hit)
			{
				int raw = floor(hit.y / TILE_SIZE);
				int col = i;

				if (isRayFacingLeft)
					col = i - 1;

				if (raw >= 0 && col >= 0 && raw < TILE_ROW_NUM && col < TILES_COL_NUM)
				{
					if (map[raw][col] != 0)
					{
						float dist = Distance(x, y, hit.x, hit.y);
						if (min_intersection_dist > dist)
						{
							min_intersection_dist = dist;
							intersection_x = hit.x;
							intersection_y = hit.y;
						}
					}
				}
			}
		}
	}

	void Render(SDL_Renderer* renderer)
	{
		DrawOutlinedRect(renderer,
			intersection_x,
			intersection_y,
			10.0f, 10.0f,
			BLUE_COLOR, BLUE_COLOR);

		SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255);

		SDL_RenderLine(renderer,
			MAP_SCALING_FACTOR * (player.x + 0.5f * player.size),
			MAP_SCALING_FACTOR * (player.y + 0.5f * player.size),
			MAP_SCALING_FACTOR * (intersection_x),
			MAP_SCALING_FACTOR * (intersection_y));
	}
};
Ray ray;
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

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();

	// Setup Platform/Renderer bindings
	ImGui_ImplSDL3_InitForSDLRenderer(window, renderer);
	ImGui_ImplSDLRenderer3_Init(renderer);

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

			ImGui_ImplSDL3_ProcessEvent(&event);
		}

		// update
		player.Update(deltaTime);

		// render

		ImGui_ImplSDLRenderer3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();

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

		ray.x = player.x; ray.y = player.y; ray.rotation_angle = player.rotation_angle;
		ray.Cast();
		ray.Render(renderer);

		ImGui::Render();
		ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), renderer);
		SDL_RenderPresent(renderer);
	}

	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}
