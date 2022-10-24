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
#include "Star.h"
#include "DynamicEffect.h"
#include "DroneLaser.h"

enum class GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR, MAIN_MENU};
enum class LevelState {START, ACTIVE, PAUSED, END, GAME_OVER, HIGHSCORE};
enum class DynamicSpriteType { BAT, BALL};

struct GamePowerUp
{
  //The tile is 32x32, expected icon size is 16x16
  GamePowerUp(std::wstring icon, uint32_t timeMS = 0)
  {
    this->icon = icon;
    this->timeMS = timeMS * 1000;
    timer = std::chrono::microseconds::zero();
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
  void SetActive(bool value, std::chrono::microseconds current)
  {
    active = value;
    timer = current;
  }
  void ResumeTime(std::chrono::microseconds offset)
  {
    timer += offset;
  }
  const bool HasTime() const
  {
    return timeMS > 0;
  }
  const float GetPercentRemaining() const
  {
    return percent;
  }
  bool ShouldTrigger(std::chrono::microseconds current)
  {
    std::chrono::microseconds delta = (current - timer);
    bool shouldTrigger = active &&
      (time == 0 || delta.count() > timeMS);
    if (shouldTrigger)
      timer = current;
    else
      percent = (float)delta.count() / timeMS;
    return shouldTrigger;
  }
private:
  bool active = false;
  std::wstring icon;
  std::chrono::microseconds timer;
  uint32_t timeMS = 0;
  float percent = 0.f;
};

typedef std::unordered_map<PowerupType, GamePowerUp> GamePowerUpMap;

struct GameController
{
  bool BreakBrick(DynamicCollider*, uint32_t);
  static GameController* GetInstance(); 
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
  const std::vector<DroneLaser>& GetDroneLasers() const;
  size_t GetCurrentLevel();
  uint32_t GetLives();
  void ShootLaser();
  const Laser& GetLaser() const;
  const std::vector<Star>& GetStars() const;
  const std::vector<DynamicEffect*> GetEffects() const;
  PlanetEffect* GetPlanetEffect();
  void DestroyBat();
  POINT GetMousePos();
private:
  POINT mousePos;
  Bat* bat = nullptr;
  void ClearBarrierBricks();
  void ClearBrickShield();
  Laser laser;
  size_t current_level = 0;
  uint32_t lives = 0;
  PlanetEffect* planetEffect;
  std::vector<Ball> balls;
  std::vector<Powerup> powerups;
  std::vector<Star> stars;
  std::vector<Star> menuStars;
  std::vector<DroneLaser> droneLasers;
  std::vector<DynamicEffect*> effects;
  PrimitiveText highscore_text;
  LevelState level_state;
  GameType game_type;
  void Respawn();
  //void UpdateBalls();
  float game_speed = 5.f;
  uint16_t score = 0;
  size_t random_chance = 20;
  std::chrono::microseconds timer;
  static GameController* instance;
  POINT mousePosPrev;
  BrickMap bricks;
  GameType old_type = GameType::MAIN_MENU;
  //bool destroyBat = false;
  std::default_random_engine rng{ std::random_device{}() };
  GamePowerUpMap powerup_map = {
    {PowerupType::HYPER_BALL, GamePowerUp(L"hyperball")},
    {PowerupType::CREATOR_BALL, GamePowerUp(L"creator", 5000)},
    {PowerupType::LASER_BAT, GamePowerUp(L"laser")},
    {PowerupType::BONUS_POINTS, GamePowerUp(L"points")},
    {PowerupType::BARRIER, GamePowerUp(L"barrier_icon")},
    {PowerupType::EXTRA_LIFE, GamePowerUp(L"heart")},
    {PowerupType::GHOST, GamePowerUp(L"ghost")},
    {PowerupType::BRICK_SHIELD, GamePowerUp(L"shield")},
    {PowerupType::PORTAL, GamePowerUp(L"portal", 10000)},
    {PowerupType::DRONE, GamePowerUp(L"drone", 3000)},
    {PowerupType::STRIKE, GamePowerUp(L"strike")},
    {PowerupType::EMP, GamePowerUp(L"emp")},
  };
  //std::chrono::microseconds timer_creator;
  //std::chrono::microseconds timer_portal;
  //const uint32_t time_creator_us = 5000 * 1000;
  //const uint32_t time_portal_us = 10000 * 1000;
  POINTF menuLightOrigin;
  POINTF menuLightEnd;
  GameController();
};