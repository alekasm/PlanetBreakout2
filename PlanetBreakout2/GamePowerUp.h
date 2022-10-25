#pragma once
#include <string>
#include <chrono>

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
    percent = 0.f;
  }
  void ResumeTime(std::chrono::microseconds offset)
  {
    timer += offset;
  }
  const bool HasTime() const
  {
    return timeMS > 0;
  }
  #
  const float GetPercentRemaining() const
  {
    return std::clamp(percent, 0.f, 1.f);
  }
  bool ShouldTrigger(std::chrono::microseconds current)
  {
    std::chrono::microseconds delta = (current - timer);
    bool shouldTrigger = active &&
      (timeMS == 0 || delta.count() > timeMS);
    if (shouldTrigger)
      timer = current;
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
#pragma once
