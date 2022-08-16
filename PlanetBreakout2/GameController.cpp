#include "GameController.h"
#include <chrono>
GameController* GameController::instance = nullptr;

GameController::GameController()
{
  //game_type = GameType::GAME_EDITOR;
  game_type = GameType::MAIN_MENU;
  mousePos = POINT();
  mousePosPrev = POINT();
  timer = std::chrono::milliseconds(0);
  level_state = LevelState::START;
 // Drawable button0draw(GAME_WIDTH - 4, CLIENT_WIDTH - GAME_WIDTH - 4, 100, 14);
  //Button* button0 = new Button(button0draw);
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

void GameController::Respawn()
{
  balls.clear();
  if (lives == 0)
  {
    level_state = LevelState::GAME_OVER;
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

void GameController::Pause()
{
  if (level_state == LevelState::ACTIVE)
    level_state = LevelState::PAUSED;
}

void GameController::NextLevel()
{
  if (level_state != LevelState::END)
    return;
  if (current_level + 1 >= campaign.levels.size())
    return;
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
  if (bricks.Empty())
  {
    if (current_level + 1 >= campaign.levels.size())
      level_state = LevelState::HIGHSCORE;
    else
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
  this->campaign = campaign;
  bat = new Bat(campaign.bat_sprite);
  game_type = GameType::GAME_NORMAL;
  lives = 6;
  current_level = 0;
  score = 0;
  bricks = campaign.levels.at(current_level).GetBrickMap();
  Respawn();
}

void GameController::EndGame()
{
  if (bat != nullptr)
  {
    delete bat;
    bat = nullptr;
  }
  balls.clear();
  current_level = 0;
  game_type = GameType::GAME_EDITOR;
}