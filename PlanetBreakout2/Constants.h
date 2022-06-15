#pragma once
#define CLIENT_WIDTH 1024
#define CLIENT_HEIGHT 768
#define TILE_WIDTH 32
#define TILE_HEIGHT 32
#define GAME_WIDTH 768
#define GAME_HEIGHT 768
#define EDITOR_WIDTH 400
#define EDITOR_HEIGHT 400
#define MENU_COUNT 1
#define BRICK_WIDTH 48
#define BRICK_HEIGHT 16
#define PANEL_WIDTH (CLIENT_WIDTH - GAME_WIDTH)
#define GRID_COLUMNS (GAME_WIDTH / BRICK_WIDTH)
#define GRID_ROWS (GAME_HEIGHT / BRICK_HEIGHT)
#define DEFAULT_CAMPAIGN L"main"
#define MAP_DIRECTORY L"assets\\maps\\"
#define DEFAULT_BACKGROUND L"bg1"
#define EDITOR_BUTTONS_PER_PAGE 16
#define EDITOR_BUTTON_HEIGHT 36
#define EDITOR_BUTTON_WIDTH 110