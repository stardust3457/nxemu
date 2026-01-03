#include "json_util.h"
#include "std_string.h"

JsonValue JsonGetNestedValue(const JsonValue & section, const char * key)
{
    strvector parts = stdstr(key).Tokenize('\\');
    JsonValue current = section;

    for (const stdstr & part : parts)
    {
        if (!current.isObject())
        {
            return JsonValue();
        }
        current = current[part];
    }
    return current;
}

void JsonSetNestedValue(JsonValue & section, const char * key, const JsonValue & value)
{
    strvector parts = stdstr(key).Tokenize('\\');
    if (parts.empty())
    {
        return;
    }

    JsonValue * current = &section;
    for (size_t i = 0; i < parts.size() - 1; i++)
    {
        if (!(*current)[parts[i]].isObject())
        {
            (*current)[parts[i]] = JsonValue(JsonValueType::Object);
        }
        current = &(*current)[parts[i]];
    }
    (*current)[parts.back()] = value;
}
