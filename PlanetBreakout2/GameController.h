#pragma once
#include <string>
#include <chrono>
#include "GameLevel.h"
enum class GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR, MAIN_MENU};
enum class LevelState {START, ACTIVE, PAUSED, END, GAME_OVER, HIGHSCORE};

//Campaigns dictate the default bat/ball sprites

struct GameController
{
  static GameController* GetInstance();
  size_t current_level = 0;
  uint32_t lives = 0;
  POINT mousePos;
  Bat* bat = nullptr;
  std::vector<Ball> balls;
  uint16_t GetScore();
  void AddScore(uint16_t);
  Campaign campaign;
  void CreateGame(Campaign&);
  void EndGame();
  void MouseUpdate(const POINT&);
  void GameUpdate();
  void Play();
  void Pause();
  void NextLevel();
  void SetHighscoreName(std::wstring&);
  BrickMap& GetBrickMap();
  LevelState GetLevelState();
  GameType GetGameType();
  void SetGameType(GameType);
  Bat* GetBat();
private:
  LevelState level_state;
  GameType game_type;
  void Respawn();
  void UpdateBalls();
  float game_speed = 5.f;
  uint16_t score = 0;
  std::chrono::microseconds timer;
  static GameController* instance;
  POINT mousePosPrev;
  BrickMap bricks;
  GameType old_type = GameType::MAIN_MENU;
  GameController();
};