#pragma once
#include "Constants.h"
#include "GameTypes.h"
bool IsReservedBrick(const Brick& brick);
bool IsReservedBrick(uint32_t col, uint32_t row);
D2D1_RECT_F GetBrickRect(uint32_t col, uint32_t row);
uint32_t GetBrickIndex(uint32_t col, uint32_t row);
bool IsInGameSceen(uint32_t x, uint32_t y);