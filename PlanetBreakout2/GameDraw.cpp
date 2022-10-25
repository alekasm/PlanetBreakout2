#include "Client.h"
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
#include "MainMenu.h"

#pragma comment(lib, "d2d1")
#pragma comment(lib, "Dwmapi")
#pragma comment(lib, "dwrite")

void DrawButton(ID2D1HwndRenderTarget* target, Button* button)
{
  if (button->type == ButtonType::PRIMITIVE)
  {
    if (button->highlighted)
      target->FillRectangle(button->GetD2D1Rect(), button->primitiveFill);
    if (button->selected)
      target->DrawRectangle(button->GetD2D1Rect(), button->primitiveFill, button->primitiveStroke);
    else
      target->DrawRectangle(button->GetD2D1Rect(), button->primitiveOutline, button->primitiveStroke);

    if (button->hasText)
    {
      std::wstring text = button->text.GetString();
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

void DrawEditor(Client* menu, LevelEditor& levelEditor)
{
  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  IDWriteTextFormat* format = ResourceLoader::GetTextFormat(TextFormat::LEFT_12F);
  IDWriteTextFormat* format2 = ResourceLoader::GetTextFormat(TextFormat::CENTER_12F);
  ID2D1Brush* brushes = ResourceLoader::GetBrush(ColorBrush::GRAY);
  ID2D1Brush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);
  ID2D1Brush* blackBrush = ResourceLoader::GetBrush(ColorBrush::BLACK);
  target->BeginDraw();
  if (menu->IsFullScreen())
  {
    target->SetTransform(
      D2D1::Matrix3x2F::Scale(
        D2D1::Size(menu->GetFullScreenScale(),
          menu->GetFullScreenScale())));
  }
  else
  {
    target->SetTransform(D2D1::Matrix3x2F::Identity());
  }
  target->Clear();
  std::wstring text = L"Planet Breakout 2 - Level Editor";
  target->DrawText(text.c_str(), text.length(), format2, D2D1::RectF(GAME_WIDTH + 1, 10, CLIENT_WIDTH - 1, 10 + 12 + 10), brushes);
  target->DrawLine(D2D1::Point2F(GAME_WIDTH, 0.f), D2D1::Point2F(GAME_WIDTH, GAME_HEIGHT), brushes);
  target->DrawLine(D2D1::Point2F(1.f, 0.f), D2D1::Point2F(1.f, GAME_HEIGHT), brushes);
  GameLevel level = levelEditor.editorLevel;
  BrickMap::const_iterator map_it = level.GetBrickMap().begin();
  for (; map_it != level.GetBrickMap().end(); ++map_it)
  {
    if (map_it->second.empty()) continue;
    const Brick& brick = *(--map_it->second.cend());
    target->DrawBitmap(ResourceLoader::GetSpriteMap().at(brick.GetSprite()), brick.GetD2D1Rect(), 1.0f);
    if (levelEditor.showGrid)
    {
      std::wstring count = std::to_wstring(map_it->second.size());
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        map_it->second.at(0).GetD2D1Rect(), blackBrush);
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        AdjustRect(map_it->second.at(0).GetD2D1Rect(), 1.f, 1.f), greenBrush);
    }
  }

  Brick* currentBrick = levelEditor.currentBrick;
  if (currentBrick != nullptr)
  {
    POINT p = GameController::GetInstance()->GetMousePos();
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
        target->DrawBitmap(ResourceLoader::GetSpriteMap().at(currentBrick->GetSprite()),
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

  if (menu->GetShowCursor())
  {
    POINT p = GameController::GetInstance()->GetMousePos();
    target->DrawBitmap(ResourceLoader::GetSpriteMap().at(L"mouse"),
      D2D1::RectF(p.x, p.y, p.x + 16, p.y + 16), 1.0f);
  }

  target->EndDraw();
}

void PrintGameInfo(ID2D1HwndRenderTarget* target, std::wstring header, std::wstring value, float y)
{
  IDWriteTextFormat* formatMedium = ResourceLoader::GetTextFormat(TextFormat::CENTER_18F);
  IDWriteTextFormat* formatMedium2 = ResourceLoader::GetTextFormat(TextFormat::CENTER_14F);
  ID2D1LinearGradientBrush* gradientBrush = (ID2D1LinearGradientBrush*)ResourceLoader::GetBrush(ColorBrush::GRADIENT_1);


  target->DrawText(header.c_str(), header.length(), formatMedium,
    D2D1::RectF(GAME_WIDTH + 1, y, CLIENT_WIDTH - 1, y + 16.f),
    ResourceLoader::GetBrush(ColorBrush::DARK_GREEN));
  
  target->DrawText(header.c_str(), header.length(), formatMedium,
    D2D1::RectF(GAME_WIDTH + 1 + 1, y + 1, CLIENT_WIDTH - 1, 2 + y + 16.f),
    ResourceLoader::GetBrush(ColorBrush::GREEN));


  gradientBrush->SetStartPoint(D2D1::Point2F(GAME_WIDTH + 1, y + 24.f));
  gradientBrush->SetEndPoint(D2D1::Point2F(CLIENT_WIDTH - 1, (y + 24.f) + 16.f));

  target->DrawText(value.c_str(), value.length(), formatMedium2,
    D2D1::RectF(GAME_WIDTH + 1, y + 24.f, CLIENT_WIDTH - 1, (y + 24.f) + 16.f), gradientBrush);
}

void DrawMainMenu(Client* menu, MainMenu& mainMenu)
{
  GameController::GetInstance()->GameUpdate();
  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  ID2D1Brush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);
  IDWriteTextFormat* formatBig = ResourceLoader::GetTextFormat(TextFormat::LEFT_24F);
  IDWriteTextFormat* formatHuge = ResourceLoader::GetTextFormat(TextFormat::CENTER_72F);
  ID2D1Brush* darkGrayBrush = ResourceLoader::GetBrush(ColorBrush::DARK_GRAY);

  target->BeginDraw();
  if (menu->IsFullScreen())
  {
    target->SetTransform(
      D2D1::Matrix3x2F::Scale(
        D2D1::Size(menu->GetFullScreenScale(),
          menu->GetFullScreenScale())));
  }
  else
  {
    target->SetTransform(D2D1::Matrix3x2F::Identity());
  }
  target->Clear();
  for (const Star& star : GameController::GetInstance()->GetStars())
  {
    target->DrawBitmap(
      ResourceLoader::GetSpriteMap().at(star.GetSprite()), star.GetD2D1Rect(), star.GetOpacity());
  }

  target->FillEllipse(D2D1::Ellipse(D2D1::Point2F(
    CLIENT_WIDTH / 2.f, CLIENT_HEIGHT / 2.f), 256.f, 256.f),
    GameController::GetInstance()->GetPlanetEffect()->GetBrush());

  target->DrawEllipse(D2D1::Ellipse(D2D1::Point2F(
    CLIENT_WIDTH / 2.f, CLIENT_HEIGHT / 2.f), 256.f, 256.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK_HALF), 1.f);

  std::wstring text = L"Planet Breakout 2";
  target->DrawText(text.c_str(), text.length(), formatHuge,
    D2D1::RectF(0.f, 100.f, CLIENT_WIDTH - 1, 200.f),
    ResourceLoader::GetBrush(ColorBrush::DARK_GREEN));
  target->DrawText(text.c_str(), text.length(), formatHuge,
    D2D1::RectF(4.f, 104.f, CLIENT_WIDTH - 1, 200.f), greenBrush);

  for (Button* button : mainMenu.GetButtons())
  {
    DrawButton(target, button);
  }

  if (mainMenu.GetState() == MainMenuState::CAMPAIGN_SELECT)
  {
    ID2D1Brush* gradientBrush2 = ResourceLoader::GetBrush(ColorBrush::GRADIENT_2);
    CampaignMap::iterator it = GameLoader::GetCampaigns().begin();
    std::advance(it, mainMenu.GetCampaignPage());
    Campaign& campaign = it->second;
    unsigned i = 1;
    std::wstring name_string;
    std::wstring score_string;
    std::wstring date_string;
    for (const Highscore& h : campaign.GetHighscores())
    {
      wchar_t buffer_score[64];
      wchar_t buffer_name[64];
      std::swprintf(buffer_name,
        sizeof(buffer_name) / sizeof(wchar_t),
        L"%d. %s\n", i, h.name.c_str());
      name_string.append(buffer_name);

      std::swprintf(buffer_score,
        sizeof(buffer_score) / sizeof(wchar_t),
        L"%016d\n", h.score);
      score_string.append(buffer_score);

      tm local;
      localtime_s(&local, &h.date);
      char buffer[256];
      strftime(buffer, sizeof(buffer), "%d %B %Y\n", &local);
      std::string a(buffer);
      date_string.append(std::wstring(a.begin(), a.end()));
      ++i;
    }

    float y = (CLIENT_HEIGHT / 2.f) - 64;
    target->DrawText(name_string.c_str(), name_string.length(), formatBig,
      D2D1::RectF(32.f, y, CLIENT_WIDTH - 32.f, y + 32.f), gradientBrush2);

    target->DrawText(score_string.c_str(), score_string.length(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_24F),
      D2D1::RectF(0.f, y, CLIENT_WIDTH, y + 32.f), gradientBrush2);

    target->DrawText(date_string.c_str(), date_string.length(), formatBig,
      D2D1::RectF(CLIENT_WIDTH - 256.f, y, CLIENT_WIDTH - 32.f, y + 32.f), gradientBrush2);
  }
  else if (mainMenu.GetState() == MainMenuState::INFO)
  {
    const GamePowerUpMap& pwr_map = GameController::GetInstance()->GetGamePowerUpMap();
    GamePowerUpMap::const_iterator pwr_it = pwr_map.begin();
    size_t index = 0;
    for (; pwr_it != pwr_map.end(); ++pwr_it)
    {
      float y = 210.f + (index * 36.f);
      float x = 64.f;

      target->DrawRectangle(
        D2D1::RectF(x, y,
          x + POWERUP_DIMENSION,
          y + POWERUP_DIMENSION),
        darkGrayBrush, 1.0f);
      x += 8.f;
      y += 8.f;
      target->DrawBitmap(
        ResourceLoader::GetSpriteMap().at(pwr_it->second.GetIcon()),
        D2D1::RectF(x, y,
          x + POWERUP_ICON,
          y + POWERUP_ICON), 1.0f);
      std::wstring desc = mainMenu.GetDescription(pwr_it->first);
      target->DrawText(desc.c_str(), desc.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        D2D1::RectF(x + 48.f, y, CLIENT_WIDTH, y + 64.f),
        ResourceLoader::GetBrush(ColorBrush::GREEN));
      ++index;
    }

    std::wstring text_hotkey = L"Hotkeys\nESC = Return to Menu\nF11 = Toggle Fullscreen";
    target->DrawText(text_hotkey.c_str(), text_hotkey.size(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_14F),
      D2D1::RectF(0.f, 660.f, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::GREEN));

    target->DrawText(mainMenu.GetInfoDescription().c_str(),
      mainMenu.GetInfoDescription().length(),
      ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
      D2D1::RectF(0.f, 0.f, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::WHITE));
  }

  if (menu->GetShowCursor())
  {
    POINT p = GameController::GetInstance()->GetMousePos();
    target->DrawBitmap(ResourceLoader::GetSpriteMap().at(L"mouse"),
      D2D1::RectF(p.x, p.y, p.x + 16, p.y + 16), 1.0f);
  }
  target->EndDraw();
}

void DrawGame(Client* menu)
{
  GameController::GetInstance()->GameUpdate();
  ID2D1HwndRenderTarget* target = ResourceLoader::GetHwndRenderTarget();
  IDWriteTextFormat* format = ResourceLoader::GetTextFormat(TextFormat::LEFT_12F);
  IDWriteTextFormat* formatBig = ResourceLoader::GetTextFormat(TextFormat::CENTER_24F);
  IDWriteTextFormat* formatleft16 = ResourceLoader::GetTextFormat(TextFormat::LEFT_16F);
  IDWriteTextFormat* formatMedium = ResourceLoader::GetTextFormat(TextFormat::CENTER_18F);
  IDWriteTextFormat* formatMedium2 = ResourceLoader::GetTextFormat(TextFormat::CENTER_14F);
  IDWriteTextFormat* formatSmallCenter = ResourceLoader::GetTextFormat(TextFormat::CENTER_10F);
  ID2D1Brush* brushes = ResourceLoader::GetBrush(ColorBrush::GRAY);
  ID2D1Brush* greenBrush = ResourceLoader::GetBrush(ColorBrush::GREEN);
  ID2D1Brush* darkGrayBrush = ResourceLoader::GetBrush(ColorBrush::DARK_GRAY);
  ID2D1Brush* darkGreenBrush = ResourceLoader::GetBrush(ColorBrush::DARK_GREEN);
  ID2D1Brush* blackBrush = ResourceLoader::GetBrush(ColorBrush::BLACK);
  ID2D1Brush* orangeBrush = ResourceLoader::GetBrush(ColorBrush::ORANGE);
  ID2D1Brush* redBrush = ResourceLoader::GetBrush(ColorBrush::RED_HALF);
  ID2D1Brush* redFullBrush = ResourceLoader::GetBrush(ColorBrush::RED);

  target->BeginDraw();
  if (menu->IsFullScreen())
  {
    target->SetTransform(
      D2D1::Matrix3x2F::Scale(
        D2D1::Size(menu->GetFullScreenScale(),
          menu->GetFullScreenScale())));
  }
  else
  {
    target->SetTransform(D2D1::Matrix3x2F::Identity());
  }
  target->Clear();

  for (const Star& star : GameController::GetInstance()->GetStars())
  {
    target->DrawBitmap(
      ResourceLoader::GetSpriteMap().at(star.GetSprite()), star.GetD2D1Rect(), star.GetOpacity());
  }

  target->DrawBitmap(ResourceLoader::GetSpriteMap().at(L"sidebar"),
    D2D1::RectF(GAME_WIDTH + 1, 0.f, GAME_WIDTH + 1 + 255, 768.f), 1.f);

  std::wstring text = L"Planet Breakout 2";
  target->DrawText(text.c_str(), text.length(), formatBig,
    D2D1::RectF(GAME_WIDTH + 1, 10, CLIENT_WIDTH - 1, 10 + 24 + 10), darkGreenBrush);
  target->DrawText(text.c_str(), text.length(), formatBig,
    D2D1::RectF(GAME_WIDTH + 2, 1 + 10, CLIENT_WIDTH - 1, 1 + 10 + 24), greenBrush);

  size_t current_level = GameController::GetInstance()->GetCurrentLevel();
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
  PrintGameInfo(target, L"PowerUps", L"", 530.f);
  for (uint32_t i = 0; i < GameController::GetInstance()->GetLives(); ++i)
  {
    if (i > MAX_LIVES) break; //Should not be possible
    float row = (i / 3) == 0 ? 0.0 : (BAT_HEIGHT * 2.f);
    float y = 360.f + 36.f + row;
    float col = (i % 3);
    float x = 16.f + (col * 16.f) + (col * BAT_WIDTH);
    target->DrawBitmap(
      ResourceLoader::GetSpriteMap().at(GameController::GetInstance()->GetBat()->GetSprite()),
      D2D1::RectF(GAME_WIDTH + x, y, GAME_WIDTH + x + BAT_WIDTH, y + BAT_HEIGHT), 1.0f);
  }

  const GamePowerUpMap& pwr_map = GameController::GetInstance()->GetGamePowerUpMap();
  GamePowerUpMap::const_iterator pwr_it = pwr_map.begin();
  size_t index = 0;
  for (; pwr_it != pwr_map.end(); ++pwr_it)
  {
    float row = (index / 4);
    float y = 566.f + (row * 45.f);
    float col = (index % 4);
    float x = 35.f + (col * 20.f) + (col * POWERUP_DIMENSION);


    if (pwr_it->second.IsActive())
    {
      target->DrawRectangle(
        D2D1::RectF(GAME_WIDTH + x + 2, y + 2,
          GAME_WIDTH + x + POWERUP_DIMENSION - 2,
          y + POWERUP_DIMENSION - 2),
        pwr_it->second.IsActive() ? greenBrush : darkGrayBrush, 2.0f);
    }

    if (!pwr_it->second.GetIcon().empty())
    {
      target->DrawBitmap(
        ResourceLoader::GetSpriteMap().at(pwr_it->second.GetIcon()),
        D2D1::RectF(GAME_WIDTH + x + 8.f, y + 8.f,
          GAME_WIDTH + x + POWERUP_ICON + 8.f,
          y + POWERUP_ICON + 8.f), 1.0f);

      //TODO Fix
      if (pwr_it->second.HasTime() && pwr_it->second.IsActive())
      {
        float height_percentage = pwr_it->second.GetPercentRemaining();
        height_percentage *= 26.f; //interior black space
        target->FillRectangle(D2D1::RectF(
          GAME_WIDTH + x + 1 + 2,
          y + 3,
          GAME_WIDTH + x + POWERUP_DIMENSION - 1 - 2,
          y + 3 + height_percentage),
          ResourceLoader::GetBrush(ColorBrush::GREEN_HALF));
      }
    }
    ++index;
  }

  const BrickMap& brickMap = GameController::GetInstance()->GetBrickMap();
  BrickMap::const_iterator map_it = brickMap.begin();
  for (; map_it != brickMap.end(); ++map_it)
  {
    if (map_it->second.empty()) continue;
    auto brick_it = (--map_it->second.cend());
    if (brick_it->subtype == BrickType::NO_POINT && map_it->second.size() > 1)
    {
      auto prev_brick_it = std::prev(brick_it);
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(prev_brick_it->GetSprite()), prev_brick_it->GetD2D1Rect(), 1.0f);
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(brick_it->GetSprite()), brick_it->GetD2D1Rect(), 1.0f);
    }
    else
    {
      target->DrawBitmap(ResourceLoader::GetSpriteMap().at(brick_it->GetSprite()), brick_it->GetD2D1Rect(), 1.0f);
    }
  }

  for (DynamicEffect* effect : GameController::GetInstance()->GetEffects())
  {
    if (effect->IsActive())
      target->DrawGeometry(effect->GetGeometry(), effect->GetBrush(), 2.0f);
  }

  for (const Powerup& powerup : GameController::GetInstance()->GetPowerups())
  {
    if (powerup.IsActive())
    {
      target->DrawBitmap(
        ResourceLoader::GetSpriteMap().at(powerup.GetSprite()), powerup.GetD2D1Rect(), 1.0f);
      target->DrawText(powerup.GetCountString().c_str(),
        powerup.GetCountString().size(), formatSmallCenter,
        powerup.GetCountRect(),
        ResourceLoader::GetBrush((ColorBrush)powerup.GetCountColor()));
    }
  }

  for (const DroneLaser& droneLaser : GameController::GetInstance()->GetDroneLasers())
  {
    D2D1_ELLIPSE ellipse_radius = D2D1::Ellipse(
      D2D1::Point2F(droneLaser.GetD2D1Rect().left + 4.f,
        droneLaser.GetD2D1Rect().top + 4.f), 4.f, 4.f);
    target->FillEllipse(ellipse_radius, ResourceLoader::GetBrush(ColorBrush::PURPLE));
  }

  bool creator_ball = GameController::GetInstance()->IsPowerUpActive(PowerupType::CREATOR_BALL);
  bool ghost = GameController::GetInstance()->IsPowerUpActive(PowerupType::GHOST);
  bool hyper_ball = GameController::GetInstance()->IsPowerUpActive(PowerupType::HYPER_BALL);
  float radius = hyper_ball ? 2.f : 1.f;
  ID2D1Brush* ellipse_brush = greenBrush;
  if (ghost)
    ellipse_brush = ResourceLoader::GetBrush(ColorBrush::WHITE);
  if (hyper_ball)
    ellipse_brush = redFullBrush;
  for (const Ball& ball : GameController::GetInstance()->GetBalls())
  {
    if (ball.IsActive())
    {

      if (!ghost)
      {
        target->DrawBitmap(
          ResourceLoader::GetSpriteMap().at(
            GameController::GetInstance()->GetSpriteForEntity(DynamicSpriteType::BALL)),
          ball.GetD2D1Rect(), 1.0f);
      }

      D2D1_ELLIPSE ellipse_radius = D2D1::Ellipse(
        D2D1::Point2F(ball.GetD2D1Rect().left + 8.f,
          ball.GetD2D1Rect().top + 8.f), 10.f, 10.f);
      target->DrawEllipse(ellipse_radius, ellipse_brush, radius);
      if (creator_ball)
      {
        D2D1_ELLIPSE ellipse_center = D2D1::Ellipse(
          D2D1::Point2F(ball.GetD2D1Rect().left + 8.f,
            ball.GetD2D1Rect().top + 8.f), 4.f, 3.f);
        target->FillEllipse(ellipse_center, ellipse_brush);
      }
    }
  }

  Laser laser = GameController::GetInstance()->GetLaser();
  if (laser.IsActive())
  {
    target->FillRectangle(laser.GetD2D1Rect(), hyper_ball ?
      ResourceLoader::GetBrush(ColorBrush::RED) :
      ResourceLoader::GetBrush(ColorBrush::GRADIENT_1));
  }

  if (GameController::GetInstance()->GetBat()->IsActive())
  {
    target->DrawBitmap(
      ResourceLoader::GetSpriteMap().at(
        GameController::GetInstance()->GetSpriteForEntity(DynamicSpriteType::BAT)),
      GameController::GetInstance()->GetBat()->GetD2D1Rect(), 1.0f);
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
      ResourceLoader::GetBrush(ColorBrush::WHITE));

    std::wstring text2 = L"Left-Click to Exit";
    target->DrawText(text2.c_str(), text2.length(), formatMedium2,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) + 26.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 46.f),
      ResourceLoader::GetBrush(ColorBrush::WHITE));
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

    std::wstring text2 = L"Left-Click to Continue";
    target->DrawText(text2.c_str(), text2.length(), formatMedium2,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) + 26.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 46.f),
      ResourceLoader::GetBrush(ColorBrush::GREEN));
  }
  else if (GameController::GetInstance()->GetLevelState() == LevelState::HIGHSCORE)
  {
    target->FillRectangle(D2D1::RectF(0.f, 0.f, GAME_WIDTH, GAME_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::BLACK_HALF));

    std::wstring text = L"New Highscore!";
    target->DrawText(text.c_str(), text.length(), formatBig,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) - 64.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 32.f),
      ResourceLoader::GetBrush(ColorBrush::WHITE));

    std::wstring name = GameController::GetInstance()->GetHighscoreText().GetString();
    target->DrawText(name.c_str(), name.length(), formatBig,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) - 12.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 14.f),
      ResourceLoader::GetBrush(ColorBrush::ORANGE));

    std::wstring text2 = L"Press Enter to Accept!";
    target->DrawText(text2.c_str(), text2.length(), formatMedium2,
      D2D1::RectF(64.f, (GAME_HEIGHT / 2) + 26.f, GAME_WIDTH - 64.f, (GAME_HEIGHT / 2) + 46.f),
      ResourceLoader::GetBrush(ColorBrush::WHITE));
  }

  target->DrawLine(D2D1::Point2F(GAME_WIDTH, 0.f), D2D1::Point2F(GAME_WIDTH, GAME_HEIGHT), ResourceLoader::GetBrush(ColorBrush::WHITE));
  target->DrawLine(D2D1::Point2F(1.f, 0.f), D2D1::Point2F(1.f, GAME_HEIGHT), ResourceLoader::GetBrush(ColorBrush::WHITE));

  if (menu->GetShowCursor())
  {
    POINT p = GameController::GetInstance()->GetMousePos();
    target->DrawBitmap(ResourceLoader::GetSpriteMap().at(L"mouse"),
      D2D1::RectF(p.x, p.y, p.x + 16, p.y + 16), 1.0f);
  }

  target->EndDraw();
}