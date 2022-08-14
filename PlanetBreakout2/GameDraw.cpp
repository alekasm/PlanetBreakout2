#include "Menu.h"
#include <d2d1_1.h>
#include <Dwmapi.h>
#include <dwrite.h>
#include <string>
#include "ResourceLoader.h"
#include "GameLoader.h"
#include "Constants.h"
#include "GameController.h"
#include "Button.h"
#include "Windowsx.h"
#include "LevelEditor.h"
#include "LogicHelper.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "dwrite")

void DrawButton(ID2D1HwndRenderTarget* target, Button* button)
{
  if (button->type == ButtonType::PRIMITIVE)
  {
    if (button->highlighted)
      target->FillRectangle(button->d2d1Rect, button->primitiveFill);
    if (button->selected)
      target->DrawRectangle(button->d2d1Rect, button->primitiveFill, button->primitiveStroke);
    else
      target->DrawRectangle(button->d2d1Rect, button->primitiveOutline, button->primitiveStroke);

    if (button->hasText)
    {
      std::wstring text = button->text.text;
      if (text.empty())
      {
        if (button->id == LevelEditorButton::AUTHOR_NAME)
          text = L"Author Name";
        else if (button->id == LevelEditorButton::MAP_NAME)
          text = L"Map Name";
      }
      target->DrawText(
        text.c_str(),
        text.length(),
        button->text.textFormat,
        button->text.textRect,
        button->text.textBrush);
    }

    if (button->hasIcon)
      target->DrawBitmap(button->icon.iconBitmap, button->icon.iconRect, 1.0f);
  }
}

void DrawEditor(ClientMenu* menu, LevelEditor& levelEditor)
{
  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  IDWriteTextFormat* format = ResourceLoader::GetTextFormat(TextFormat::LEFT_12F);
  IDWriteTextFormat* format2 = ResourceLoader::GetTextFormat(TextFormat::CENTER_12F);
  ID2D1SolidColorBrush* brushes = ResourceLoader::GetBrush(ColorBrush::GRAY);
  ID2D1SolidColorBrush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);
  ID2D1SolidColorBrush* blackBrush = ResourceLoader::GetBrush(ColorBrush::BLACK);
  target->BeginDraw();
  target->SetTransform(D2D1::Matrix3x2F::Identity());
  target->Clear();
  std::wstring text = L"Planet Breakout 2 - Level Editor";
  target->DrawText(text.c_str(), text.length(), format2, D2D1::RectF(GAME_WIDTH + 1, 10, CLIENT_WIDTH - 1, 10 + 12 + 10), brushes);
  target->DrawLine(D2D1::Point2F(GAME_WIDTH, 0.f), D2D1::Point2F(GAME_WIDTH, GAME_HEIGHT), brushes);
  GameLevel level = levelEditor.editorLevel;
  BrickMap::const_iterator map_it = level.GetBrickMap().begin();
  for (; map_it != level.GetBrickMap().end(); ++map_it)
  {
    if (map_it->second.empty()) continue;
    for (Brick brick : map_it->second)
    {
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(brick.sprite), brick.d2d1Rect, 1.0f);
    }
    if (levelEditor.showGrid)
    {
      std::wstring count = std::to_wstring(map_it->second.size());
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        map_it->second.at(0).d2d1Rect, blackBrush);
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        AdjustRect(map_it->second.at(0).d2d1Rect, 1.f, 1.f), greenBrush);
    }
  }

  Brick* currentBrick = levelEditor.currentBrick;
  if (currentBrick != nullptr)
  {
    POINT p = GameController::GetInstance()->mousePos;
    unsigned x = p.x / BRICK_WIDTH;
    unsigned y = p.y / BRICK_HEIGHT;
    if (IsInGameSceen(x, y))
    {
      if (IsReservedBrick(x, y))
      {
        target->FillRectangle(GetBrickRectF(x, y), ResourceLoader::GetBrush(ColorBrush::RED_HALF));
      }
      else
      {
        target->DrawBitmap(ResourceLoader::GetSpriteMap().at(currentBrick->sprite),
          GetBrickRectF(x, y), 0.5f);
      }
    }
  }

  for (Button* button : levelEditor.primaryButtons)
  {
    DrawButton(target, button);
  }

  unsigned start = levelEditor.BrickIndexStart();
  unsigned end = levelEditor.BrickIndexEnd();
  for (unsigned i = start; i < end; ++i)
  {
    DrawButton(target, levelEditor.brickButtons.at(i));
  }
  std::wstring text_page = L"Page ";
  text_page.append(std::to_wstring(levelEditor.currentBrickPage + 1));
  text_page.append(L" / ");
  text_page.append(std::to_wstring(levelEditor.brickPages + 1));
  //GAME_WIDTH + 10, 720, 100, 14
  target->DrawText(text_page.c_str(), text_page.length(), format,
    D2D1::RectF(GAME_WIDTH + 8, CLIENT_HEIGHT - 20, CLIENT_WIDTH - 4, CLIENT_HEIGHT - 1), brushes);


  if (levelEditor.showGrid)
  {
    for (unsigned x = 0; x < GAME_WIDTH; x += BRICK_WIDTH)
    {
      target->DrawLine(
        D2D1::Point2F((FLOAT)x, 0.f), D2D1::Point2F((FLOAT)x, GAME_HEIGHT),
        brushes);
    }

    for (unsigned y = 0; y < GAME_HEIGHT; y += BRICK_HEIGHT)
    {
      target->DrawLine(
        D2D1::Point2F(0.f, (FLOAT)y), D2D1::Point2F(GAME_WIDTH, (FLOAT)y),
        brushes);
    }
  }
  target->EndDraw();
}

