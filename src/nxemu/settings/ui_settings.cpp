#include "ui_settings.h"
#include <common/json.h>
#include <common/path.h>
#include <nxemu-core/settings/settings.h>
#include <nxemu-core/notification.h>

namespace
{
    Path GetDefaultLanguageDir();

    enum class SettingType
    {
        Path,
        String,
        Bool,
        StringList,
    };

    class UISetting
    {
    public:
        UISetting(const char * key, Path * path, std::string * value, Path defaultPath, const char * defaultValue);
        UISetting(const char * key, std::string * value, const char * defaultValue);
        UISetting(const char * key, bool * value, bool defaultValue);
        UISetting(const char * key, Stringlist * value);

        const char * json_key;
        SettingType settingType;
        union
        {
            Path * path;
            std::string * string;
            bool * boolean;
            Stringlist * string_list;
        } setting;
        std::string * settingValue;
        Path default_path;
        const char * default_string;
        const bool default_bool;
    };

    static UISetting settings[] = {
        { "RecentFiles", &uiSettings.recentFiles},
        { "GameDirectories", &uiSettings.gameDirectories},
        { "LanguageDirectory", &uiSettings.languageDir, &uiSettings.languageDirValue, GetDefaultLanguageDir(), "./lang"},
        { "LanguageBase", &uiSettings.languageBase, "english"},
        { "LanguageCurrent", &uiSettings.languageCurrent, "english"},
        { "SciterConsole", &uiSettings.sciterConsole, false},
        { "PerformVulkanCheck", &uiSettings.performVulkanCheck, true},
        { nullptr, &uiSettings.hasBrokenVulkan, false},
        { nullptr, &uiSettings.enableAllControllers, false},
    };

} // namespace

UISettings uiSettings = {};

void LoadUISetting(void)
{
    uiSettings = {};
    for (const UISetting & setting : settings)
    {
        switch (setting.settingType)
        {
        case SettingType::Path:
            *(setting.settingValue) = setting.default_string;
            *(setting.setting.path) = setting.default_path;
            break;
        case SettingType::String:
            *(setting.setting.string) = setting.default_string;
            break;
        case SettingType::Bool:
            *(setting.setting.boolean) = setting.default_bool;
            break;
        case SettingType::StringList:
            *(setting.setting.string_list) = {};
            break;
        default:
            g_notify->BreakPoint(__FILE__, __LINE__);
        }
    }

    JsonValue section = SettingsStore::GetInstance().GetSettings("UI");
    for (const UISetting & setting : settings)
    {
        const JsonValue * node;
        switch (setting.settingType)
        {
        case SettingType::Path:
            node = setting.json_key ? section.Find(setting.json_key) : nullptr;
            if (node != nullptr && node->isString())
            {
                std::string dirValue = node->asString();
                if (!dirValue.empty())
                {
                    *(setting.settingValue) = dirValue;
                    *(setting.setting.path) = Path(*(setting.settingValue), "").DirectoryNormalize(Path(Path::MODULE_DIRECTORY));
                }
            }
            break;
        case SettingType::String:
            node = setting.json_key ? section.Find(setting.json_key) : nullptr;
            if (node != nullptr && node->isString())
            {
                *(setting.setting.string) = node->asString();
            }
            break;
        case SettingType::Bool:
            node = setting.json_key ? section.Find(setting.json_key) : nullptr;
            if (node != nullptr && node->isBool())
            {
                *(setting.setting.boolean) = node->asBool();
            }
            break;
        case SettingType::StringList:
            node = setting.json_key ? section.Find(setting.json_key) : nullptr;
            if (node != nullptr && node->isArray())
            {
                for (uint32_t i = 0; i < node->size(); i++)
                {
                    const JsonValue & item = (*node)[i];
                    if (item.isString())
                    {                        
                        setting.setting.string_list->push_back(item.asString());
                    }
                }
            }
            break;
        default:
            g_notify->BreakPoint(__FILE__, __LINE__);
        }
    }
}

void SaveUISetting(void)
{
    JsonValue json;
    for (const UISetting& setting : settings)
    {
        switch (setting.settingType)
        {
        case SettingType::StringList:
            if (!setting.setting.string_list->empty())
            {
                JsonValue jsonList(JsonValueType::Array);
                for (const std::string & item : *(setting.setting.string_list))
                {
                    jsonList.Append(JsonValue(item));
                }
                json[setting.json_key] = std::move(jsonList);
            }
            break;
        case SettingType::Path:
            if (strcmp(setting.settingValue->c_str(), setting.default_string) != 0)
            {
                json[setting.json_key] = JsonValue(*setting.settingValue);
            }
            break;
        case SettingType::String:
            if (strcmp(setting.setting.string->c_str(), setting.default_string) != 0)
            {
                json[setting.json_key] = JsonValue(*setting.setting.string);
            }
            break;
        case SettingType::Bool:
            if (*setting.setting.boolean != setting.default_bool)
            {
                json[setting.json_key] = JsonValue(*setting.setting.boolean);
            }
            break;
        default:
            g_notify->BreakPoint(__FILE__, __LINE__);
        }
    }
    
    SettingsStore & settingstore = SettingsStore::GetInstance();
    settingstore.SetSettings("UI", json);
    settingstore.Save();
}

namespace
{
    Path GetDefaultLanguageDir()
    {
        Path dir(Path::MODULE_DIRECTORY);
        dir.AppendDirectory("lang");
        return dir;
    }

    UISetting::UISetting(const char * key, Path * path, std::string * value, Path defaultPath, const char * defaultValue) :
        json_key(key),
        settingType(SettingType::Path),
        settingValue(value),
        default_path(defaultPath),
        default_string(defaultValue),
        default_bool(false)
    {
        setting.path = path;
    }

    UISetting::UISetting(const char * key, std::string * value, const char * defaultValue) :
        json_key(key),
        settingType(SettingType::String),
        settingValue(nullptr),
        default_string(defaultValue),
        default_bool(false)
    {
        setting.string = value;
    }

    UISetting::UISetting(const char * key, bool * value, bool defaultValue) :
        json_key(key),
        settingType(SettingType::Bool),
        settingValue(nullptr),
        default_string(nullptr),
        default_bool(defaultValue)
    {
        setting.boolean = value;
    }

    UISetting::UISetting(const char* key, Stringlist* value) :
        json_key(key),
        settingType(SettingType::StringList),
        settingValue(nullptr),
        default_string(nullptr),
        default_bool(false)
    {
        setting.string_list = value;
    }

}