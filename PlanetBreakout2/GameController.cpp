#include "GameController.h"
#include "LogicHelper.h"
#include <chrono>
#include <numeric>
#include <algorithm>
#include <random>
GameController* GameController::instance = nullptr;

GameController::GameController()
{
  srand(time(NULL));
  game_type = GameType::MAIN_MENU;
  old_type = game_type;
  mousePos = POINT();
  mousePosPrev = POINT();
  timer = std::chrono::microseconds(0);
  level_state = LevelState::START;
  planetEffect = new PlanetEffect(CLIENT_WIDTH / 2.f, CLIENT_HEIGHT / 2.f);

  menuStars.clear();
  for (int i = 0; i < 300; ++i)
  {
    menuStars.push_back(Star(CLIENT_WIDTH, CLIENT_HEIGHT));
  }
}

PlanetEffect* GameController::GetPlanetEffect()
{
  return planetEffect;
}

PrimitiveText& GameController::GetHighscoreText()
{
  return highscore_text;
}

LevelState GameController::GetLevelState()
{
  return level_state;
}

GameController* GameController::GetInstance()
{
  if (instance == nullptr)
    instance = new GameController();
  return instance;
}

void GameController::MouseUpdate(const POINT& mouse)
{
  mousePos = mouse;
}

POINT GameController::GetMousePos()
{
  return mousePos;
}

const std::vector<DynamicEffect*> GameController::GetEffects() const
{
  return effects;
}

GameType GameController::GetGameType()
{
  return game_type;
}
void GameController::SetGameType(GameType type)
{
  game_type = type;
}

void GameController::SetHighscoreName(std::wstring name)
{
  if (level_state != LevelState::HIGHSCORE)
    return;
  Highscore highscore;
  highscore.date = time(NULL);
  highscore.score = score;
  highscore.name = name;
  campaign.AddHighscore(highscore);

  //Needs to write back to gameloader...
  level_state = LevelState::GAME_OVER;
  game_type = GameType::MAIN_MENU;
}

const GamePowerUpMap& GameController::GetGamePowerUpMap() const
{
  return powerup_map;
}

std::wstring GameController::GetSpriteForEntity(DynamicSpriteType type)
{
  switch (type)
  {
  case DynamicSpriteType::BAT:
    if (IsPowerUpActive(PowerupType::LASER_BAT))
      return LASERBAT_SPRITE;
    return campaign.bat_sprite;
  case DynamicSpriteType::BALL:
    if (IsPowerUpActive(PowerupType::HYPER_BALL))
      return HYPERBALL_SPRITE;
    return campaign.ball_sprite;
  }
  return L"";//not reachable
}

