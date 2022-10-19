#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include "GameTypes.h"
#include "GameLevel.h"

typedef std::unordered_map<std::wstring, Campaign> CampaignMap;

struct GameLoader
{
  static bool LoadMap(const std::wstring& filename, GameLevel& level);
  static bool LoadAssets(const std::wstring& filename);
  static bool LoadCampaigns();
  static bool SaveMap(GameLevel& level, std::wstring& save_out);
  static bool SaveCampaign(Campaign&);
  static std::vector<Brick>& GetAssetBricks();
  static CampaignMap& GetCampaigns();
private:
  static std::vector<Brick> assetBricks;
  static CampaignMap campaignMap;
  static HCURSOR cursor;
};