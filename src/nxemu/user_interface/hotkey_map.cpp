#include "hotkey_map.h"
#include <sciter_handler.h>

namespace
{
const uint32_t kKeyboardStateControl = 0x0040u | 0x0080u;
const uint32_t kKeyboardStateAlt = 0x0100u | 0x0200u;
const uint32_t kKeyboardStateShift = 0x0001u | 0x0002u;

bool IsModifierKeyCode(uint32_t keyCode)
{
    return keyCode >= (uint32_t)SCITER_KEY_LEFT_SHIFT && keyCode <= (uint32_t)SCITER_KEY_RIGHT_SUPER;
}

bool AccelEqual(const MenuBarAccelerator & a, const MenuBarAccelerator & b)
{
    return a.key == b.key && a.ctrl == b.ctrl && a.alt == b.alt && a.shift == b.shift;
}

} // namespace

std::string MenuBarAcceleratorToJson(const MenuBarAccelerator & accel)
{
    JsonValue o(JsonValueType::Object);
    o["key"] = JsonValue((int64_t)accel.key);
    o["ctrl"] = JsonValue(accel.ctrl);
    o["alt"] = JsonValue(accel.alt);
    o["shift"] = JsonValue(accel.shift);
    return JsonStyledWriter().write(o);
}

MenuBarAccelerator MenuBarAcceleratorFromKeyEvent(uint32_t keyCode, uint32_t keyboardState)
{
    MenuBarAccelerator none{};
    if (IsModifierKeyCode(keyCode))
    {
        return none;
    }
    bool ctrl = (keyboardState & kKeyboardStateControl) != 0;
    bool alt = (keyboardState & kKeyboardStateAlt) != 0;
    bool shift = (keyboardState & kKeyboardStateShift) != 0;

    // Escape is bindable like any key (e.g. Exit Fullscreen). Cancel capture by clicking outside the cell.
    if (keyCode == (uint32_t)SCITER_KEY_ESCAPE)
    {
        MenuBarAccelerator a{};
        a.key = keyCode;
        a.ctrl = ctrl;
        a.alt = alt;
        a.shift = shift;
        return a;
    }
    if (ctrl || alt || shift)
    {
        MenuBarAccelerator a{};
        a.key = keyCode;
        a.ctrl = ctrl;
        a.alt = alt;
        a.shift = shift;
        return a;
    }
    if (keyCode >= (uint32_t)SCITER_KEY_F1 && keyCode <= (uint32_t)SCITER_KEY_F24)
    {
        MenuBarAccelerator a{};
        a.key = keyCode;
        return a;
    }
    return none;
}

JsonValue HotkeyMapToJsonObject(const HotkeyMap & m)
{
    JsonValue jsonObject(JsonValueType::Object);
    for (const HotkeyMap::value_type & item : m)
    {
        jsonObject[item.first] = JsonValue(MenuBarAcceleratorToJson(item.second));
    }
    return jsonObject;
}

JsonValue HotkeyMapToJsonObjectDiff(const HotkeyMap & current, const HotkeyMap & defaults)
{
    JsonValue jsonObject(JsonValueType::Object);
    for (const HotkeyMap::value_type & item : current)
    {
        const HotkeyMap::const_iterator defIt = defaults.find(item.first);
        if (defIt != defaults.end())
        {
            if (AccelEqual(item.second, defIt->second))
            {
                continue;
            }
        }
        else if (item.second.IsNone())
        {
            continue;
        }
        uint32_t defaultKey = defIt != defaults.end() ? defIt->second.key : 0;
        if (item.second.key != defaultKey)
        {
            JsonValue o(JsonValueType::Object);
            o["key"] = JsonValue((int64_t)item.second.key);
            if (item.second.ctrl)
            {
                o["ctrl"] = JsonValue(item.second.ctrl);            
            }
            if (item.second.alt)
            {
                o["alt"] = JsonValue(item.second.alt);
            }
            if (item.second.shift)
            {
                o["shift"] = JsonValue(item.second.shift);
            }
            jsonObject[item.first] = o;
        }
    }
    return jsonObject;
}

std::string SerializeUIHotkeysMap(const HotkeyMap & m)
{
    return JsonStyledWriter().write(HotkeyMapToJsonObject(m));
}

bool DeserializeHotkeyMapFromJsonObject(const JsonValue & root, HotkeyMap & out)
{
    out = {};
    if (!root.isObject())
    {
        return false;
    }
    for (const std::string & name : root.GetMemberNames())
    {
        const JsonValue & v = root[name];
        if (!v.isString())
        {
            continue;
        }
        const std::string & json = v.asString();
        if (json.empty())
        {
            continue;
        }

        JsonReader reader;
        JsonValue item;
        if (!reader.Parse(json.c_str(), json.c_str() + json.size(), item) || !item.isObject())
        {
            continue;
        }
        if (!item.isMember("key"))
        {
            continue;
        }
        const JsonValue & k = item["key"];
        if (!k.isInt())
        {
            continue;
        }

        MenuBarAccelerator outAccel;
        outAccel.key = (uint32_t)k.asInt64();
        if (item.isMember("ctrl") && item["ctrl"].isBool())
        {
            outAccel.ctrl = item["ctrl"].asBool();
        }
        if (item.isMember("alt") && item["alt"].isBool())
        {
            outAccel.alt = item["alt"].asBool();
        }
        if (item.isMember("shift") && item["shift"].isBool())
        {
            outAccel.shift = item["shift"].asBool();
        }
        out[name] = outAccel;
    }
    return true;
}

bool DeserializeUIHotkeysMap(const std::string & json, HotkeyMap & out)
{
    out.clear();
    if (json.empty())
    {
        return true;
    }
    JsonReader reader;
    JsonValue root;
    if (!reader.Parse(json.data(), json.data() + json.size(), root))
    {
        return false;
    }
    return DeserializeHotkeyMapFromJsonObject(root, out);
}