bool GameController::IsPowerUpActive(PowerupType type)
{
  return powerup_map.at(type).IsActive();
}
void GameController::AddPowerup()
{
  std::vector<int> v_powerups(POWERUP_SIZE);
  std::iota(std::begin(v_powerups), std::end(v_powerups), 0);
  std::shuffle(std::begin(v_powerups), std::end(v_powerups), rng);
  for (int i : v_powerups)
  {
    PowerupType type = (PowerupType)i;
    if (!powerup_map[type].IsActive())
    {
      powerup_map[type].SetActive(true, timer);
      random_chance += 5;
      switch (type)
      {
      case EMP:
        ClearBrickShield();
        ClearBarrierBricks();
        break;
      case BARRIER:
      { //TODO ball in brick when spawning?
        for (int col = 0; col < GRID_COLUMNS; ++col)
        {
          Brick brick(BrickType::NORMAL_BRICK, BARRIER_SPRITE, col, GRID_ROWS - 1);
          uint32_t index;
          if (!GetBrickIndex(col, GRID_ROWS - 1, index))
            continue;
          bricks[index].push_back(brick);
        }
        if (IsPowerUpActive(PowerupType::EMP))
          ClearBrickShield();
      }
      break;
      case BRICK_SHIELD:
        for (uint32_t index : GetBrickMap().GetBrickCheckSet())
        {
          //Should be a useless check, but just to be sure...
          const BrickMap::const_iterator it = bricks.find(index);
          if (it == bricks.end() || it->second.empty())
            return;
          unsigned col = index % GRID_COLUMNS;
          unsigned row = index / GRID_COLUMNS;
          const Brick& last = *(--it->second.end());
          if (last.subtype != BrickType::NO_POINT)
          {
            Brick brick(BrickType::NO_POINT, NOPOINT_SPRITE, col, row);
            bricks[index].push_back(brick);
          }
          if (IsPowerUpActive(PowerupType::EMP))
            ClearBrickShield();
        }
        break;
      case STRIKE:
      {
        uint32_t max_index = GRID_COLUMNS * (GRID_ROWS - 6);
        std::set<uint32_t> strike;
        for (int i = 0; i < 50; ++i)
        {
          uint32_t index = rand() % max_index;
          strike.insert(index);
        }
        for (uint32_t index : strike)
        {
          RECT r = GetBrickRect(index);
          effects.push_back(new BrickBurnEffect(r.left, r.top));
          BreakBrick(nullptr, index);
        }
      }
      break;
      }
      return;
    }
  }
  //All powerups are active, just add score
  AddScore(100);
}
void GameController::ClearBarrierBricks()
{
  for (int col = 0; col < GRID_COLUMNS; ++col)
  {
    uint32_t index;
    if (!GetBrickIndex(col, GRID_ROWS - 1, index))
      continue;
    BrickMap::iterator it = bricks.find(index);
    if (it != bricks.end() && !it->second.empty())
    {
      effects.push_back(new SpinSquareEffect(
        GetBrickRectF(index),
        ColorBrush::BLUE));
      it->second.clear();
    }
  }
}
void GameController::ClearBrickShield()
{
  BrickMap::iterator it;
  for (it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->second.empty())
      continue;

    auto brick_it = (--it->second.end());
    if (brick_it->subtype == BrickType::NO_POINT)
    {
      it->second.erase(brick_it);
      effects.push_back(new SpinSquareEffect(
        GetBrickRectF(it->first),
        ColorBrush::BLUE));
    }
  }
}
void GameController::Respawn()
{
  GamePowerUpMap::iterator pwr_it = powerup_map.begin();
  for (; pwr_it != powerup_map.end(); ++pwr_it)
    pwr_it->second.SetActive(false, std::chrono::microseconds::zero());
  ClearBrickShield();
  ClearBarrierBricks();
  powerups.clear();
  droneLasers.clear();
  balls.clear();
  effects.clear();
  laser.SetActive(false);
  //destroyBat = false;
  //todo update
  random_chance = 20;
  bat->SetActive(true);
  if (lives == 0)
  {
    level_state = LevelState::GAME_OVER;
    if (!campaign.IsTestMode() && campaign.NewHighscore(score))
    {
      level_state = LevelState::HIGHSCORE;
    }
  }
  else
  {
    level_state = LevelState::START;
    timer = std::chrono::microseconds::zero();
    //timer_creator = std::chrono::microseconds::zero();
    Ball starter_ball(campaign.ball_sprite);
    starter_ball.Update(0, (GRID_ROWS - 4) * BRICK_HEIGHT);
    starter_ball.MoveCenterX(GAME_WIDTH / 2);
    starter_ball.Start();
    balls.push_back(starter_ball);
    bat->Update(0, BAT_Y);
    bat->MoveCenterX(GAME_WIDTH / 2);
  }
}

Bat* GameController::GetBat()
{
  return bat;
}

void GameController::Play()
{
  //Update all the timers
  if (level_state == LevelState::START ||
    level_state == LevelState::PAUSED)
  {

    std::chrono::microseconds now = std::chrono::duration_cast<std::chrono::microseconds>(
      std::chrono::system_clock::now().time_since_epoch());
    if (timer.count() == 0)
      timer = now;

    std::chrono::microseconds offset = (now - timer);
    GamePowerUpMap::iterator it = powerup_map.begin();
    for (; it != powerup_map.end(); ++it)
      it->second.ResumeTime(offset);
    timer = now;
    level_state = LevelState::ACTIVE;
  }
}

const Laser& GameController::GetLaser() const
{
  return laser;
}

void GameController::ShootLaser()
{
  if (laser.IsActive())
    return;
  if (!powerup_map.at(PowerupType::LASER_BAT).IsActive())
    return;
  int cx = bat->x + (BAT_WIDTH / 2) - (laser.width / 2);
  laser.SetPosition(cx, BAT_Y);
  laser.Start();
}

