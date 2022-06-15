#pragma once
#include "Constants.h"
#include "Button.h"
#include "GameLevel.h"
#include "ResourceLoader.h"

enum class EditorMode { BRICK_SELECT, BG_SELECT };
struct LevelEditor
{

  void initialize()
  {
    {
      Drawable button0draw(GAME_WIDTH + 110 + 20, 50 + (14 + 10), 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Toggle Grid");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { showGrid ^= true; };
      primaryButtons.push_back(button0);
    }

    {
      //Drawable button0draw(GAME_WIDTH + 110 + 20, 50, 100, 14);
      Drawable button0draw(GAME_WIDTH + 20, 50 + (14 + 10) * 2, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Select Brick");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { editorMode = EditorMode::BRICK_SELECT; };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 110 + 20, 50 + (14 + 10) * 2, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Select Bkgnd");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { editorMode = EditorMode::BG_SELECT; };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 110 + 20, 50, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Save");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {  GameLoader::SaveMap(editorLevel); };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 20, 50 + (14 + 10), 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Load");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {
        std::wstring loadmap;
        if (ResourceLoader::GetFile(loadmap))
        {
          GameLoader::LoadMap(loadmap, editorLevel);
          //editorLevel.validate();
        }
      };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 20, 50, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"New");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {
        editorLevel.clear();
      };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 10, 720, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Previous");
      button0->SetText(button0Text); 
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {
        if (editorMode == EditorMode::BRICK_SELECT)
        {
          if (currentBrickPage > 0)
            currentBrickPage--;
        }
        else if (editorMode == EditorMode::BG_SELECT)
        {
          if (currentBgPage > 0)
            currentBgPage--;
        }
      };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(CLIENT_WIDTH - 110, 720, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Next");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {
        if (editorMode == EditorMode::BRICK_SELECT)
        {
          if (currentBrickPage < brickPages)
            currentBrickPage++;
        }
        else if (editorMode == EditorMode::BG_SELECT)
        {
          if (currentBgPage < bgPages)
            currentBgPage++;
        }
      };
      primaryButtons.push_back(button0);
    }

    unsigned brickCount = GameLoader::GetAssetBricks().size();
    brickPages = (brickCount - 1) / EDITOR_BUTTONS_PER_PAGE;
    for (size_t i = 0; i < brickCount; ++i)
    {
      Brick brick = GameLoader::GetAssetBricks().at(i);
      Button* button1 = CreateButton(i);

      std::wstring desc = brick.subtype ==
        BrickType::NORMAL_BRICK ?
        L"Breakable" : L"Invincible";
      
      Text button1Text(button1->d2d1Rect, desc);
      button1Text.AlignText(0.f, BRICK_HEIGHT + 2);
      button1->SetText(button1Text);

      Icon button1icon(D2D1::RectF(
        button1->x, button1->y,
        button1->x + BRICK_WIDTH, button1->y + BRICK_HEIGHT),
        brick.sprite);
      button1icon.CenterX(button1->d2d1Rect);
      button1icon.AlignIcon(0.f, 1.0f);
      button1->SetIcon(button1icon);
      button1->action = [this](void) {
        for (int i = 0; i < brickButtons.size(); ++i)
        {
          if (brickButtons.at(i)->selected) 
          {
            currentBrick = &GameLoader::GetAssetBricks().at(i);
            break;
          }
        }
      };
      brickButtons.push_back(button1);
    }

    unsigned bgCount = GameLoader::GetAssetBackgrounds().size();
    bgPages = (bgCount - 1) / EDITOR_BUTTONS_PER_PAGE;
    for (size_t i = 0; i < bgCount; ++i)
    {
      std::wstring bg = GameLoader::GetAssetBackgrounds().at(i);
      Button* button1 = CreateButton(i);
      Icon button1icon(D2D1::RectF(
        button1->x, button1->y,
        button1->x + TILE_WIDTH, button1->y + TILE_HEIGHT),
        bg);
      button1icon.CenterX(button1->d2d1Rect);
      button1icon.AlignIcon(0.f, 1.0f);
      button1->SetIcon(button1icon);
      button1->action = [this](void) {
        for (int i = 0; i < bgButtons.size(); ++i)
        {
          if (bgButtons.at(i)->selected)
          {
            editorLevel.background = GameLoader::GetAssetBackgrounds().at(i);
            break;
          }
        }
      };
      bgButtons.push_back(button1);
    }


  }

  Button* CreateButton(size_t index)
  {
    unsigned buttonOnPage = index % EDITOR_BUTTONS_PER_PAGE;
    unsigned x = GAME_WIDTH + 10;
    unsigned y = 200 + (buttonOnPage * (EDITOR_BUTTON_HEIGHT + 15));
    if (buttonOnPage > (EDITOR_BUTTONS_PER_PAGE / 2) - 1)
    {
      x += 125;
      y -= (((EDITOR_BUTTONS_PER_PAGE / 2) + 0) * (EDITOR_BUTTON_HEIGHT + 15));
    }
    Drawable button1draw(x, y, EDITOR_BUTTON_WIDTH, EDITOR_BUTTON_HEIGHT);
    Button* button = new Button(button1draw);
    button->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
    return button;
  }

  ~LevelEditor()
  {
    for (Button* brickButton : brickButtons)
      delete brickButton;
    for (Button* bgButton : bgButtons)
      delete bgButton;
    for (Button* primaryButton : primaryButtons)
      delete primaryButton;
    brickButtons.clear();
    bgButtons.clear();
    primaryButtons.clear();
  }

  const unsigned BrickIndexStart()
  {
    return currentBrickPage * EDITOR_BUTTONS_PER_PAGE;
  }

  const unsigned BrickIndexEnd()
  {
    unsigned end = BrickIndexStart();
    unsigned remaining = brickButtons.size() - end;
    if (remaining < EDITOR_BUTTONS_PER_PAGE)
      end += remaining;
    else
      end += EDITOR_BUTTONS_PER_PAGE;
    return end;
  }

  const unsigned BackgroundIndexStart()
  {
    return currentBgPage * EDITOR_BUTTONS_PER_PAGE;
  }

  const unsigned BackgroundIndexEnd()
  {
    unsigned end = BackgroundIndexStart();
    unsigned remaining = bgButtons.size() - end;
    if (remaining < EDITOR_BUTTONS_PER_PAGE)
      end += remaining;
    else
      end += EDITOR_BUTTONS_PER_PAGE;
    return end;
  }

  std::vector<Button*> brickButtons;
  std::vector<Button*> bgButtons;
  std::vector<Button*> primaryButtons;
  bool showGrid = true;
  EditorMode editorMode = EditorMode::BRICK_SELECT;
  GameLevel editorLevel;
  Brick* currentBrick;
  unsigned currentBrickPage = 0;
  unsigned brickPages = 0;
  unsigned currentBgPage = 0;
  unsigned bgPages = 0;
};