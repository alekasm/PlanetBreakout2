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

  for (Ball& ball : balls)
  {
    ball.UpdateFrame(elapsed_frames, now.count());
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

void GameController::CreateGame(Campaign& campaign)
{
  this->campaign = campaign;
  bat = new Bat(campaign.bat_sprite);
  Ball starter_ball(campaign.ball_sprite);
  timer = std::chrono::milliseconds::zero();
  starter_ball.Update(0, (GRID_ROWS - 4) * BRICK_HEIGHT);
  starter_ball.MoveCenterX(GAME_WIDTH / 2);
  starter_ball.Start();
  balls.push_back(starter_ball);
  //ball->MoveCenterX(GAME_WIDTH / 2);
  bat->Update(0, (GRID_ROWS - 2) * BRICK_HEIGHT);
  bat->MoveCenterX(GAME_WIDTH / 2);
  type = GAME_NORMAL;
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