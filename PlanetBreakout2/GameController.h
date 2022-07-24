#pragma once
#include <string>
#include <chrono>
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
  std::vector<Ball> balls;
  Campaign campaign;
  void CreateGame(Campaign&);
  void EndGame();
  void MouseUpdate(const POINT& mouse);
  void GameUpdate();

private:
  void UpdateBalls();
  float game_speed = 5.f;
  std::chrono::milliseconds timer;
  static GameController* instance;
  POINT mousePosPrev;
  GameController()
  {
    type = GAME_EDITOR;
    mousePos = POINT();
    mousePosPrev = POINT();
  }
};