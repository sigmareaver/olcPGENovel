
#include "ConfigData.h"

#include <string.h>


namespace Config
{
    std::ostream& operator<<(std::ostream& os, const ValuePack& vp)
    {
        os << vp.operator const std::string&();
        return os;
    }

    extern const char* const default_config_strings[] = {
	    "ScreenWidth", "ScreenHeight", "Fullscreen", "VSync",
        "LuaYield", "LuaYieldInstructions"
    };

    DefaultConfigValue ConfigKeyToEnum(const std::string& key)
    {
        for (size_t i = 0; i < MAX_DEFAULT_CONFIG_VALUE; ++i)
        {
            if (!key.compare(default_config_strings[i]))
                return DefaultConfigValue(i);
        }
        return CFG_NODEFAULT;
    }

    std::optional<ValuePack> DefaultValue(DefaultConfigValue enum_val)
    {
        static std::optional<ValuePack> opt_value_pack = std::nullopt;
        switch (enum_val)
        {
        case CFG_SCREENWIDTH:
            opt_value_pack = std::optional<ValuePack>(ValuePack(1280));
            break;
        case CFG_SCREENHEIGHT:
            opt_value_pack = std::optional<ValuePack>(ValuePack(720));
            break;
        case CFG_FULLSCREEN:
            opt_value_pack = std::optional<ValuePack>(ValuePack(false));
            break;
        case CFG_VSYNC:
            opt_value_pack = std::optional<ValuePack>(ValuePack(true));
            break;
        case CFG_LUAYIELD:
            opt_value_pack = std::optional<ValuePack>(ValuePack(true));
            break;
        case CFG_LUAYIELDINSTRUCTIONS:
            opt_value_pack = std::optional<ValuePack>(ValuePack(10000));
            break;
        default:
            return std::nullopt;
        }
        return opt_value_pack;
    }

    void GenerateDefaultConfig(ConfigData& config)
    {
        config.values.clear();
        for (size_t i = 0; i < MAX_DEFAULT_CONFIG_VALUE; ++i)
        {
            config.Set(default_config_strings[i], DefaultValue(DefaultConfigValue(i)).value());
        }
    }

}
