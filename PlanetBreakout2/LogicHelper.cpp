#include "LogicHelper.h"
#include <vector>

uint32_t GetBrickIndex(uint32_t col, uint32_t row)
{
  return col + GRID_COLUMNS * row;
}

D2D1_RECT_F GetBrickRectF(uint32_t col, uint32_t row)
{
  return D2D1::RectF(
    static_cast<float>(col * BRICK_WIDTH),
    static_cast<float>(row * BRICK_HEIGHT),
    static_cast<float>((col * BRICK_WIDTH) + BRICK_WIDTH),
    static_cast<float>((row * BRICK_HEIGHT) + BRICK_HEIGHT));
}

RECTF::RECTF(const RECT& other)
{
  LINEF top =
  {
    POINTF {(float)other.left, (float)other.top},
    POINTF {(float)other.right, (float)other.top}
  };
  LINEF bottom =
  {
    POINTF {(float)other.left, (float)other.bottom},
    POINTF {(float)other.right, (float)other.bottom}
  };
  LINEF left =
  {
    POINTF {(float)other.left, (float)other.top},
    POINTF {(float)other.left, (float)other.bottom}
  };
  LINEF right =
  {
    POINTF {(float)other.right, (float)other.top},
    POINTF {(float)other.right, (float)other.bottom}
  };
  this->top = { top };
  this->bottom = {  bottom };
  this->left = { left };
  this->right = { right };
}

bool Intersects(LINEF& a, LINEF& b, POINTF& p)
{
  float a2 = b.p2.y - b.p1.y;
  float b2 = b.p2.x - b.p1.x;
  float c2 = a2 * b.p1.x + b2 * b.p1.y;

  float a1 = a.p2.y - a.p1.y;
  float b1 = a.p2.x - a.p1.x;
  float c1 = a2 * a.p1.x + b2 * a.p1.y;

  float det = a1 * b2 - a2 * b1;
  float abs2 = fabsf(det);
  if (abs2 < 0.001f) return false;

  p.x = (b2 * c1 - b1 * c2) / det;
  p.y = -(a1 * c2 - a2 * c1) / det;
  return true;
}

void Intersect(LINEF& line, RECTF& rect, std::vector<LineIntersection>& intersections)
{
  POINTF p;
  if (Intersects(line, rect.bottom, p))
  {
    intersections.push_back({ LineIntersection::LineType::BOTTOM, p });
  }

  if (Intersects(line, rect.top, p))
  {
    intersections.push_back({ LineIntersection::LineType::TOP, p });
  }
  
  if (Intersects(line, rect.left, p))
  {
    intersections.push_back({ LineIntersection::LineType::LEFT, p });
  }

  if (Intersects(line, rect.top, p))
  {
    intersections.push_back({ LineIntersection::LineType::TOP, p });
  }
}



RECT GetBrickRect(uint32_t col, uint32_t row)
{
  return RECT {
    static_cast<long>(col * BRICK_WIDTH),
    static_cast<long>(row * BRICK_HEIGHT),
    static_cast<long>((col * BRICK_WIDTH) + BRICK_WIDTH),
    static_cast<long>((row * BRICK_HEIGHT) + BRICK_HEIGHT) };
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