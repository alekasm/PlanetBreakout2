#pragma once
#include <string>
#include <chrono>
#include <unordered_map>
#include "GameLevel.h"
#include "PrimitiveText.h"
#include "Ball.h"
#include "Powerup.h"

enum class GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR, MAIN_MENU};
enum class LevelState {START, ACTIVE, PAUSED, END, GAME_OVER, HIGHSCORE};

struct GamePowerUp
{
  //The tile is 32x32, expected icon size is 16x16
  GamePowerUp(std::wstring icon)
  {
    this->icon = icon;
  }
  GamePowerUp() = default;
  const bool IsActive() const
  {
    return active;
  }
  const std::wstring& GetIcon() const
  {
    return icon;
  }
  void SetActive(bool value)
  {
    active = value;
  }
private:
  ID2D1Bitmap* bitmap_active;
  bool active = false;
  std::wstring icon;
};

typedef std::unordered_map<PowerupType, GamePowerUp> GamePowerUpMap;

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
  void AddPowerup(PowerupType);
  const GamePowerUpMap& GetGamePowerUpMap() const;
  BrickMap& GetBrickMap();
  LevelState GetLevelState();
  GameType GetGameType();
  void SetGameType(GameType);
  Bat* GetBat();
  PrimitiveText& GetHighscoreText();// TODO
  const std::vector<Ball>& GetBalls() const;
  const std::vector<Powerup>& GetPowerups() const;
  size_t GetCurrentLevel();
  uint32_t GetLives();
private:
  size_t current_level = 0;
  uint32_t lives = 0;
  std::vector<Ball> balls;
  std::vector<Powerup> powerups;
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
  GamePowerUpMap powerup_map = {
    {PowerupType::HYPER_BALL, GamePowerUp(L"hyperball")},
    {PowerupType::CREATOR_BALL, GamePowerUp(L"")},
    {PowerupType::LASER_BAT, GamePowerUp(L"")},
    {PowerupType::BONUS_POINTS, GamePowerUp(L"")},
  };
  GameController();
};