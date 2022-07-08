#pragma once
#include <string>
#include "GameLevel.h"
enum GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR};

//Campaigns dictate the default bat/ball sprites
struct Campaign
{
  std::wstring bat_sprite = L"bat";
  std::wstring ball_sprite = L"ball";
  std::vector<GameLevel> levels;
};
struct GameController
{
  static GameController* GetInstance();
  GameType type;
  size_t current_level = 0;
  uint64_t points = 0;
  uint64_t lives = 0;
  POINT mousePos;
  Bat* bat = nullptr;
  Ball* ball = nullptr;
  Campaign campaign;
  void CreateGame(Campaign&);
  void EndGame();
private:
  static GameController* instance;
  GameController()
  {
    type = GAME_EDITOR;
    mousePos = POINT();
  }
};