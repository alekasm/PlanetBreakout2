#pragma once
#include <unordered_map>
#include "Constants.h"
#include "Button.h"
#include "ResourceLoader.h"
#include "Client.h"
#include "Star.h"

enum class MainMenuState {MAIN, INFO, HIGHSCORE, CAMPAIGN_SELECT};
struct MainMenu
{
  void initialize(Client*);
  //std::vector<Button*>& GetButtons(MainMenuState);
  std::vector<Button*>& GetButtons();
  MainMenuState GetState();
  uint32_t GetCampaignPage();
  void RefreshFullscreenButton(Client* client);
  void RefreshAudioButton();
  const std::wstring& GetDescription(PowerupType) const;
  const std::wstring& GetVersion() const;
  PowerupType GetPowerUpSelection();
  const std::wstring& GetCurrentCampaignNameSelection() const;
  const std::wstring& GetCurrentDifficultySelection() const;
  void SwitchState(MainMenuState);
private:
  std::unordered_map<MainMenuState, std::vector<Button*>> buttons;
  MainMenuState state = MainMenuState::MAIN;
  size_t campaign_page = 0;
  size_t difficulty_page = 0;
  ID2D1LinearGradientBrush* planetBrush = nullptr;
  Button* fullscreenButton;
  Button* audioButton;
  std::unordered_map<PowerupType, std::wstring> powerupDescriptions;
  PowerupType currentPowerupSelected;
  std::wstring currentDifficultySelected;
  std::wstring currentCampaignNameSelected;
  std::vector<Button*> powerupButtons;
  const std::wstring versionString =
    L"Version " + std::to_wstring(VERSION);

};