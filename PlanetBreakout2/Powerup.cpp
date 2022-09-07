#include "GameTypes.h"
#include "GameController.h"
#include "LogicHelper.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <random>


void Powerup::UpdateFrame(int64_t elapsed)
{
  if (!active) return;

  float frame_modifier = 1.0f;
  if (elapsed < TARGET_FRAMES) //slow down
    frame_modifier = elapsed / TARGET_FRAMES;
  float distance = frame_modifier * speed;
  float old_x = real_x;
  float old_y = real_y;
  real_x = real_x + cos(direction) * distance;
  real_y = real_y + sin(direction) * distance;

  float y_new_bottom = real_y + POWERUP_DIMENSION;
  float y_old_bottom = old_y + POWERUP_DIMENSION;
  bool old_above = y_old_bottom <= BAT_Y;
  bool new_below = y_new_bottom >= BAT_Y;

  if (real_y + POWERUP_DIMENSION > GAME_HEIGHT)
  {
    active = false;
  }
  else if (old_above && new_below)
  {
    float bat_x = GameController::GetInstance()->bat->x;
    //Add a little forgiveness to the bat dimension
    float x1 = bat_x - POWERUP_DIMENSION - 2.5f;
    float x2 = bat_x + BAT_WIDTH + 2.5f;
    if (real_x >= x1 && real_x <= x2)
    {
      srand(time(NULL));
      //GameController::GetInstance()->AddPowerup(
      //  (PowerupType)(rand() % PowerupType::POWERUP_COUNT));
      GameController::GetInstance()->AddPowerup();
      active = false;
    }
  }
  Update(real_x, real_y);
}

void Powerup::Start()
{
  speed = 1.f;
  real_x = x;
  real_y = y;
  direction = M_PI / 2.f;
}