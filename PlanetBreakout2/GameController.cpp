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
  timer = std::chrono::milliseconds(0);
  level_state = LevelState::START;
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
      switch (type)
      {
      case HYPER_BALL:
      {
        for (Ball& b : balls)
          b.sprite = L"hyperball";
      }
      break;
      case BARRIER:
      { //TODO ball in brick when spawning?
        for (int col = 0; col < GRID_COLUMNS; ++col)
        {
          Brick brick(BrickType::NORMAL_BRICK, L"barrier", col, GRID_ROWS - 1);
          bricks[GetBrickIndex(col, GRID_ROWS - 1)].push_back(brick);
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
  //Clears barrier bricks
  for (int col = 0; col < GRID_COLUMNS; ++col)
  {
    bricks[GetBrickIndex(col, GRID_ROWS - 1)].clear();
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
    Ball starter_ball(campaign.ball_sprite);
    timer = std::chrono::milliseconds::zero();
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

//Returns true if the specified ball should bounce
bool GameController::BreakBrick(Ball* ball, uint32_t index)
{
  bool hyper_ball = powerup_map.at(PowerupType::HYPER_BALL).IsActive();
  size_t erased = GetBrickMap().Erase(index,
    hyper_ball ? PB2_BRICKMAP_ERASE_ALL :
    PB2_BRICKMAP_ERASE_TOP);
  GameController::GetInstance()->AddScore(
    (uint16_t)(ball->GetSpeed() * 10 * erased));
  if (erased > 0)
  {
    //todo randomize, this is for testing purposes
    if (!hyper_ball && powerups.empty())
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
  return !hyper_ball && erased > 0;
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

  if (level_state == LevelState::START)
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

  bool any_ball_active = false;
  for (Ball& ball : balls)
  {
    if (ball.IsActive())
    {
      any_ball_active = true;
      ball.UpdateFrame(delta.count());
    }
  }

  for (Powerup& powerup : powerups)
  {
    if (powerup.IsActive())
      powerup.UpdateFrame(delta.count());
  }

  if (!any_ball_active)
  {
    lives--;
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


void GameController::CreateGame(Campaign& campaign)
{
  srand(time(NULL));
  this->campaign = campaign;
  bat = new Bat(campaign.bat_sprite);
  old_type = game_type;
  game_type = GameType::GAME_NORMAL;
  lives = 6;
  current_level = 0;
  score = 0;
  bricks = campaign.levels.at(current_level).GetBrickMap();
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
  current_level = 0;
  game_type = old_type;
}