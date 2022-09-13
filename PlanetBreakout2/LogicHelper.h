#pragma once
#include "Constants.h"
#include "GameTypes.h"
#include <vector>


bool IsReservedBrick(const Brick& brick);
bool IsReservedBrick(uint32_t col, uint32_t row);
D2D1_RECT_F GetBrickRectF(uint32_t col, uint32_t row);
RECT GetBrickRect(uint32_t col, uint32_t row);
D2D1_RECT_F AdjustRect(const D2D1_RECT_F& rect, float offsetX, float offsetY);
bool GetBrickIndex(uint32_t col, uint32_t row, uint32_t& index);
bool IsInGameSceen(uint32_t x, uint32_t y);
RECT GetBrickRect(uint32_t index);
