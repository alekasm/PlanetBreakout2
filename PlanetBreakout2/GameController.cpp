#include "GameController.h"
#include <chrono>
GameController* GameController::instance = nullptr;

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

void GameController::Respawn()
{
  balls.clear();
  if (lives == 0)
  {
    //Game Over
  }
  else
  {
    lives--;
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

void GameController::GameUpdate()
{
  std::chrono::milliseconds now = std::chrono::duration_cast<std::chrono::milliseconds>(
    std::chrono::system_clock::now().time_since_epoch());
  if (timer.count() == 0)
  {
    timer = now;
    return;
  }
  std::chrono::milliseconds delta = now - timer;
  timer = now;

  float target_fps = (1000.f / TARGET_FPS);
  float elapsed_frames = delta.count() / target_fps;

  bool any_ball_active = false;
  for (Ball& ball : balls)
  {
    if (ball.IsActive())
    {
      any_ball_active = true;
      ball.UpdateFrame(elapsed_frames, now.count());
    }
  }

  if (!any_ball_active)
  {
    Respawn();
    return;
  }

  int halfWidth = bat->width / 2;
  int dx = mousePos.x - halfWidth;
  if (dx < 0)
    dx = 0;
  else if (dx + bat->width > GAME_WIDTH)
    dx = GAME_WIDTH - bat->width;
  bat->Update(dx, bat->y);
  //UpdateBalls();
}

BrickMap& GameController::GetBrickMap()
{
  return bricks;
}

void GameController::CreateGame(Campaign& campaign)
{
  this->campaign = campaign;
  bat = new Bat(campaign.bat_sprite);
  type = GAME_NORMAL;
  lives = 3;
  current_level = 0;
  bricks = campaign.levels.at(current_level).brickMap;
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
  type = GAME_EDITOR;
}