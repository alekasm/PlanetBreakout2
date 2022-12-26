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

  if (button->IsSelected() && button->GetButtonHighlightType() == ButtonHighlightType::FILL)
    target->FillRectangle(button->GetD2D1Rect(), button->GetHighlightBrush());
  else if (button->GetButtonFillType() == ButtonFillType::FILL)
    target->FillRectangle(button->GetD2D1Rect(), button->GetFillBrush());


  if (button->IsSelected() && button->GetButtonHighlightType() == ButtonHighlightType::BORDER)
    target->DrawRectangle(button->GetD2D1Rect(), button->GetHighlightBrush(), button->GetBorderStroke());
  else
    target->DrawRectangle(button->GetD2D1Rect(), button->GetBorderBrush(), button->GetBorderStroke());

  if (button->HasText())
  {
    ID2D1Brush* textBrush;
    if (button->IsSelected() && button->GetButtonHighlightType() == ButtonHighlightType::TEXT)
      textBrush = button->GetHighlightBrush();
    else
      textBrush = button->text.textBrush;
    std::wstring text = button->text.GetString();
    if (text.empty())
    {
      if (button->GetPriority() == LevelEditorButton::AUTHOR_NAME)
        text = L"Author Name";
      else if (button->GetPriority() == LevelEditorButton::MAP_NAME)
        text = L"Map Name";
    }
    target->DrawText(
      text.c_str(),
      text.length(),
      button->text.textFormat,
      button->text.textRect,
      button->text.textBrush);
  }
  if (button->HasIcon())
    target->DrawBitmap(button->icon.iconBitmap, button->icon.iconRect, 1.0f);
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
  {
    std::wstring text = L"Planet Breakout 2 - Level Editor";
    target->DrawText(text.c_str(), text.length(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_14F),
      D2D1::RectF(GAME_WIDTH + 1, 20, CLIENT_WIDTH - 1, 10 + 12 + 10), brushes);
  }
  {
    std::wstring text = L"Invincible Brick";
    target->DrawText(text.c_str(), text.length(),
      ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
      D2D1::RectF(GAME_WIDTH + 50, 23 + (14 + 22) * 7, CLIENT_WIDTH - 1, 23 + (14 + 22) * 8), brushes);
  }

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
      if (brick.subtype == BrickType::INVINCIBLE_BRICK)
        count = L"inf";
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        map_it->second.at(0).GetD2D1Rect(), blackBrush);
      target->DrawTextA(count.c_str(), count.length(),
        ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
        AdjustRect(map_it->second.at(0).GetD2D1Rect(), 1.f, 1.f), greenBrush);

    }
  }

  std::wstring currentBrick = levelEditor.currentBrick;
  if (!currentBrick.empty())
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
        target->DrawBitmap(ResourceLoader::GetSpriteMap().at(currentBrick),
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
  target->DrawText(text_page.c_str(), text_page.length(), format2,
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


void DrawGradient(ID2D1HwndRenderTarget* target, IDWriteTextFormat* format, std::wstring value, const D2D1_RECT_F& rect)
{
  ID2D1LinearGradientBrush* gradientBrush = (ID2D1LinearGradientBrush*)ResourceLoader::GetBrush(ColorBrush::GRADIENT_1);
  gradientBrush->SetStartPoint(D2D1::Point2F(rect.right - rect.left / 2.f, rect.top));
  gradientBrush->SetEndPoint(D2D1::Point2F(rect.right - rect.left / 2.f, rect.bottom));
  target->DrawText(value.c_str(), value.length(), format, rect, gradientBrush);
}

void DrawDropShadow(ID2D1HwndRenderTarget* target, IDWriteTextFormat* format, ID2D1Brush* brush,
  std::wstring value, const D2D1_RECT_F& rect)
{

  target->DrawText(value.c_str(), value.length(), format,
    D2D1::RectF(rect.left + 1.f, rect.top + 2.f, rect.right + 1.f, rect.bottom + 2.f),
    ResourceLoader::GetBrush(ColorBrush::BLACK));

  target->DrawText(value.c_str(), value.length(), format, rect,
    brush);
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
    ResourceLoader::GetBrush(ColorBrush::MAIN_MENU_DROP_GRADIENT));
  target->DrawText(text.c_str(), text.length(), formatHuge,
    D2D1::RectF(4.f, 104.f, CLIENT_WIDTH - 1, 200.f),
    ResourceLoader::GetBrush(ColorBrush::MAIN_MENU_GRADIENT));

  for (Button* button : mainMenu.GetButtons())
  {
    DrawButton(target, button);
  }

  if (mainMenu.GetState() == MainMenuState::CAMPAIGN_SELECT)
  {
    ID2D1Brush* gradientBrush2 = ResourceLoader::GetBrush(ColorBrush::HIGHSCORE_GRADIENT);
    std::vector<Campaign>::iterator it = GameLoader::GetCampaigns().begin();
    std::advance(it, mainMenu.GetCampaignPage());
    Campaign& campaign = *it;
    unsigned i = 1;
    std::wstring name_string;
    std::wstring score_string;
    std::wstring date_string;
    const DifficultyType difficulty = GameController::GetInstance()->GetDifficultyType();
    for (const Highscore& h : campaign.GetHighscores(difficulty))
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

    std::wstring campaignName = mainMenu.GetCurrentCampaignNameSelection();
    target->DrawText(campaignName.c_str(), campaignName.length(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_24F),
      D2D1::RectF(0.f, (CLIENT_HEIGHT / 2) - 160, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::WHITE));
    target->DrawRectangle(D2D1::RectF(
      (CLIENT_WIDTH / 2) - 140,
      (CLIENT_HEIGHT / 2) - 160,
      (CLIENT_WIDTH / 2) + 140,
      (CLIENT_HEIGHT / 2) - 130),
      ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);

    std::wstring difficultyName = mainMenu.GetCurrentDifficultySelection();
    target->DrawText(difficultyName.c_str(), difficultyName.length(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_24F),
      D2D1::RectF(0.f, (CLIENT_HEIGHT / 2) - 110, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::WHITE));

    target->DrawRectangle(D2D1::RectF(
      (CLIENT_WIDTH / 2) - 140,
      (CLIENT_HEIGHT / 2) - 110,
      (CLIENT_WIDTH / 2) + 140,
      (CLIENT_HEIGHT / 2) - 80),
      ResourceLoader::GetBrush(ColorBrush::GRAY), 1.f);

    float y = (CLIENT_HEIGHT / 2.f) + 30.f;
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
    std::wstring select_powerup = L"Select a PowerUp for more information";
    target->DrawText(select_powerup.c_str(), select_powerup.size(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_14F),
      D2D1::RectF(0.f, 320.f, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::WHITE));

    std::wstring info_text = mainMenu.GetDescription(mainMenu.GetPowerUpSelection());
    target->DrawText(info_text.c_str(), info_text.size(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_14F),
      D2D1::RectF(0.f, 470.f, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::GREEN));


    target->DrawRectangle(D2D1::RectF(340.f, 570.f, 684.f, 684.f),
      ResourceLoader::GetBrush(ColorBrush::GRAY), 2.f);

    std::wstring text_hotkey1 = L"Controls";
    target->DrawText(text_hotkey1.c_str(), text_hotkey1.size(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_14F),
      D2D1::RectF(340.f, 584.f, 684.f, 660.f),
      ResourceLoader::GetBrush(ColorBrush::WHITE));

    std::wstring text_hotkey = L"\nESC = Return to Menu\nF9 = Toggle Audio\nF11 = Toggle Fullscreen\nRight Click = Pause";
    target->DrawText(text_hotkey.c_str(), text_hotkey.size(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_14F),
      D2D1::RectF(340.f, 584.f, 684.f, 660.f),
      ResourceLoader::GetBrush(ColorBrush::GREEN));

    std::wstring author = L"Written by Aleksander Krimsky\nwww.krimsky.net";
    target->DrawText(author.c_str(), author.length(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_12F),
      D2D1::RectF(0.f, 190.f, CLIENT_WIDTH, CLIENT_HEIGHT),
      ResourceLoader::GetBrush(ColorBrush::GREEN));

    std::wstring version = mainMenu.GetVersion();
    target->DrawText(version.c_str(), version.length(),
      ResourceLoader::GetTextFormat(TextFormat::CENTER_12F),
      D2D1::RectF(0.f, 220.f, CLIENT_WIDTH, CLIENT_HEIGHT),
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
  IDWriteTextFormat* formatCenter36f = ResourceLoader::GetTextFormat(TextFormat::CENTER_36F);
  ID2D1Brush* grayBrush = ResourceLoader::GetBrush(ColorBrush::GRAY);
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
    D2D1::RectF(GAME_WIDTH + 1, 13, CLIENT_WIDTH - 1, 13 + 24 + 10), blackBrush);
  target->DrawText(text.c_str(), text.length(), formatBig,
    D2D1::RectF(GAME_WIDTH + 2, 1 + 13, CLIENT_WIDTH - 1, 1 + 13 + 24), ResourceLoader::GetBrush(ColorBrush::GRAY));

  size_t current_level = GameController::GetInstance()->GetCurrentLevel();
  const GameLevel& level = GameController::GetInstance()->GetCampaign().levels.at(current_level);
  std::wstring campaign_name = GameController::GetInstance()->GetCampaign().name;
  wchar_t buffer_score[64];
  swprintf(buffer_score, sizeof(buffer_score) / sizeof(wchar_t), L"%016d",
    GameController::GetInstance()->GetScore());
  std::wstring wscore(buffer_score);

  
  std::wstring w_mission = L"Mission";
  target->DrawText(w_mission.c_str(), w_mission.length(),
    ResourceLoader::GetTextFormat(TextFormat::CENTER_12F),
    D2D1::RectF(
      GAME_WIDTH + 1 + 58,
      59,
      GAME_WIDTH + 1 + 58 + 140,
      59 + 14), ResourceLoader::GetBrush(ColorBrush::DARK_GRAY));
  
  
  /*
  DrawDropShadow(target,
    ResourceLoader::GetTextFormat(TextFormat::CENTER_12F),
    grayBrush,
    L"Mission", D2D1::RectF(
    GAME_WIDTH + 1 + 58,
    59,
    GAME_WIDTH + 1 + 58 + 140,
    59 + 14));
  */

  target->DrawText(campaign_name.c_str(), campaign_name.length(), formatMedium,
    D2D1::RectF(
      GAME_WIDTH + 1 + 13,
      9 + 73,
      GAME_WIDTH + 1 + 13 + 230,
      9 + 73 + 37), greenBrush);

  
  std::wstring w_level = L"Level";
  target->DrawText(w_level.c_str(), w_level.length(),
    ResourceLoader::GetTextFormat(TextFormat::CENTER_12F),
    D2D1::RectF(
      GAME_WIDTH + 1 + 58,
      116,
      GAME_WIDTH + 1 + 58 + 140,
      116 + 14), ResourceLoader::GetBrush(ColorBrush::DARK_GRAY));

  target->DrawText(level.map_name.c_str(), level.map_name.length(),
    ResourceLoader::GetTextFormat(TextFormat::CENTER_TRIM_18F),
    D2D1::RectF(
      GAME_WIDTH + 1 + 13,
      6 + 130,
      GAME_WIDTH + 1 + 13 + 230,
      6 + 130 + 37), greenBrush);

  
  std::wstring w_author = L"Author";
  target->DrawText(w_author.c_str(), w_author.length(),
    ResourceLoader::GetTextFormat(TextFormat::CENTER_12F),
    D2D1::RectF(
      GAME_WIDTH + 1 + 58,
      173,
      GAME_WIDTH + 1 + 58 + 140,
      173 + 14), ResourceLoader::GetBrush(ColorBrush::DARK_GRAY));
  
  target->DrawText(level.author.c_str(), level.author.length(),
    ResourceLoader::GetTextFormat(TextFormat::CENTER_TRIM_18F),
    D2D1::RectF(
      GAME_WIDTH + 1 + 13,
      9 + 187,
      GAME_WIDTH + 1 + 13 + 230,
      9 + 187 + 37), greenBrush);

  
  std::wstring w_score = L"Score";
  target->DrawText(w_score.c_str(), w_score.length(), formatMedium,
    D2D1::RectF(
      GAME_WIDTH + 1 + 13,
      243,
      GAME_WIDTH + 1 + 13 + 230,
      243 + 22), ResourceLoader::GetBrush(ColorBrush::DARK_GRAY));
  
  DrawGradient(target,
    ResourceLoader::GetTextFormat(TextFormat::CENTER_TRIM_18F),
    wscore, D2D1::RectF(
    GAME_WIDTH + 1 + 13,
    9 + 271,
    GAME_WIDTH + 1 + 13 + 230,
    9 + 271 + 37));

  
  std::wstring w_lives = L"Lives";
  target->DrawText(w_lives.c_str(), w_lives.length(), formatMedium,
    D2D1::RectF(
      GAME_WIDTH + 1 + 13,
      328,
      GAME_WIDTH + 1 + 13 + 230,
      328 + 22),
    ResourceLoader::GetBrush(ColorBrush::DARK_GRAY));  

  for (uint32_t i = 0; i < GameController::GetInstance()->GetLives(); ++i)
  {
    if (i > MAX_LIVES) break; //Should not be possible
    float row = (i / 3) == 0 ? 0.0 : (BAT_HEIGHT * 2.f);
    float y = 356.f + 16.f + row;
    float col = (i % 3);
    float x = 16.f + (col * 16.f) + (col * BAT_WIDTH);
    target->DrawBitmap(
      ResourceLoader::GetSpriteMap().at(GameController::GetInstance()->GetBat()->GetSprite()),
      D2D1::RectF(GAME_WIDTH + x, y, GAME_WIDTH + x + BAT_WIDTH, y + BAT_HEIGHT), 1.0f);
  }

  
  std::wstring w_powerups = L"PowerUps";
  target->DrawText(w_powerups.c_str(), w_powerups.length(), formatMedium,
    D2D1::RectF(
      GAME_WIDTH + 1 + 13,
      455,
      GAME_WIDTH + 1 + 13 + 230,
      455 + 22),
    ResourceLoader::GetBrush(ColorBrush::DARK_GRAY));
  

  const GamePowerUpMap& pwr_map = GameController::GetInstance()->GetGamePowerUpMap();
  GamePowerUpMap::const_iterator pwr_it = pwr_map.begin();
  size_t index = 0;
  for (; pwr_it != pwr_map.end(); ++pwr_it)
  {
    int row = (index % 6);
    float y = 491.f + (row * 38);
    //float y = 566.f + (row * 45.f);
    int col = (index / 6);
    float x = col == 0 ? 15.f : 131.f;
    float xinfo = col == 0 ? 51.f : 167.f;
    
    if (pwr_it->second.IsActive())
    {
      target->DrawRectangle(
        D2D1::RectF(GAME_WIDTH + x + 2, y + 2,
          GAME_WIDTH + x + POWERUP_DIMENSION - 2,
          y + POWERUP_DIMENSION - 2),
        pwr_it->second.IsActive() ? greenBrush : darkGrayBrush, 2.0f);

      target->DrawTextA(
        PowerupNames[pwr_it->first].c_str(),
        PowerupNames[pwr_it->first].size(),
        ResourceLoader::GetTextFormat(CENTER_10F),
        D2D1::RectF(
          GAME_WIDTH + xinfo,
          10 + y,
          GAME_WIDTH + xinfo + 72,
          10 + y + 30),
        greenBrush);
    }
    else
    {
     target->DrawTextA(
       PowerupNames[pwr_it->first].c_str(),
       PowerupNames[pwr_it->first].size(),
       ResourceLoader::GetTextFormat(CENTER_10F),
       D2D1::RectF(
          GAME_WIDTH + xinfo,
          10 + y,
          GAME_WIDTH + xinfo + 72,
          10 + y + 30),
       ResourceLoader::GetBrush(ColorBrush::GRAY));
    }

    if (!pwr_it->second.GetIcon().empty())
    {
      target->DrawBitmap(
        ResourceLoader::GetSpriteMap().at(pwr_it->second.GetIcon()),
        D2D1::RectF(GAME_WIDTH + x + 8.f, y + 8.f,
          GAME_WIDTH + x + POWERUP_ICON + 8.f,
          y + POWERUP_ICON + 8.f), 1.0f);

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
  bool hyper_ball = GameController::GetInstance()->IsPowerUpActive(PowerupType::HYPER_ENERGY);
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
      ResourceLoader::GetBrush(ColorBrush::PURPLE));
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

  /*
  for (unsigned x = 0; x < GAME_WIDTH; x += BRICK_WIDTH)
  {
    target->DrawLine(
      D2D1::Point2F((FLOAT)x, 0.f), D2D1::Point2F((FLOAT)x, GAME_HEIGHT),
      brushes);

    std::wstring count = std::to_wstring(x / BRICK_WIDTH);
    target->DrawTextA(count.c_str(), count.length(),
      ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
      D2D1::RectF(x + 1.f, 0.f,
        (x + BRICK_WIDTH), 14.f), greenBrush);

  }

  for (unsigned y = 0; y < GAME_HEIGHT; y += BRICK_HEIGHT)
  {
    target->DrawLine(
      D2D1::Point2F(0.f, (FLOAT)y), D2D1::Point2F(GAME_WIDTH, (FLOAT)y),
      brushes);

    std::wstring count = std::to_wstring(y / BRICK_HEIGHT);
    target->DrawTextA(count.c_str(), count.length(),
      ResourceLoader::GetTextFormat(TextFormat::LEFT_12F),
      D2D1::RectF(0.f, y + 1.f,
        14.f, y + 16.f), greenBrush);
  }
  */

  target->EndDraw();
}