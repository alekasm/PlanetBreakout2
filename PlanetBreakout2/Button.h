#pragma once
#include "Drawable.h"
#include "ResourceLoader.h"
#include "GameController.h"
#include <functional>

struct PrimitiveText
{
  PrimitiveText() = default;
  PrimitiveText(std::wstring text)
  {
    this->text = text;
  }
  void AddChar(wchar_t letter)
  {
    if (text.size() < 16)
      text += letter;
  }

  void DeleteChar()
  {
    if (!text.empty())
      text.erase(text.size() - 1);
  }

  void Update(std::wstring text)
  {
    this->text = text;
  }
  const std::wstring& GetString() const
  {
    return text;
  }
  void Clear()
  {
    text.clear();
  }
private:
  std::wstring text;
};
struct Text : PrimitiveText
{
  IDWriteTextFormat* textFormat;
  ID2D1Brush* textBrush;
  D2D1_RECT_F textRect;

  Text(const D2D1_RECT_F& drawable, const std::wstring& text) :
    PrimitiveText(text), textRect(drawable)
  {
    textFormat = ResourceLoader::GetTextFormat(TextFormat::CENTER_12F);
    textBrush = ResourceLoader::GetBrush(ColorBrush::GRAY);
  }

  Text() = default;

  void FormatText(TextFormat format, ColorBrush brush)
  {
    textFormat = ResourceLoader::GetTextFormat(format);
    textBrush = ResourceLoader::GetBrush(brush);
  }

  void AlignText(float x_offset, float y_offset)
  {
    textRect.left += x_offset;
    textRect.top += y_offset;
  }

};

struct Icon
{
  ID2D1Bitmap* iconBitmap;
  D2D1_RECT_F iconRect;
  Icon(const D2D1_RECT_F& drawable, const std::wstring& sprite) :
    iconRect(drawable)
  {
    iconBitmap = ResourceLoader::GetSpriteMap().at(sprite);
  }
  Icon() = default;

  void CenterX(const D2D1_RECT_F& parent)
  {
    float currentWidth = iconRect.right - iconRect.left;
    float parentWidth = parent.right - parent.left;
    if (currentWidth >= parentWidth) return;
    float offset = (parentWidth - currentWidth) / 2;
    iconRect.left += offset;
    iconRect.right = iconRect.left + currentWidth;
  }
  void AlignIcon(float x_offset, float y_offset)
  {
    iconRect.left += x_offset;
    iconRect.top += y_offset;
  }
};


//typedef void (*Action)(void);
//typedef void (Drawable::* Action2)(void);
enum class ButtonType { NONE, PRIMITIVE, IMAGE };
struct Button : Drawable
{
  bool hasIcon = false;
  bool hasText = false;
  bool highlighted = false;
  bool selected = false;
  ButtonType type;
  int32_t id = -1;
  Icon icon;
  Text text;

  float primitiveStroke;
  ID2D1Brush* primitiveOutline = nullptr;
  ID2D1Brush* primitiveFill = nullptr;
  std::function<void()> action = [](){};

  Button(const Drawable& drawable) : Drawable(drawable),
    type(ButtonType::NONE), hasIcon(false), hasText(false)
  {
  }

  void SetText(const Text& text)
  {
    this->text = text;
    hasText = true;
  }

  void SetIcon(const Icon& icon)
  {
    this->icon = icon;
    hasIcon = true;
  }

  void SetPrimitive(float stroke, ColorBrush outline)
  {
    this->primitiveStroke = stroke;
    this->primitiveOutline = ResourceLoader::GetBrush(outline);
    type = ButtonType::PRIMITIVE;
  }

  void SetPrimitive(float stroke, ColorBrush outline, ColorBrush fill)
  {
    this->primitiveStroke = stroke;
    this->primitiveOutline = ResourceLoader::GetBrush(outline);
    this->primitiveFill = ResourceLoader::GetBrush(fill);
    type = ButtonType::PRIMITIVE;
  }

  void Update()
  {
    if (primitiveFill)
    {
      highlighted = PtInRect(&win32Rect, GameController::GetInstance()->mousePos);
    }
  }

  bool Click()
  {
    if (highlighted)
    {
      action();
      return true;
    }
    return false;
  }
};
