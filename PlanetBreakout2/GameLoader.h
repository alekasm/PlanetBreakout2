#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <string>
#include "GameTypes.h"
#include "GameLevel.h"


//typedef std::unordered_map<std::wstring, GameLevel> LevelMap;
//typedef std::unordered_map<std::wstring, LevelMap&> CampaignMap;

struct GameLoader
{
  static bool LoadMap(const std::wstring& filename, GameLevel& level);
  static bool LoadAssets(const std::wstring& filename);
  //static bool LoadCampaign(const std::wstring& filename);
  static bool LoadCampaigns();
  //static bool GetLevel(const std::wstring& key, GameLevel& level);
  static bool SaveMap(GameLevel& level, std::wstring& save_out);
  static std::vector<Brick>& GetAssetBricks();
  static std::vector<Campaign>& GetCampaigns();
private:
  //static LevelMap level_map;
  //static CampaignMap campaign_map;
  static std::vector<Brick> assetBricks;
  static std::vector<Campaign> campaigns;
  static HCURSOR cursor;
};