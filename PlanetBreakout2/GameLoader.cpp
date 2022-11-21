#include "GameLoader.h"
#include <vector>
#include <fstream>
#include <conio.h>
#include <filesystem>
#include "Constants.h"
#include "ResourceLoader.h"
#include "Client.h"
#include "LogicHelper.h"

std::vector<Brick> GameLoader::assetBricks;
CampaignMap GameLoader::campaignMap;
HCURSOR GameLoader::cursor;

std::vector<Brick>& GameLoader::GetAssetBricks()
{
  return assetBricks;
}

CampaignMap& GameLoader::GetCampaigns()
{
  return campaignMap;
}

//Used for parsing bricks from map files and for asset maps
//type,sprite,x,y
bool ParseBrick(std::wstring& line, bool hasPosition, std::vector<Brick>& out)
{
  const std::wstring original(line);
  BrickType subtype;
  uint32_t x = 0;
  uint32_t y = 0;
  std::wstring sprite;
  const unsigned fields = hasPosition ? 4 : 2;
  for (unsigned i = 0; i < fields; ++i)
  {
    std::wstring token = line;
    std::wstring::size_type it = line.find(',');
    if (i < fields - 1 && it == std::wstring::npos)
    {
      printf("Missing fields for brick: %ls\n", original.c_str());
      return false;
    }
    token = line.substr(0, it);
    line.erase(0, it + 1);

    if (token.empty())
    {
      printf("Field is empty for brick: %ls\n", original.c_str());
      return false;
    }

    if (i == 0)
    {
      subtype = (BrickType)std::stoul(token);
      if (!ISVALID_BRICKTYPE(subtype))
      {
        printf("Invalid brick type (%u): %ls\n", subtype, original.c_str());
        return false;
      }
    }
    else if (i == 1)
    {
      sprite = token;
      if (!ResourceLoader::ContainsSprite(sprite))
      {
        printf("Invalid brick sprite (%ls): %ls\n", sprite.c_str(), original.c_str());
        return false;
      }
    }
    else if (i == 2)
    {
      x = std::stoul(token);
      if (x > GRID_COLUMNS)
      {
        printf("Invalid brick x (%u): %ls\n", x, original.c_str());
        return false;
      }
    }
    else if (i == 3)
    {
      y = std::stoul(token);
      if (y > GRID_ROWS)
      {
        printf("Invalid brick y (%u): %ls\n", y, original.c_str());
        return false;
      }
    }
  }
  out.push_back(Brick(subtype, sprite, x, y));
  return true;
}

enum class FileType { MAP, ASSET };

struct FileInfo
{
  FileType type;
  std::vector<Brick> bricks;
  std::wstring author;
};

static bool LoadFile(const std::wstring& filename, FileInfo& info)
{
  std::wifstream file_stream(filename.c_str());
  if (!file_stream.good())
  {
    printf("Invalid File: %ls\n", filename.c_str());
    return false;
  }
  const bool parseBrick = info.type == FileType::MAP;
  std::wstring line;
  while (std::getline(file_stream, line))
  {
    if (line.empty()) continue;
    if (line.at(0) == L'#') continue;

    //Expecting a delimeter
    std::wstring::size_type it = line.find(':');
    if (it == std::wstring::npos) continue;

    std::wstring token = line.substr(0, it);
    line.erase(0, it + 1);

    if (token == L"brick")
    {
      if (!ParseBrick(line, parseBrick, info.bricks))
      {
        file_stream.close();
        return false;
      }
    }
    else if (token == L"author")
    {
      info.author = line;
    }
  }
  file_stream.close();
  return true;
}

struct Token
{
  std::wstring key;
  std::vector<std::wstring> values;
};

