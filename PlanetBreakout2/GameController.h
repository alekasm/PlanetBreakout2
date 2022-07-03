#pragma once
#include <string>
#include "GameLevel.h"
enum GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR};
struct GameController
{
  static GameController* GetInstance();
  GameType type;
  std::wstring map_filename;
  GameLevel current_level;
  uint64_t points = 0;
  uint64_t lives = 0;
  POINT mousePos;
private:
  static GameController* instance;
  GameController()
  {
    type = GAME_EDITOR;
    mousePos = POINT();
  }
};