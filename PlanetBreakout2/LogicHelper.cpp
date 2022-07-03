#include "LogicHelper.h"

uint32_t GetBrickIndex(uint32_t col, uint32_t row)
{
  return col + GRID_COLUMNS * row;
}

D2D1_RECT_F GetBrickRect(uint32_t col, uint32_t row)
{
  return D2D1::RectF(
    static_cast<float>(col * BRICK_WIDTH),
    static_cast<float>(row * BRICK_HEIGHT),
    static_cast<float>((col * BRICK_WIDTH) + BRICK_WIDTH),
    static_cast<float>((row * BRICK_HEIGHT) + BRICK_HEIGHT));
}

bool IsReservedBrick(uint32_t col, uint32_t row)
{
  return (row == 0 || row >= GRID_ROWS - 4 ||
    col == 0 || col == GRID_COLUMNS - 1);
}

bool IsReservedBrick(const Brick& brick)
{
  return IsReservedBrick(brick.col, brick.row);
}

bool IsInGameSceen(uint32_t x, uint32_t y)
{
  return x >= 0 && x < GRID_COLUMNS&& y >= 0 && y < GRID_ROWS;
}