//Returns true if the specified ball should bounce
bool GameController::BreakBrick(DynamicCollider* ball, uint32_t index)
{
  bool hyper_ball = powerup_map.at(PowerupType::HYPER_BALL).IsActive();
  size_t erased = GetBrickMap().Erase(index,
    hyper_ball ? PB2_BRICKMAP_ERASE_ALL :
    PB2_BRICKMAP_ERASE_TOP);

  if (erased > 0)
  {
    unsigned col = index % GRID_COLUMNS;
    unsigned row = index / GRID_COLUMNS;
    effects.push_back(new SpinSquareEffect(
      GetBrickRectF(col, row),
      hyper_ball ?  ColorBrush::RED : ColorBrush::GREEN));

    if (ball != nullptr)
    {
      float multiplier = 10.f;
      if (powerup_map.at(PowerupType::BONUS_POINTS).IsActive())
        multiplier = 12.f;
      AddScore((uint16_t)(ball->GetSpeed() * multiplier * erased));

      if ((rand() % random_chance) == 0)
      {
        Powerup p;
        //Since ball width < powerup width
        unsigned x = ball->x;
        if (x + POWERUP_DIMENSION > GAME_WIDTH)
          x = GAME_WIDTH - POWERUP_DIMENSION;
        p.Update(ball->x, ball->y);

        bool spawn_new = true;
        for (Powerup& powerup : powerups)
        {
          RECT dummy;
          if (IntersectRect(&dummy, &(powerup.GetRect()), &(p.GetRect())) != 0)
          {
            powerup.AddCount();
            spawn_new = false;
            break;
          }
        }
        if (spawn_new)
        {
          p.Start();
          powerups.push_back(p);
        }
      }
    }

  }
  if (!hyper_ball) return true;
  //Cannot to fast check because of no-point bricks
  //if (erased == 0) return true; //fast check
  return bricks.find(index) != bricks.end(); //expensive check
}

const std::vector<Ball>& GameController::GetBalls() const
{
  return balls;
}

const std::vector<Powerup>& GameController::GetPowerups() const
{
  return powerups;
}

const std::vector<DroneLaser>& GameController::GetDroneLasers() const
{
  return droneLasers;
}

size_t GameController::GetCurrentLevel()
{
  return current_level;
}
uint32_t GameController::GetLives()
{
  return lives;
}

void GameController::Pause()
{
  if (game_type != GameType::GAME_NORMAL)
    return;
  if (level_state == LevelState::ACTIVE)
    level_state = LevelState::PAUSED;
}

void GameController::NextLevel()
{
  if (level_state != LevelState::END)
    return;
  if (current_level + 1 >= campaign.levels.size())
    current_level = 0;
  else
    ++current_level;
  bricks = campaign.levels.at(current_level).GetBrickMap();
  Respawn();
  level_state = LevelState::START;
}

