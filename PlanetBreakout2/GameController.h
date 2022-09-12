#pragma once
#include <string>
#include <chrono>
#include <unordered_map>
#include  <random>
#include "GameLevel.h"
#include "PrimitiveText.h"
#include "Ball.h"
#include "Powerup.h"
#include "Laser.h"

enum class GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR, MAIN_MENU};
enum class LevelState {START, ACTIVE, PAUSED, END, GAME_OVER, HIGHSCORE};
enum class DynamicSpriteType { BAT, BALL};

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
  bool BreakBrick(DynamicEntity*, uint32_t);
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
  void AddPowerup();
  std::wstring GetSpriteForEntity(DynamicSpriteType);
  const GamePowerUpMap& GetGamePowerUpMap() const;
  bool IsPowerUpActive(PowerupType);
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
  void ShootLaser();
  const Laser& GetLaser() const;
private:
  Laser laser;
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
  std::default_random_engine rng{ std::random_device{}() };
  GamePowerUpMap powerup_map = {
    {PowerupType::HYPER_BALL, GamePowerUp(L"hyperball")},
    {PowerupType::CREATOR_BALL, GamePowerUp(L"creator")},
    {PowerupType::LASER_BAT, GamePowerUp(L"laser")},
    {PowerupType::BONUS_POINTS, GamePowerUp(L"points")},
    {PowerupType::BARRIER, GamePowerUp(L"barrier_icon")},
    {PowerupType::EXTRA_LIFE, GamePowerUp(L"heart")},
  };
  std::chrono::microseconds timer_creator;
  GameController();
};