#pragma once

// Internal
//#include "ObjectMap.h"

// External
#include <bitset>
#include <filesystem>
#include <mutex>
#include <string>
#include <yaml-cpp/yaml.h>

namespace Config
{
	struct ConfigData;
}

#define KEY(o, a, b)		(o) << YAML::Key << (a) << YAML::Value << (b)
#define BEGIN_FSEQ(o, a)	(o) << YAML::Key << (a) << YAML::Value << YAML::Flow << YAML::BeginSeq
#define BEGIN_BSEQ(o, a)	(o) << YAML::Key << (a) << YAML::Value << YAML::Block << YAML::BeginSeq
#define SEQ_VALUE(o, a)		(o) << (a)
#define END_SEQ(o)			(o) << YAML::EndSeq
#define BEGIN_MAP(o, a)		(o) << YAML::Key << (a) << YAML::Value << YAML::BeginMap
#define END_MAP(o)			(o) << YAML::EndMap
#define REGION(o, a, b)		(o) << YAML::Key << ("$" a "$") << YAML::Value << (b)
#define REGION_VER(o, a, b)	((o)[("$" a "$")] && ((o)[("$" a "$")].as<int>() == (b)))


namespace Serializer
{
	// Resource dirs
	constexpr const char* const RES_DIR = "res";						// res/
	constexpr const char* const FONT_DIR = "font";						// res/font/
	constexpr const char* const IMAGE_DIR = "image";					// res/font/
	constexpr const char* const LUA_DIR = "lua";						// res/lua/
	constexpr const char* const GUI_DIR	= "gui";						// res/lua/GUI/

	const std::filesystem::path RES_PATH{std::filesystem::current_path() / RES_DIR};	// res/
	const std::filesystem::path FONT_PATH{RES_PATH / FONT_DIR};							// res/font/
	const std::filesystem::path IMAGE_PATH{RES_PATH / IMAGE_DIR};						// res/font/
	const std::filesystem::path LUA_PATH{RES_PATH / LUA_DIR};							// res/lua/
	const std::filesystem::path GUI_PATH{LUA_PATH / GUI_DIR};							// res/lua/GUI/

	// Save dir
	constexpr const char* const SAVE_DIR = "save";			// save/

	constexpr const char* const CONFIG_FILE = "config.ini";

	constexpr const char* const SAVE_FILE_EXT = ".sav";

	constexpr const int CONFIG_VERSION = 1;
	constexpr const int GAME_SETTINGS_VERSION = 1;
	constexpr const int SAVE_VERSION = 1;

	extern std::mutex save_mutex;

	int SaveGame(const char* filename, bool overwrite);
	bool LoadGame(const char* filename);

	void SaveConfigData(YAML::Emitter& out, Config::ConfigData& config);
	void SaveConfigFile(Config::ConfigData& config);
	bool LoadConfigData(YAML::Node& node, Config::ConfigData& config);
	bool LoadConfigFile(Config::ConfigData& config);

	void CheckDirs();
	void FormatEmitter(YAML::Emitter& emitter);

	template<size_t Size>
	YAML::Emitter& operator<< (YAML::Emitter& out, const std::bitset<Size>& bits);
	template<size_t Size>
	std::bitset<Size>& operator<< (std::bitset<Size>& bits, const std::string bitstring);

}
