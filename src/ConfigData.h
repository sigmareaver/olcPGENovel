
#pragma once

#include <any>
#include <concepts>
#include <iomanip>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <typeindex>
#include <utility>

#include "Log.h"


namespace Config
{
    enum DefaultConfigValue
    {
        CFG_NODEFAULT = -1,

        CFG_SCREENWIDTH = 0, CFG_SCREENHEIGHT, CFG_FULLSCREEN, CFG_VSYNC,

        CFG_LUAYIELD, CFG_LUAYIELDINSTRUCTIONS,

        MAX_DEFAULT_CONFIG_VALUE
    };


    template <typename T>
    concept is_c_str = requires(T t)
    {
        {t} -> std::convertible_to<const char *>;
    };

    template <typename T>
    concept is_std_str = std::is_same_v<std::string, T>;

    struct ValuePack : public std::pair<std::type_index, std::any>
    {
        std::string stored_value_str;

        template <typename T>
        ValuePack(T&& t) : 
            std::pair<std::type_index, std::any>(typeid(T), std::any(std::forward<T>(t)))
        {
            std::ostringstream oss;
            oss << std::setprecision(8) << std::noshowpoint << t;
            stored_value_str = oss.str();
            //stored_value_str = std::to_string(t);
        }

        template <is_std_str T>
        ValuePack(T&& t) :
            std::pair<std::type_index, std::any>(typeid(T), std::any(std::forward<T>(t)))
        {
        }

        template <is_c_str T>
        ValuePack(T&& t) : 
            std::pair<std::type_index, std::any>(typeid(T), std::any(std::string(std::forward<T>(t))))
        {
        }

        template <typename T>
        inline const T try_any_cast() const
        {
            try
            {
                return std::forward<T>(std::any_cast<T>(second));
            }
            catch (const std::bad_any_cast& e)
            {
                using T_unrefed = typename std::remove_cvref_t<T>;
                static T_unrefed t{};
                Logging::Log(Logging::Channel::Error, "ValuePack: {} of type: {} (original type was {})", e.what(), typeid(T).name(), first.name());
                return std::forward<T>(t);
            }
        }

        template <typename T>
        inline T try_any_cast()
        {
            try
            {
                return std::forward<T>(std::any_cast<T>(second));
            }
            catch (const std::bad_any_cast& e)
            {
                using T_unrefed = typename std::remove_cvref_t<T>;
                static T_unrefed t;
                t = T_unrefed{};
                Logging::Log(Logging::Channel::Error, "ValuePack: {} of type: {} (original type was {})", e.what(), typeid(T).name(), first.name());
                return std::forward<T>(t);
            }
        }

        template <typename T>
        inline operator const T&() const
        {
            return std::forward<const T&>(try_any_cast<const T&>());
        }

        template <typename T>
        inline operator T&()
        {
            return std::forward<T&>(try_any_cast<T&>());
        }

        template <typename T>
        inline operator T() const
        {
            return std::forward<T>(try_any_cast<T>());
        }

        inline operator const std::string&() const
        {
            if (!stored_value_str.empty())
                return stored_value_str;
            return std::forward<const std::string&>(std::any_cast<const std::string&>(second));
        }

        inline operator std::string() const
        {
            if (!stored_value_str.empty())
                return stored_value_str;
            return std::forward<const std::string&>(std::any_cast<const std::string&>(second));
        }

        template <typename T>
        inline ValuePack& operator=(T&& t)
        {
            std::ostringstream oss;
            oss << std::setprecision(8) << std::noshowpoint << t;
            stored_value_str = oss.str();
            //stored_value_str = std::to_string(t);
            first = typeid(T);
            second = std::any(std::forward<T>(t));
            return *this;
        }

        template <is_std_str T>
        inline ValuePack& operator=(T&& t)
        {
            stored_value_str.clear();
            first = typeid(std::string);
            second = std::any(std::forward<T>(t));
            return *this;
        }

        template <is_c_str T>
        inline ValuePack& operator=(T&& t)
        {
            stored_value_str.clear();
            first = typeid(std::string);
            second = std::any(std::string(std::forward<T>(t)));
            return *this;
        }
        
        friend std::ostream& operator<<(std::ostream&, const ValuePack& vp);
    };


    std::ostream& operator<<(std::ostream&, const ValuePack& vp);

    DefaultConfigValue ConfigKeyToEnum(const std::string& key);
    std::optional<ValuePack> DefaultValue(DefaultConfigValue enum_val);


    struct ConfigData
    {
        using MapType = std::map<const std::string, std::optional<ValuePack>, std::less<const std::string>>;
        MapType values;

        inline ValuePack& operator[](const std::string& key) { return Get(key); }

        inline ValuePack& Get(const std::string& key)
        {
            auto it = values.find(key);
            if (it != values.end())
                return it->second.value();
            auto default_value = DefaultValue(ConfigKeyToEnum(key));
            Set(key, default_value.value_or(0));
            return values[key].value();
        }

        template <typename T>
        void Set(const std::string& key, T&& t)
        {
            values[key] = std::move(std::optional<ValuePack>(std::forward<T>(t)));
        }

        void Set(const std::string& key, std::optional<ValuePack>&& opt_value_pack)
        {
            values[key] = std::move(opt_value_pack);
        }
    };

    void GenerateDefaultConfig(ConfigData& config);
}
