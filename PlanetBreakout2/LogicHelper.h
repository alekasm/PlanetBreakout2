#pragma once
#include "Constants.h"
#include "GameTypes.h"
#include <vector>

struct LINEF
{
  POINTF p1;
  POINTF p2;
};

struct LineIntersection
{
  enum LineType { TOP, BOTTOM, LEFT, RIGHT } type;
  POINTF point;
};
struct RECTF
{
  LINEF top;
  LINEF left;
  LINEF bottom;
  LINEF right;
  RECTF(const RECT& other);
};

bool IsReservedBrick(const Brick& brick);
bool IsReservedBrick(uint32_t col, uint32_t row);
D2D1_RECT_F GetBrickRectF(uint32_t col, uint32_t row);
RECT GetBrickRect(uint32_t col, uint32_t row);
D2D1_RECT_F AdjustRect(const D2D1_RECT_F& rect, float offsetX, float offsetY);
uint32_t GetBrickIndex(uint32_t col, uint32_t row);
bool IsInGameSceen(uint32_t x, uint32_t y);
bool Intersects(LINEF& a, LINEF& b, POINTF& p);
void Intersect(LINEF& line, RECTF& rect, std::vector<LineIntersection>& type);

