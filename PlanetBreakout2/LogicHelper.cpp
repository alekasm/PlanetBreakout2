#include "LogicHelper.h"
#include <vector>

bool GetBrickIndex(uint32_t col, uint32_t row, uint32_t& index)
{
  if (col >= GRID_COLUMNS)
    return false;
  if (row >= GRID_ROWS)
    return false;
  index = col + GRID_COLUMNS * row;
  return true;
}

D2D1_RECT_F GetBrickRectF(uint32_t col, uint32_t row)
{
  return D2D1::RectF(
    static_cast<float>(col * BRICK_WIDTH),
    static_cast<float>(row * BRICK_HEIGHT),
    static_cast<float>((col * BRICK_WIDTH) + BRICK_WIDTH),
    static_cast<float>((row * BRICK_HEIGHT) + BRICK_HEIGHT));
}

RECT GetBrickRect(uint32_t col, uint32_t row)
{
  return RECT {
    static_cast<long>(col * BRICK_WIDTH),
    static_cast<long>(row * BRICK_HEIGHT),
    static_cast<long>((col * BRICK_WIDTH) + BRICK_WIDTH),
    static_cast<long>((row * BRICK_HEIGHT) + BRICK_HEIGHT) };
}

RECT GetBrickRect(uint32_t index)
{
  unsigned col = index % GRID_COLUMNS;
  unsigned row = index / GRID_COLUMNS;
  return GetBrickRect(col, row);
}

bool IsReservedBrick(uint32_t col, uint32_t row)
{
  return row >= GRID_ROWS - 6;
}

bool IsReservedBrick(const Brick& brick)
{
  return IsReservedBrick(brick.col, brick.row);
}

bool IsInGameSceen(uint32_t x, uint32_t y)
{
  return x >= 0 && x < GRID_COLUMNS && y >= 0 && y < GRID_ROWS;
}

D2D1_RECT_F AdjustRect(const D2D1_RECT_F& rect, float offsetX, float offsetY)
{
  D2D1_RECT_F copy(rect);
  copy.left += offsetX;
  copy.right += offsetX;
  copy.top += offsetY;
  copy.bottom += offsetY;
  return copy;
}