bool Tokenize(std::wstring& line, Token& token)
{
  std::wstring::size_type it = line.find(':');
  if (it == std::wstring::npos)
    return false;

  token.key = line.substr(0, it);
  line.erase(0, it + 1);

  while((it = line.find(',')) != std::wstring::npos)
  {
    token.values.push_back(line.substr(0, it));
    line.erase(0, it + 1);
  }
  if (!line.empty())
    token.values.push_back(line);

  return true;

}

bool ReadFile(const std::wstring& filename, std::vector<std::wstring>& tokens)
{
  std::wifstream file_stream(filename.c_str());
  if (!file_stream.good())
  {
    printf("Invalid File: %ls\n", filename.c_str());
    return false;
  }
  std::wstring line;
  while (std::getline(file_stream, line))
  {
    if (line.empty()) continue;
    if (line.at(0) == L'#') continue;
    tokens.push_back(line);
  }
  return true;
}

bool GameLoader::SaveCampaign(Campaign& campaign)
{
  std::filesystem::path fs_path(campaign.path);
  fs_path.append(L"campaign.cfg");
  std::wofstream output(fs_path.wstring());
  if (!output.is_open())
  {
    wprintf(L"Failed to save file (%d): %ls\n", GetLastError(),
      fs_path.wstring().c_str());
    return false;
  }
  output << L"name:" << campaign.name << L"\n";
  output << L"bat:" << campaign.bat_sprite << L"\n";
  output << L"ball:" << campaign.ball_sprite << L"\n";
  HighscoreMap::const_iterator h_it;
  for (h_it = campaign.GetHighscores().begin();
    h_it != campaign.GetHighscores().end(); ++h_it)
  {
    for (const Highscore& h : h_it->second)
    {
      if (h.pseudo) continue;
      output << L"highscore:";
      output << h.name << L",";
      output << std::to_wstring(h.score) << L",";
      output << std::to_wstring(h.difficulty) << L",";
      output << std::to_wstring(h.date) << L",";
      output << std::to_wstring(h.CalculateChecksum()) << L"\n";
    }
  }
  output.close();
  CampaignMap::iterator it = campaignMap.find(campaign.name);
  if (it != campaignMap.end())
  {
    it->second = campaign;
  }
  return true;
}

bool ReadCampaignConfig(std::wstring filename, Campaign& campaign)
{
  std::vector<std::wstring> tokens;
  if (!ReadFile(filename, tokens))
  {
    wprintf(L"Failed to read config: %s\n", filename.c_str());
    return false;
  }
  for (std::wstring string : tokens)
  {
    Token t;
    if (!Tokenize(string, t)) continue;
    if (t.values.empty()) continue;
    std::wstring first_value = t.values.at(0);
    if (t.key == L"ball")
      campaign.ball_sprite = first_value;
    else if (t.key == L"bat")
      campaign.bat_sprite = first_value;
    else if (t.key == L"name")
      campaign.name = first_value;
    else if (t.key == L"highscore")
    {
      if (t.values.size() != 5) continue;
      Highscore highscore;
      highscore.name = first_value;
      highscore.score = (uint16_t)std::stoi(t.values.at(1).c_str());
      highscore.difficulty = (DifficultyType)std::stoi(t.values.at(2).c_str());
      wchar_t* end1;
      highscore.date = std::wcstoll(t.values.at(3).c_str(), &end1, 10);
      wchar_t* end2;
      uint64_t checksum = std::wcstoull(t.values.at(4).c_str(), &end2, 10);
      if(highscore.IsValid(checksum))
        campaign.AddHighscore(highscore);
    }
  }
  return true;
}