void PrintGameInfo(ID2D1HwndRenderTarget* target, std::wstring header, std::wstring value, float y)
{
  ID2D1SolidColorBrush* orangeBrush = ResourceLoader::GetBrush(ColorBrush::ORANGE);
  IDWriteTextFormat* formatMedium = ResourceLoader::GetTextFormat(TextFormat::CENTER_18F);
  IDWriteTextFormat* formatMedium2 = ResourceLoader::GetTextFormat(TextFormat::CENTER_14F);
  ID2D1SolidColorBrush* darkGrayBrush = ResourceLoader::GetBrush(ColorBrush::DARK_GRAY);
  ID2D1SolidColorBrush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);

  target->DrawText(header.c_str(), header.length(), formatMedium,
    D2D1::RectF(GAME_WIDTH + 1, y, CLIENT_WIDTH - 1, y + 16.f), darkGrayBrush);
  target->DrawText(header.c_str(), header.length(), formatMedium,
    D2D1::RectF(GAME_WIDTH + 3, y + 2, CLIENT_WIDTH - 1, 2 + y + 16.f), greenBrush);

  target->DrawText(value.c_str(), value.length(), formatMedium2,
    D2D1::RectF(GAME_WIDTH + 1, y + 24.f, CLIENT_WIDTH - 1, (y + 24.f ) + 16.f), orangeBrush);
}

