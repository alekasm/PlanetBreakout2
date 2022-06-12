#pragma once
#include <Windows.h>
#include <d2d1helper.h>

struct Drawable
{
  D2D1_RECT_F d2d1Rect;
  RECT win32Rect;
  unsigned x, y, width, height;
  Drawable(unsigned x, unsigned y, unsigned width, unsigned height) :
    x(x), y(y), width(width), height(height)
  {
    RecalculateBounds();
  }

  Drawable(const Drawable& other) :
    x(other.x), y(other.y), width(other.width), height(other.height)
  {
    RecalculateBounds();
  }

  void RecalculateBounds()
  {
    win32Rect.left = x;
    win32Rect.right = x + width;
    win32Rect.top = y;
    win32Rect.bottom = y + height;
    d2d1Rect = D2D1::RectF(
      static_cast<float>(win32Rect.left),
      static_cast<float>(win32Rect.top),
      static_cast<float>(win32Rect.right),
      static_cast<float>(win32Rect.bottom));
  }

  void Update(unsigned x, unsigned y)
  {
    this->x = x;
    this->y = y;
    RecalculateBounds();
  }
};