#pragma once
#include "Constants.h"
#include "Button.h"
#include "GameLevel.h"
#include "ResourceLoader.h"

enum class EditorMode { BRICK_SELECT, BG_SELECT };
struct LevelEditor
{
  void initialize();
  ~LevelEditor();
  const unsigned BrickIndexStart();
  const unsigned BrickIndexEnd();
  const unsigned BackgroundIndexStart();
  const unsigned BackgroundIndexEnd();
  std::vector<Button*> brickButtons;
  std::vector<Button*> bgButtons;
  std::vector<Button*> primaryButtons;
  bool showGrid = false;
  EditorMode editorMode = EditorMode::BRICK_SELECT;
  GameLevel editorLevel;
  Brick* currentBrick;
  unsigned currentBrickPage = 0;
  unsigned brickPages = 0;
  unsigned currentBgPage = 0;
  unsigned bgPages = 0;
};