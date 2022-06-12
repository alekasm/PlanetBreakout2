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
    //Add 2 to the font height so its event
    {
      Drawable button0draw(GAME_WIDTH + 20, 50, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Toggle Grid");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { showGrid ^= true; };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 110 + 20, 50, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Select Brick");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { editorMode = EditorMode::BRICK_SELECT; };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 20, 50 + 14 + 10, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Select Bkgnd");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { editorMode = EditorMode::BG_SELECT; };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 110 + 20, 50 + (14 + 10), 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Save");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {  GameLoader::SaveMap(editorLevel); };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 20, 50 + (14 + 10) * 2, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Load");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() {
        std::wstring loadmap;
        if (ResourceLoader::GetFile(loadmap))
        {
          GameLoader::LoadMap(loadmap, editorLevel);
        }
      };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(GAME_WIDTH + 110 + 20, 50 + (14 + 10) * 2, 100, 14);
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
      Drawable button0draw(GAME_WIDTH + 4,
        125,
        CLIENT_WIDTH - GAME_WIDTH - 6,
        60);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"z");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY);
      selectionButton = button0;
    }

    {
      Drawable button0draw(GAME_WIDTH + 10, 720, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Previous");
      button0->SetText(button0Text); 
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { if (currentBrickPage > 0) currentBrickPage--; };
      primaryButtons.push_back(button0);
    }

    {
      Drawable button0draw(CLIENT_WIDTH - 110, 720, 100, 14);
      Button* button0 = new Button(button0draw);
      Text button0Text(button0draw.d2d1Rect, L"Next");
      button0->SetText(button0Text);
      button0->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      button0->action = [this]() { if (currentBrickPage < brickPages) currentBrickPage++; };
      primaryButtons.push_back(button0);
    }

    const unsigned brickButtonHeight = 14 + BRICK_HEIGHT + 2;
    unsigned brickCount = GameLoader::GetAssetBricks().size();
    brickPages = (brickCount - 1) / BRICKS_PER_PAGE;
    for (unsigned i = 0; i < brickCount; ++i)
    {
      Brick brick = GameLoader::GetAssetBricks().at(i);
      unsigned brickOnPage = i % BRICKS_PER_PAGE;
      currentBrickPage = (i / BRICKS_PER_PAGE);
      //left or right
      unsigned x = GAME_WIDTH + 10;
      unsigned y = 200 + (brickOnPage * (brickButtonHeight + 15));
      if (brickOnPage > (BRICKS_PER_PAGE / 2) - 1)
      {
        x += 125;
        y -= (((BRICKS_PER_PAGE / 2) + 0) * (brickButtonHeight + 15));
      }

      Drawable button1draw(x, y, 110, brickButtonHeight);
      Button* button1 = new Button(button1draw);
      button1->SetPrimitive(1.0f, ColorBrush::GRAY, ColorBrush::GREEN);
      
      std::wstring desc = brick.subtype ==
        BrickType::NORMAL_BRICK ?
        L"Breakable" : L"Invincible";
      
      Text button1Text(button1draw.d2d1Rect, desc);
      button1Text.AlignText(0.f, BRICK_HEIGHT + 2);
      button1->SetText(button1Text);

      Icon button1icon(D2D1::RectF(
        button1draw.x, button1draw.y,
        button1draw.x + BRICK_WIDTH, button1draw.y + BRICK_HEIGHT),
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
          }
        }
      };
      brickButtons.push_back(button1);
    }

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
    return currentBrickPage * BRICKS_PER_PAGE;
  }

  const unsigned BrickIndexEnd()
  {
    unsigned end = BrickIndexStart();
    if (brickButtons.size() < BRICKS_PER_PAGE)
      end += brickButtons.size();
    else
      end += BRICKS_PER_PAGE;
    return end;
  }

  std::vector<Button*> brickButtons;
  std::vector<Button*> bgButtons;
  std::vector<Button*> primaryButtons;
  bool showGrid = true;
  EditorMode editorMode = EditorMode::BRICK_SELECT;
  GameLevel editorLevel;
  //std::wstring mapName;
  Brick* currentBrick;
  Button* selectionButton;
  unsigned currentBrickPage = 0;
  unsigned brickPages = 0;
  unsigned currentBgPage = 0;
  unsigned bgPages = 0;
};