void DrawGame(ClientMenu* menu)
{
  //menu->UpdateMousePosition();
  GameController::GetInstance()->GameUpdate();
  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  IDWriteTextFormat* format = ResourceLoader::GetTextFormat(TextFormat::LEFT_12F);
  IDWriteTextFormat* formatBig = ResourceLoader::GetTextFormat(TextFormat::CENTER_24F);
  IDWriteTextFormat* formatleft16 = ResourceLoader::GetTextFormat(TextFormat::LEFT_16F);
  IDWriteTextFormat* formatMedium = ResourceLoader::GetTextFormat(TextFormat::CENTER_18F);
  IDWriteTextFormat* formatMedium2 = ResourceLoader::GetTextFormat(TextFormat::CENTER_14F);
  ID2D1SolidColorBrush* brushes = ResourceLoader::GetBrush(ColorBrush::GRAY);
  ID2D1SolidColorBrush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);
  ID2D1SolidColorBrush* darkGrayBrush = ResourceLoader::GetBrush(ColorBrush::DARK_GRAY);
  ID2D1SolidColorBrush* darkGreenBrush = ResourceLoader::GetBrush(ColorBrush::DARK_GREEN);
  ID2D1SolidColorBrush* blackBrush = ResourceLoader::GetBrush(ColorBrush::BLACK);
  ID2D1SolidColorBrush* orangeBrush = ResourceLoader::GetBrush(ColorBrush::ORANGE);
  target->BeginDraw();
  target->SetTransform(D2D1::Matrix3x2F::Identity());
  target->Clear();

  for(int y = 0; y < CLIENT_WIDTH; y += 64)
    for (int x = GAME_WIDTH; x < CLIENT_WIDTH; x += 64)
    {
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(L"bg1"),
        D2D1::RectF(x, y, x + 64, y + 64), 1.0f);
    }

  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 5.f, CLIENT_WIDTH - 5.f, CLIENT_HEIGHT - 5.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 5.f, CLIENT_WIDTH - 5.f, 40.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));
  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 5.f, CLIENT_WIDTH - 5.f, 40.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 60.f, CLIENT_WIDTH - 5.f, 230.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));
  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 60.f, CLIENT_WIDTH - 5.f, 230.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 300.f, CLIENT_WIDTH - 5.f, 460.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));
  target->FillRectangle(D2D1::RectF(GAME_WIDTH + 5.f, 300.f, CLIENT_WIDTH - 5.f, 460.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));
  
  std::wstring text = L"Planet Breakout 2";
  target->DrawText(text.c_str(), text.length(), formatBig,
    D2D1::RectF(GAME_WIDTH + 1, 10, CLIENT_WIDTH - 1, 10 + 24 + 10), darkGrayBrush);
  target->DrawText(text.c_str(), text.length(), formatBig,
    D2D1::RectF(GAME_WIDTH + 2, 1 + 10, CLIENT_WIDTH - 1, 1 + 10 + 24), greenBrush);


  size_t current_level = GameController::GetInstance()->current_level;
  const GameLevel& level = GameController::GetInstance()->campaign.levels.at(current_level);
  std::wstring campaign_name = GameController::GetInstance()->campaign.name;
  wchar_t buffer_score[64];
  swprintf(buffer_score, sizeof(buffer_score), L"%016d",
    GameController::GetInstance()->GetScore());

  PrintGameInfo(target, L"Campaign", campaign_name, 60.f);
  PrintGameInfo(target, L"Current Level", level.map_name, 120.f);
  PrintGameInfo(target, L"Level Author", level.author, 180.f);
  PrintGameInfo(target, L"Score", std::wstring(buffer_score), 300.f);
  PrintGameInfo(target, L"Lives", L"", 360.f);
  for (uint32_t i = 0; i < GameController::GetInstance()->lives; ++i)
  {
    if (i > MAX_LIVES) break; //Should not be possible
    float row = (i / 3) == 0 ? 0.0 : (BAT_HEIGHT * 2.f);
    float y = 360.f + 36.f + row;
    float col = (i % 3);
    float x = 12.f + (col * 20.f) + (col * BAT_WIDTH);
    target->DrawBitmap(
      ResourceLoader::GetSpriteMap().at(GameController::GetInstance()->bat->sprite),
      D2D1::RectF(GAME_WIDTH + x, y, GAME_WIDTH + x + BAT_WIDTH, y + BAT_HEIGHT), 1.0f);
  }


  const BrickMap& brickMap = GameController::GetInstance()->GetBrickMap();
  BrickMap::const_iterator map_it = brickMap.begin();
  for (; map_it != brickMap.end(); ++map_it)
  {
    if (map_it->second.empty()) continue;
    for (Brick brick : map_it->second)
    {
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(brick.sprite),
        brick.d2d1Rect, 1.0f);
    }
  }
  target->DrawBitmap(
    ResourceLoader::GetSpriteMap().at(GameController::GetInstance()->bat->sprite),
    GameController::GetInstance()->bat->d2d1Rect, 1.0f);

  for (Ball ball : GameController::GetInstance()->balls)
  {
    if (ball.IsActive())
    {
      target->DrawBitmap(
        ResourceLoader::GetSpriteMap().at(ball.sprite), ball.d2d1Rect, 1.0f);
    }
  }

  if (GameController::GetInstance()->GetLevelState() == LevelState::PAUSED)
  {
    target->FillRectangle(D2D1::RectF(0.f, 0.f, GAME_WIDTH, GAME_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

    std::wstring text = L"Game Paused";
    target->DrawText(text.c_str(), text.length(), formatBig,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) - 12.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 14.f),
      ResourceLoader::GetBrush(ColorBrush::WHITE));

    std::wstring text2 = L"Right-Click to Resume";
    target->DrawText(text2.c_str(), text2.length(), formatMedium2,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) + 26.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 46.f),
      ResourceLoader::GetBrush(ColorBrush::WHITE));
  }
  else if (GameController::GetInstance()->GetLevelState() == LevelState::GAME_OVER)
  {
    target->FillRectangle(D2D1::RectF(0.f, 0.f, GAME_WIDTH, GAME_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

    std::wstring text = L"Game Over";
    target->DrawText(text.c_str(), text.length(), formatBig,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) - 12.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 14.f),
      ResourceLoader::GetBrush(ColorBrush::GREEN));
  }
  else if (GameController::GetInstance()->GetLevelState() == LevelState::START)
  {
    std::wstring text = L"Left-Click to Launch!";
    target->DrawText(text.c_str(), text.length(), format,
      D2D1::RectF(4.f, 4.f, GAME_WIDTH - 4.f, 16.f),
      ResourceLoader::GetBrush(ColorBrush::GREEN));
  }
  else if (GameController::GetInstance()->GetLevelState() == LevelState::END)
  {
    target->FillRectangle(D2D1::RectF(0.f, 0.f, GAME_WIDTH, GAME_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

    std::wstring text = L"Level Complete!";
    target->DrawText(text.c_str(), text.length(), formatBig,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) - 12.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 14.f),
      ResourceLoader::GetBrush(ColorBrush::GREEN));
  }

  target->DrawLine(D2D1::Point2F(GAME_WIDTH, 0.f), D2D1::Point2F(GAME_WIDTH, GAME_HEIGHT), brushes);
  target->EndDraw();
}