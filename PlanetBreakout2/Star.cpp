#include "Star.h"

void Star::Start()
{
  int min_int = rand() % 50 + 1;
  int max_int = rand() % (100 - 50 + 1) + 50;
  int curr_int = rand() % (max_int - min_int + 1) + min_int;
  int rate_int = rand() % (1000 - 10 + 1) + 10;
  int dir_int = rand() % 2;
  int size_int = rand() % (3 - 1 + 1) + 1;
  UpdateSize(size_int, size_int);
  int x_int = rand() % (GAME_WIDTH - size_int);
  int y_int = rand() % (GAME_HEIGHT - size_int);
  SetPosition(x_int, y_int);

  minOpacity = float(min_int) * 0.01f;
  maxOpacity = float(max_int) * 0.01f;
  currentOpacity = float(curr_int) * 0.01f;
  opacityRate = float(rate_int) * 1e-6;
  starDirection = (StarDirection)dir_int;
  active = true;
}

void Star::PostFrameUpdate()
{
  if (starDirection == StarDirection::BRIGHTER)
  {
    currentOpacity += opacityRate;
    if (currentOpacity > maxOpacity)
    {
      currentOpacity = maxOpacity;
      starDirection = StarDirection::DARKER;
    }
  }
  else
  {
    currentOpacity -= opacityRate;
    if (currentOpacity < minOpacity)
    {
      currentOpacity = minOpacity;
      starDirection = StarDirection::BRIGHTER;
    }
  }
}

const float Star::GetOpacity() const
{
  return currentOpacity;
}