bool GameLoader::LoadCampaigns()
{
  campaignMap.clear();
  std::vector<std::filesystem::path> campaign_paths;
  for (const std::filesystem::directory_entry& entry :
    std::filesystem::directory_iterator(ResourceLoader::GetLevelPath()))
  {
    if (entry.is_directory())
    {
      campaign_paths.push_back(entry.path());
    }
  }

  for (std::filesystem::path& campaign_path : campaign_paths)
  {
    Campaign campaign;
    try
    {
      std::wstring name = campaign_path.filename().wstring();
      campaign.name = name;
      campaign.path = campaign_path.wstring();
    }
    catch (std::exception& e)
    {
      printf("Error setting campaign name: %s\n", e.what());
      continue;
    }
    for (const std::filesystem::directory_entry& entry :
      std::filesystem::directory_iterator(campaign_path))
    {
      if (entry.is_regular_file())
      {
        std::wstring filename = entry.path().filename().wstring();
        if (filename == L"campaign.cfg")
        {
          ReadCampaignConfig(entry.path().wstring(), campaign);
        }
        else
        {
          GameLevel level;
          if (!LoadMap(entry.path().wstring(), level))
          {
            wprintf(L"Failed to load level: %s\n", filename.c_str());
            return false;
          }
          campaign.levels.push_back(level);
        }
      }
    }
    wprintf(L"Loaded %s with %d levels\n", campaign.name.c_str(), campaign.levels.size());
    campaignMap[campaign.name] = campaign;
  }  
  return true;
}


//Used only for level editing
bool GameLoader::LoadAssets(const std::wstring& filename)
{
  FileInfo info;
  info.type = FileType::ASSET;
  bool success = LoadFile(filename, info);
  assetBricks = info.bricks;
  return success;
}

bool GameLoader::LoadMap(const std::wstring& filename, GameLevel& out)
{
  GameLevel level;
  FileInfo info;
  info.type = FileType::MAP;
  if (!LoadFile(filename, info))
    return false;

  try {
    std::wstring key = std::filesystem::path(filename).filename().wstring();
    level.map_name = key;
    //level_map[key] = level;
    level.author = info.author;
    for (Brick& brick : info.bricks)
    {
      uint32_t index;
      if (GetBrickIndex(brick.col, brick.row, index))
      {
        level.GetBrickMap().Add(index, brick);
      }
      //level.brickMap[index].push_back(brick);
    }
    out = level;
    return true;
  }
  catch (const std::exception& e)
  {
    printf("Error: %s\n", e.what());
    return false;
  }
}

bool GameLoader::SaveMap(GameLevel& level, std::wstring& save_out)
{
  std::wstring savename(level.map_name);
  std::filesystem::path map_path = ResourceLoader::GetLevelPath();
  if (savename.empty())
  {
    unsigned next = 0;
    do
    {
      if (next > 0x7FFFFFFF) return false;
      savename = std::wstring(L"custom_");
      savename.append(std::to_wstring(next++));
      std::filesystem::path test(map_path);
      test.append(savename);
      if (!std::filesystem::exists(test))
      {
        map_path = test;
        break;
      }
    } while (true);
  } 
  else
  {
    map_path.append(level.map_name);
  }
  std::wofstream output(map_path.wstring());
  if (!output.is_open())
  {
    printf("Failed to save file (%d): %ls\n", GetLastError(),
      map_path.wstring().c_str());
    return false;
  }
  level.map_name = savename;

  if (!level.author.empty())
  {
    output << L"author:" << level.author << L"\n";
  }
  BrickMap::const_iterator map_it = level.GetBrickMap().begin();
  for (; map_it != level.GetBrickMap().end(); ++map_it)
  {
    for (const Brick& brick : map_it->second)
    {
      output << L"brick:";
      output << std::to_wstring(brick.subtype) + L",";
      output << brick.GetSprite() + L",";
      output << std::to_wstring(brick.col) + L",";
      output << std::to_wstring(brick.row) << L"\n";
    }
  }
  output.close();
  printf("Saved map: %ls\n", savename.c_str());
  save_out = savename;
  return true;
}

/*
bool GameLoader::GetLevel(const std::wstring& key, GameLevel& level)
{
  auto it = level_map.find(key);
  if (it != level_map.end())
  {
    level = it->second;
    return true;
  }
  return false;
}
*/