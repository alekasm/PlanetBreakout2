#pragma once
#include <string>
#include <chrono>
#include <unordered_map>
#include "GameLevel.h"
#include "PrimitiveText.h"
#include "Ball.h"
enum class GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR, MAIN_MENU};
enum class LevelState {START, ACTIVE, PAUSED, END, GAME_OVER, HIGHSCORE};


struct GlobalPowerup
{
  bool laser_bat = false;
  bool bonus_multiplier = false;
};

struct BallPowerup
{
  bool creator_ball = false;
  bool hyper_ball = false;
};

typedef std::unordered_map<Ball*, BallPowerup> BallPowerupMap;

struct GameController
{
  bool BreakBrick(Ball*, uint32_t);
  static GameController* GetInstance();
  POINT mousePos;
  Bat* bat = nullptr;
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
  void SetHighscoreName(std::wstring);
  BrickMap& GetBrickMap();
  LevelState GetLevelState();
  GameType GetGameType();
  void SetGameType(GameType);
  Bat* GetBat();
  PrimitiveText& GetHighscoreText();// TODO
  const std::vector<Ball>& GetBalls() const;
  size_t GetCurrentLevel();
  uint32_t GetLives();
private:
  size_t current_level = 0;
  uint32_t lives = 0;
  std::vector<Ball> balls;
  PrimitiveText highscore_text;
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
  BallPowerupMap ball_powerups;
  GameController();
};