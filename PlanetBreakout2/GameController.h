#pragma once
#include <string>
#include "GameLevel.h"
enum GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR};
struct GameController
{
  static GameController* GetInstance();
  static void RemoveIllegalBricks(GameLevel&);
  static void AddGameBricks(GameLevel&);
  GameType type;
  std::wstring map_filename;
  GameLevel current_level;
  uint64_t points;
  uint64_t lives;
  POINT mousePos;
private:
  static GameController* instance;
  GameController()
  {
    type = GAME_EDITOR;
  }
};