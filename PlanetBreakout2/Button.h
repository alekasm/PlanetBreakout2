#pragma once
#include "Drawable.h"
#include "ResourceLoader.h"
#include "GameController.h"
#include "PrimitiveText.h"
#include <functional>

struct Text : PrimitiveText
{
  IDWriteTextFormat* textFormat;
  ID2D1Brush* textBrush;
  D2D1_RECT_F textRect;
  TextFormat type;

  Text(const D2D1_RECT_F& drawable, const std::wstring& text) :
    PrimitiveText(text), textRect(drawable)
  {
    type = TextFormat::CENTER_12F;
    textFormat = ResourceLoader::GetTextFormat(type);
    textBrush = ResourceLoader::GetBrush(ColorBrush::GRAY);
  }

  Text() = default;

  void FormatText(TextFormat format, ColorBrush brush)
  {
    textFormat = ResourceLoader::GetTextFormat(format);
    textBrush = ResourceLoader::GetBrush(brush);
  }

  /*
  void AlignText(float x_offset, float y_offset)
  {
    textRect.left += x_offset;
    textRect.top += y_offset;
  }
  */

  void AlignCenter()
  {
    const TextFormatData& data = ResourceLoader::GetTextFormatData(type);
    float currentHeight = data.size;
    float parentHeight = textRect.bottom - textRect.top;
    if (currentHeight >= parentHeight) return;
    float offset = (parentHeight - currentHeight) / 2;
    textRect.top += offset;
    textRect.bottom = textRect.top + currentHeight;
  }

  void AlignBottom()
  {
    const TextFormatData& data = ResourceLoader::GetTextFormatData(type);
    //2 padding is added so the text doesnt sit on the line
    float currentHeight = data.size + 2.f;
    float parentHeight = textRect.bottom - textRect.top;
    if (currentHeight >= parentHeight) return;
    textRect.top = textRect.bottom - currentHeight;
    textRect.bottom = textRect.top + currentHeight;
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

  void UpdateBitmap(std::wstring sprite)
  {
    iconBitmap = ResourceLoader::GetSpriteMap().at(sprite);
  }
  void CenterX(const D2D1_RECT_F& parent)
  {
    float currentWidth = iconRect.right - iconRect.left;
    float parentWidth = parent.right - parent.left;
    if (currentWidth >= parentWidth) return;
    float offset = (parentWidth - currentWidth) / 2;
    iconRect.left += offset;
    iconRect.right = iconRect.left + currentWidth;
  }

  void CenterY(const D2D1_RECT_F& parent)
  {
    float currentHeight = iconRect.bottom - iconRect.top;
    float parentHeight = parent.bottom - parent.top;
    if (currentHeight >= parentHeight) return;
    float offset = (parentHeight - currentHeight) / 2;
    iconRect.top += offset;
    iconRect.bottom = iconRect.top + currentHeight;
  }

  void AlignIcon(float x_offset, float y_offset)
  {
    iconRect.left += x_offset;
    iconRect.top += y_offset;
  }
};


enum class ButtonHighlightType {BORDER, FILL, TEXT};
enum class ButtonFillType { EMPTY, FILL };
struct Button : Drawable
{
private:
  ButtonHighlightType highlightType = ButtonHighlightType::FILL;
  ButtonFillType fillType = ButtonFillType::EMPTY;
  float borderStroke = 1.f;
  ID2D1Brush* fillBrush = nullptr;
  ID2D1Brush* highlightBrush = nullptr;
  ID2D1Brush* borderBrush = nullptr;
  bool hasIcon = false;
  bool hasText = false;
  bool highlighted = false;
  bool selected = false;
  int32_t id = -1;
public:
  void SetBorder(ID2D1Brush*, float);
  void SetButtonFill(ID2D1Brush*);
  void SetButtonFill(ID2D1LinearGradientBrush*);
  void SetButtonHighlightType(ButtonHighlightType, ID2D1Brush*);
  ButtonHighlightType GetButtonHighlightType();
  ButtonFillType GetButtonFillType();
  ID2D1Brush* GetFillBrush();
  ID2D1Brush* GetHighlightBrush();
  ID2D1Brush* GetBorderBrush();
  float GetBorderStroke();
  void SetId(int32_t);
  int32_t GetId();
  bool IsHighlighted();
  void SetSelected(bool);
  void SetHighlighted(bool);
  bool HasText();
  bool HasIcon();
  bool IsSelected();
  void SetText(const Text& text);
  void UpdateText(std::wstring string);
  void SetIcon(const Icon& icon);
  void Update();
  bool Click();

  Icon icon;
  Text text;
  std::function<void()> onClick = []() {};
  Button(const Drawable& drawable) : Drawable(drawable),
    hasIcon(false), hasText(false)
  {
  }
};