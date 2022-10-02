#include "GameController.h"
#include "LogicHelper.h"
#include <chrono>
#include <numeric>
#include <algorithm>
#include <random>
GameController* GameController::instance = nullptr;

GameController::GameController()
{
  game_type = GameType::MAIN_MENU;
  old_type = game_type;
  mousePos = POINT();
  mousePosPrev = POINT();
  timer = std::chrono::microseconds(0);
  timer_creator = std::chrono::microseconds(0);
  level_state = LevelState::START;
  srand(time(NULL));
  menuStars.clear();
  for (int i = 0; i < 300; ++i)
  {
    menuStars.push_back(Star(CLIENT_WIDTH, CLIENT_HEIGHT));
  }
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
      return L"laserbat";
    return campaign.bat_sprite;
  case DynamicSpriteType::BALL:
    if (IsPowerUpActive(PowerupType::HYPER_BALL))
      return L"hyperball";
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
      powerup_map[type].SetActive(true);
      random_chance += 5;
      switch (type)
      {
      case BARRIER:
      { //TODO ball in brick when spawning?
        for (int col = 0; col < GRID_COLUMNS; ++col)
        {
          Brick brick(BrickType::NORMAL_BRICK, L"barrier", col, GRID_ROWS - 1);
          uint32_t index;
          if (!GetBrickIndex(col, GRID_ROWS - 1, index))
            continue;
          bricks[index].push_back(brick);
        }
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
          if(last.subtype != BrickType::NO_POINT)
          {
            Brick brick(BrickType::NO_POINT, L"nopoint", col, row);
            bricks[index].push_back(brick);
          }
        }
        break;
      }
      break;
    }
  }
}

void GameController::Respawn()
{
  GamePowerUpMap::iterator pwr_it = powerup_map.begin();
  for (; pwr_it != powerup_map.end(); ++pwr_it)
    pwr_it->second.SetActive(false);
  powerups.clear();
  balls.clear();
  effects.clear();
  laser.SetActive(false);
  random_chance = 20;

  //Clears barrier bricks
  for (int col = 0; col < GRID_COLUMNS; ++col)
  {
    uint32_t index;
    if (!GetBrickIndex(col, GRID_ROWS - 1, index))
      continue;
    bricks[index].clear();
  }

  //Remove brick shield
  BrickMap::iterator it;
  for (it = bricks.begin(); it != bricks.end(); ++it)
  {
    if (it->second.empty())
      continue;

    auto brick_it = (--it->second.end());
    if (brick_it->subtype == BrickType::NO_POINT)
      it->second.erase(brick_it);
  }

  if (lives == 0)
  {
    level_state = LevelState::GAME_OVER;
    if (campaign.NewHighscore(score))
    {
      level_state = LevelState::HIGHSCORE;
    }
  }
  else
  {
    level_state = LevelState::START;
    timer = std::chrono::microseconds::zero();
    timer_creator = std::chrono::microseconds::zero();
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
  if (level_state == LevelState::START ||
      level_state == LevelState::PAUSED)
  {
    //redundant, but just to make sure
    timer = std::chrono::milliseconds::zero();
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
  float multiplier = 10.f;
  if (powerup_map.at(PowerupType::BONUS_POINTS).IsActive())
    multiplier = 12.f;
  size_t erased = GetBrickMap().Erase(index,
    hyper_ball ? PB2_BRICKMAP_ERASE_ALL :
    PB2_BRICKMAP_ERASE_TOP);
  
  if (erased > 0)
  {
    GameController::GetInstance()->AddScore(
      (uint16_t)(ball->GetSpeed() * multiplier * erased));
    RECT r = GetBrickRect(index);
    effects.push_back(new RingEffect(
      r.left + (BRICK_WIDTH / 2),
      r.top + (BRICK_HEIGHT / 2)));
    if ((rand() % random_chance) == 0)
    {
      Powerup p;
      //Since ball width < powerup width
      unsigned x = ball->x;
      if (x + POWERUP_DIMENSION > GAME_WIDTH)
        x = GAME_WIDTH - POWERUP_DIMENSION;
      p.Update(ball->x, ball->y);
      p.Start();
      powerups.push_back(p);
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

  bool spawn_creator = false;
  if (IsPowerUpActive(PowerupType::CREATOR_BALL))
  {
    if (timer_creator.count() == 0)
    {
      timer_creator = now;
    }
    std::chrono::microseconds delta = now - timer_creator;
    if (delta.count() > 5000 * 1000) // 5 seconds
    {
      timer_creator = now;
      spawn_creator = true;
    }
  }

  if (laser.IsActive())
  {
    laser.UpdateFrame(delta.count());
  }

  std::vector<DynamicEffect*>::iterator effects_it;
  for (effects_it = effects.begin(); effects_it != effects.end();)
  {
    if (!(*effects_it)->IsActive())
    {
      effects_it = effects.erase(effects_it);
    }
    else
    {
      (*effects_it)->UpdateFrame(delta.count());
      ++effects_it;
    }
  }


  bool any_ball_active = false;
  std::vector<Ball> new_balls;
  for (Ball& ball : balls)
  {
    if (ball.IsActive())
    {
      any_ball_active = true;
      ball.UpdateFrame(delta.count());
      if (spawn_creator)
      {
        Ball starter_ball(campaign.ball_sprite);
        starter_ball.SetPosition(ball.GetRealX(), ball.GetRealY());
        new_balls.push_back(starter_ball);
      }
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

  for (Powerup& powerup : powerups)
  {
    if (powerup.IsActive())
      powerup.UpdateFrame(delta.count());
  }

  if (!any_ball_active)
  {
    if (!powerup_map.at(PowerupType::EXTRA_LIFE).IsActive())
    {
      lives--;
    }
    Respawn();
    return;
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
  for (int i = 0; i < 200; ++i)
  {
    stars.push_back(Star(GAME_WIDTH, GAME_HEIGHT));
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
  laser.SetActive(false);
  current_level = 0;
  game_type = old_type;
}