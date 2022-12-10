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
#include "GamePowerUp.h"

enum class GameType {GAME_NORMAL, GAME_CUSTOM, GAME_EDITOR, MAIN_MENU};
enum class LevelState {START, ACTIVE, PAUSED, ENDING, END, GAME_OVER, HIGHSCORE};
enum class DynamicSpriteType { BAT, BALL};
typedef std::unordered_map<PowerupType, GamePowerUp> GamePowerUpMap;
struct Difficulty
{
  Difficulty(float maxSpeed,
    float incSpeed,
    float startSpeed,
    std::wstring name) :
    maxSpeed(maxSpeed), incSpeed(incSpeed),
    startSpeed(startSpeed), name(name)
  {
  }
  const float maxSpeed;
  const float incSpeed;
  const float startSpeed;
  const std::wstring name;
};
typedef std::unordered_map<DifficultyType, Difficulty> DifficultyMap;

struct GameController
{
  bool BreakBrick(DynamicCollider*, uint32_t);
  static GameController* GetInstance(); 
  uint16_t GetScore();
  void AddScore(uint16_t);
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
  void SetDifficulty(DifficultyType);
  const Difficulty& GetDifficulty();
  const DifficultyType GetDifficultyType() const;
  const DifficultyMap& GetDifficultyMap() const;
  const Campaign& GetCampaign() const;
private:
  Campaign* campaign;
  POINT mousePos;
  Bat* bat = nullptr;
  void ClearBarrierBricks();
  void ClearBrickShield();
  void CalculateBonus();
  Laser laser;
  size_t current_level = 0;
  uint32_t lives = 0;
  PlanetEffect* planetEffect;
  std::vector<Ball> balls;
  std::vector<Powerup> powerups;
  std::vector<Star> stars;
  std::vector<DroneLaser> droneLasers;
  std::vector<DynamicEffect*> effects;
  PrimitiveText highscore_text;
  LevelState level_state;
  GameType game_type;
  void Respawn();
  float game_speed = 5.f;
  uint16_t score = 0;
  uint16_t levelEndBonusCount = 0;
  size_t random_chance = 20;
  std::chrono::microseconds timer;
  static GameController* instance;
  POINT mousePosPrev;
  BrickMap bricks;
  GameType old_type = GameType::MAIN_MENU;
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

  DifficultyMap difficultyMap = {
    {DifficultyType::EASY, Difficulty(5.5f, 0.02f, 2.f, L"Space Cadet")},
    {DifficultyType::MEDIUM, Difficulty(8.f, 0.05f, 3.5f, L"Lunar Captain")},
    {DifficultyType::HARD, Difficulty(10.f, 0.1f, 4.5f, L"Galaxy Commander")},
  };
  DifficultyType currentDifficulty = DifficultyType::EASY;
  GameController();
};