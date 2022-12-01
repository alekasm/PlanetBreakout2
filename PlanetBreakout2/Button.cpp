#include "Button.h"

void Button::SetText(const Text& text)
{
  this->text = text;
  hasText = true;
}


void Button::SetBorder(ID2D1Brush* brush, float thickness)
{
  borderBrush = brush;
  borderStroke = thickness;
}
void Button::SetButtonFill(ID2D1Brush* brush)
{
  fillType = ButtonFillType::FILL;
  fillBrush = brush;
}

void Button::SetId(int32_t id)
{
  this->id = id;
}

int32_t Button::GetId()
{
  return id;
}

bool Button::IsHighlighted()
{
  return highlighted;
}

bool Button::HasText()
{
  return hasText;
}

bool Button::HasIcon()
{
  return hasIcon;
}

bool Button::IsSelected()
{
  return selected || highlighted;
}

void Button::SetSelected(bool value)
{
  selected = value;
}

void Button::SetHighlighted(bool value)
{
  highlighted = value;
}

void Button::SetButtonFill(ID2D1LinearGradientBrush* brush)
{
  fillType = ButtonFillType::FILL;
  float centerX = (d2d1Rect.right - d2d1Rect.left) / 2.f;
  brush->SetStartPoint(D2D1::Point2F(centerX, d2d1Rect.top));
  brush->SetEndPoint(D2D1::Point2F(centerX, d2d1Rect.bottom));
  fillBrush = brush;
}

float Button::GetBorderStroke()
{
  return borderStroke;
}
void Button::SetButtonHighlightType(ButtonHighlightType type, ID2D1Brush* brush)
{
  highlightType = type;
  highlightBrush = brush;
}

ButtonHighlightType Button::GetButtonHighlightType()
{
  return highlightType;
}
ButtonFillType Button::GetButtonFillType()
{
  return fillType;
}
ID2D1Brush* Button::GetFillBrush()
{
  return fillBrush;
}
ID2D1Brush* Button::GetHighlightBrush()
{
  return highlightBrush;
}
ID2D1Brush* Button::GetBorderBrush()
{
  return borderBrush;
}

void Button::UpdateText(std::wstring string)
{
  if (hasText)
  {
    text.Update(string);
  }
}

void Button::SetIcon(const Icon& icon)
{
  this->icon = icon;
  hasIcon = true;
}

void Button::Update()
{
  highlighted = PtInRect(&win32Rect,
    GameController::GetInstance()->GetMousePos());
  //if(!old && highlighted)
  //  ResourceLoader::PlayAudio(L"click.wav");
}

bool Button::Click()
{
  if (highlighted)
  {
    ResourceLoader::PlayAudio(L"click.wav");
    onClick();
    //highlighted = false;
    return true;
  }
  return false;
}