void GameController::GameUpdate()
{

  if (level_state == LevelState::PAUSED)
    return;

  std::chrono::microseconds now = std::chrono::duration_cast<std::chrono::microseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  if (timer.count() == 0)
  {
    timer = now;
    return;
  }
  std::chrono::microseconds delta = now - timer;
  timer = now;

  if (game_type == GameType::MAIN_MENU)
  {
    for (Star& star : menuStars)
      star.UpdateFrame(delta.count());
    planetEffect->UpdateFrame(delta.count());
    return;
  }

  if (level_state == LevelState::PAUSED)
    return;
  if (level_state == LevelState::GAME_OVER)
    return;
  if (level_state == LevelState::END)
    return;
  if (level_state == LevelState::HIGHSCORE)
    return;
  if (bricks.Empty())
  {
    level_state = LevelState::END;
    return;
  }

  int halfWidth = bat->width / 2;
  int dx = mousePos.x - halfWidth;
  if (dx < 0)
    dx = 0;
  else if (dx + bat->width > GAME_WIDTH)
    dx = GAME_WIDTH - bat->width;
  bat->Update(dx, bat->y);

  for (Star& star : stars)
    star.UpdateFrame(delta.count());

  if (level_state == LevelState::START)
    return;

  //vector<Base*> v2(v1.begin(), v1.end());
  std::vector<DynamicEffect*>::iterator effects_it;
  for (effects_it = effects.begin(); effects_it != effects.end();)
  {
    DynamicEffect* effect = (*effects_it);
    if (!effect->IsActive())
    {
      effects_it = effects.erase(effects_it);
      delete effect;
    }
    else
    {
      effect->UpdateFrame(delta.count());
      ++effects_it;
    }
  }

  if (!bat->IsActive())
  {
    if (!effects.empty())
      return;
    if (!powerup_map.at(PowerupType::EXTRA_LIFE).IsActive())
    {
      lives--;
    }
    Respawn();
    return;
  }

  bool spawn_creator = false;
  bool spawn_portal = false;
  bool spawn_drone = false;
  GamePowerUpMap::iterator it = powerup_map.begin();
  for (; it != powerup_map.end(); ++it)
  {
    if (!it->second.IsActive())
      continue;
    switch (it->first)
    {
    case PowerupType::CREATOR_BALL:
      spawn_creator = it->second.ShouldTrigger(now);
      break;
    case PowerupType::PORTAL:
      spawn_portal = it->second.ShouldTrigger(now);
      break;
    case PowerupType::DRONE:
      spawn_drone = it->second.ShouldTrigger(now);
      break;
    }
  }

  if (laser.IsActive())
  {
    laser.UpdateFrame(delta.count());
  }

  bool any_ball_active = false;
  std::vector<Ball> new_balls;
  std::vector<Ball>::iterator ball_it;
  for (ball_it = balls.begin(); ball_it != balls.end();)
  {
    if (ball_it->IsActive())
    {
      any_ball_active = true;
      ball_it->UpdateFrame(delta.count());
      if (spawn_creator)
      {
        Ball starter_ball(campaign.ball_sprite);
        starter_ball.SetPosition(ball_it->GetRealX(), ball_it->GetRealY());
        new_balls.push_back(starter_ball);
        effects.push_back(new RingEffect(starter_ball.GetRealX(),
          starter_ball.GetRealY()));
      }
      if (spawn_portal)
      {
        effects.push_back(new RingEffect(ball_it->GetRealX(),
          ball_it->GetRealY()));
        ball_it->Reset();
        ball_it->Update(0, (GRID_ROWS - 4) * BRICK_HEIGHT);
        ball_it->MoveCenterX(GAME_WIDTH / 2);
        ball_it->Start();
      }
      if (spawn_drone)
      {
        DroneLaser dlaser;
        dlaser.SetPosition(ball_it->GetRealX(), ball_it->GetRealY());
        dlaser.Start();
        droneLasers.push_back(dlaser);
      }
      ++ball_it;
    }
    else
    {
      ball_it = balls.erase(ball_it);
    }
  }

  for (Ball& newball : new_balls)
  {
    newball.Start();
    balls.push_back(newball);
  }

  std::vector<Powerup>::iterator pwr_it;
  for (pwr_it = powerups.begin(); pwr_it != powerups.end();)
  {
    if (pwr_it->IsActive())
    {
      pwr_it->UpdateFrame(delta.count());
      ++pwr_it;
    }
    else
    {
      pwr_it = powerups.erase(pwr_it);
    }
  }

  std::vector<DroneLaser>::iterator drone_it;
  for (drone_it = droneLasers.begin(); drone_it != droneLasers.end();)
  {
    if (drone_it->IsActive())
    {
      drone_it->UpdateFrame(delta.count());
      ++drone_it;
    }
    else
    {
      drone_it = droneLasers.erase(drone_it);
    }
  }

  for (Powerup& powerup : powerups)
  {
    if (powerup.IsActive())
      powerup.UpdateFrame(delta.count());
  }

  if (!any_ball_active)
  {
    DestroyBat();
  }
}

void GameController::DestroyBat()
{
  if (bat == nullptr)
    return;
  bat->SetActive(false);
  for (int i = 0; i < 10; ++i)
  {
    SpinSquareEffect* effect = new SpinSquareEffect(
      bat->GetD2D1Rect(), ColorBrush::GREEN);
    effect->SetMaxUpdates(750);
    effects.push_back(effect);
  }
}

BrickMap& GameController::GetBrickMap()
{
  return bricks;
}

uint16_t GameController::GetScore()
{
  return score;
}

#undef max
void GameController::AddScore(uint16_t amount)
{
  if (score < std::numeric_limits<uint16_t>::max())
  {
    score += amount;
  }
}

const std::vector<Star>& GameController::GetStars() const
{
  if (game_type == GameType::MAIN_MENU)
    return menuStars;
  return stars;
}

void GameController::CreateGame(Campaign& campaign)
{
  this->campaign = campaign;
  bat = new Bat(campaign.bat_sprite);
  old_type = game_type;
  game_type = GameType::GAME_NORMAL;
  lives = 6;
  current_level = 0;
  score = 0;
  bricks = campaign.levels.at(current_level).GetBrickMap();
  stars.clear();
  for (int i = 0; i < 300; ++i)
  {
    stars.push_back(Star(CLIENT_WIDTH, CLIENT_HEIGHT));
  }
  Respawn();
}

void GameController::EndGame()
{
  if (game_type != GameType::GAME_NORMAL)
    return;
  if (bat != nullptr)
  {
    delete bat;
    bat = nullptr;
  }
  balls.clear();
  powerups.clear();
  droneLasers.clear();
  laser.SetActive(false);
  current_level = 0;
  game_type = old_type;
}