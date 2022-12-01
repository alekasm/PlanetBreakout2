#pragma once
#include "Constants.h"
#include "Button.h"
#include "GameLevel.h"
#include "ResourceLoader.h"
#include "Client.h"

enum LevelEditorButton { MAP_NAME, AUTHOR_NAME };
struct LevelEditor
{
  void initialize(Client*);
  ~LevelEditor();
  const unsigned BrickIndexStart();
  const unsigned BrickIndexEnd();
  void ClearSelected();
  std::vector<Button*> brickButtons;
  std::vector<Button*> bgButtons;
  std::vector<Button*> primaryButtons;
  bool showGrid = false;
  GameLevel editorLevel;
  Button* buttonTextSelect = nullptr;
  unsigned currentBrickPage = 0;
  unsigned brickPages = 0;
  std::wstring currentBrick = L"";
  bool invincibleBrick = false;
private:
  Button* buttonMapName;
  Button* buttonAuthorName;
};