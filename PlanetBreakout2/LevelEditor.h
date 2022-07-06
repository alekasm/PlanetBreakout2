#pragma once
#include "Constants.h"
#include "Button.h"
#include "GameLevel.h"
#include "ResourceLoader.h"

struct LevelEditor
{
  void initialize();
  ~LevelEditor();
  const unsigned BrickIndexStart();
  const unsigned BrickIndexEnd();
  void ClearSelected();
  std::vector<Button*> brickButtons;
  std::vector<Button*> bgButtons;
  std::vector<Button*> primaryButtons;
  bool showGrid = false;
  GameLevel editorLevel;
  Brick* currentBrick = nullptr;
  Button* buttonTextSelect = nullptr;
  unsigned currentBrickPage = 0;
  unsigned brickPages = 0;
//private:
//  Button* buttonMapName;
//  Button* buttonAuthorName;
};