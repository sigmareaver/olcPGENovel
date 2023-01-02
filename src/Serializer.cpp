// Internal
#include "Serializer.h"
#include "ConfigData.h"
#include "Log.h"
#include "StringHashMap.h"

// External
#include <bitset>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <mutex>
#include <string>

namespace Serializer
{
	std::mutex save_mutex;

	template<typename T, typename K>
	T SafeIndex(YAML::Node& node, const K& key)
	{
		try
		{
			if (!node[key])
				throw std::runtime_error("invalid key");
			return node[key].as<T>();
		}
		catch (const std::exception& e)
		{
			WARN("Serializer: (key: {}) {}", key, e.what());
			return T{};
		}
	}

	template<size_t Size>
	YAML::Emitter& operator<< (YAML::Emitter& out, const std::bitset<Size>& bits)
	{
		out << bits.to_string();
		return out;
	}

	template<size_t Size>
	std::bitset<Size>& operator<< (std::bitset<Size>& bits, const std::string bitstring)
	{
		std::istringstream bitstream(bitstring);
		bitstream >> bits;
		return bits;
	}

	int SaveGame(const char* filename, bool overwrite)
	{
		YAML::Emitter out;

		if (!filename || strlen(filename) == 0)
		{
			ERR("SaveGame(): Invalid filename!");
			return -1;
		}

		std::filesystem::path save_file = std::filesystem::current_path() / SAVE_DIR / filename;
		save_file += SAVE_FILE_EXT;

		if (std::filesystem::exists(save_file) && !overwrite)
			return 0;

		FormatEmitter(out);
		out << YAML::BeginMap;

		REGION(out, "SAVE", SAVE_VERSION);
		KEY(out, "Filename", filename);

		REGION(out, "SETTINGS", GAME_SETTINGS_VERSION);
		BEGIN_MAP(out, "Settings");
		//SaveConfigData(out, g_current_game->settings);
		END_MAP(out);

		out << YAML::EndMap;

		//std::filesystem::path path = std::filesystem::current_path() / DATA_DIR / WORLD_DIR;
		//path /= area_index->filename;
		std::ofstream fp(save_file);
		fp << out.c_str();
		LOG("{} saved.", filename);

		return 1;
	}

	bool LoadGame(const char* filename)
	{
		if (!filename || strlen(filename) == 0)
		{
			ERR("LoadGame(): Invalid filename!");
			return false;
		}

		std::filesystem::path save_file = std::filesystem::current_path() / SAVE_DIR / filename;
		save_file += SAVE_FILE_EXT;

		if (!std::filesystem::exists(save_file))
		{
			ERR("LoadGame(): Specified filename doesn't exist!");
			return false;
		}

		YAML::Node node = YAML::LoadFile(save_file.string());

		if (!REGION_VER(node, "SAVE", SAVE_VERSION))
		{
			ERR("LoadGame(): Invalid SAVE Format");
			return false;
		}
		else
		{
			if (REGION_VER(node, "SETTINGS", GAME_SETTINGS_VERSION))
			{
				auto settings_map = node["Settings"];
				//if (!LoadConfigData(settings_map, g_current_game->settings))
				//{
				//	Log(Channel::Error, "LoadGame(): Failed to load game settings. Aborting!");
				//	return false;
				//}
			}
		}

		return true;
	}

	void CheckDirs()
	{
		std::filesystem::path path = std::filesystem::current_path();

		{ // Resource Dir
			if (!std::filesystem::exists(path / RES_DIR))
				std::filesystem::create_directory(path / RES_DIR);
			if (!std::filesystem::exists(path / FONT_DIR))
				std::filesystem::create_directory(path / FONT_DIR);
			if (!std::filesystem::exists(path / IMAGE_DIR))
				std::filesystem::create_directory(path / IMAGE_DIR);
			{ // Lua Dir
				if (!std::filesystem::exists(path / LUA_DIR))
					std::filesystem::create_directory(path / LUA_DIR);
				if (!std::filesystem::exists(path / GUI_DIR))
					std::filesystem::create_directory(path / GUI_DIR);
			}
		}
		// Save Dir
		if (!std::filesystem::exists(path / SAVE_DIR))
			std::filesystem::create_directory(path / SAVE_DIR);
	}

	void SaveConfigData(YAML::Emitter& out, Config::ConfigData& config)
	{
		for (auto it = config.values.begin(); it != config.values.end(); ++it)
		{
			auto& value_pack = it->second.value();
			if (value_pack.first == typeid(bool))
				KEY(out, it->first, std::any_cast<bool>(value_pack.second));
			else if (value_pack.first == typeid(char8_t))
				KEY(out, it->first, std::any_cast<char8_t>(value_pack.second));
			else if (value_pack.first == typeid(int32_t))
				KEY(out, it->first, std::any_cast<int32_t>(value_pack.second));
			else if (value_pack.first == typeid(uint32_t))
				KEY(out, it->first, std::any_cast<uint32_t>(value_pack.second));
			else if (value_pack.first == typeid(float_t))
				KEY(out, it->first, std::any_cast<float_t>(value_pack.second));
			else if (value_pack.first == typeid(double_t))
				KEY(out, it->first, std::any_cast<double_t>(value_pack.second));
			else if (value_pack.first == typeid(std::string))
				KEY(out, it->first, std::any_cast<std::string>(value_pack.second));
			else if (value_pack.first == typeid(char*))
				KEY(out, it->first, std::any_cast<char*>(value_pack.second));
		}
	}

	void SaveConfigFile(Config::ConfigData& config)
	{
		YAML::Emitter out;

		FormatEmitter(out);

		out << YAML::BeginMap;
		REGION(out, "CONFIG", CONFIG_VERSION);
		BEGIN_MAP(out, "Settings");
		SaveConfigData(out, config);
		out << YAML::EndMap;
		out << YAML::EndMap;

		std::filesystem::path path = std::filesystem::current_path() / CONFIG_FILE;
		std::ofstream fp(path);
		fp << out.c_str();
	}

	bool LoadConfigData(YAML::Node& node, Config::ConfigData& config)
	{
		for (auto it : node)
		{
			if (it.second.IsScalar())
			{
				std::string str_value = it.second.as<std::string>();
				if (!str_value.compare("true") || !str_value.compare("false"))
					config.Set(it.first.as<std::string>(), it.second.as<bool>());
				else if (!std::any_of(str_value.begin(), str_value.end(), [](char c) { return !(std::isdigit(c)); }))
					config.Set(it.first.as<std::string>(), it.second.as<int32_t>());
				else if (!std::any_of(str_value.begin(), str_value.end(), [](char c) { return !(std::isdigit(c) || c == '.'); }))
					config.Set(it.first.as<std::string>(), it.second.as<double_t>());
				else
					config.Set(it.first.as<std::string>(), it.second.as<std::string>());
			}
			// STUB: Add support for sub-maps/sequences?
		}
		return true;
	}

	bool LoadConfigFile(Config::ConfigData& config)
	{
		std::filesystem::path config_file = std::filesystem::current_path() / CONFIG_FILE;

		if (!std::filesystem::exists(config_file))
			return false;

		YAML::Node node = YAML::LoadFile(config_file.string());
		if (REGION_VER(node, "CONFIG", CONFIG_VERSION))
		{
			auto config_map = node["Settings"];
			return LoadConfigData(config_map, config);
		}
		return false;
	}

	void FormatEmitter(YAML::Emitter& emitter)
	{
		if (!emitter.good())
			return;
	}

}
