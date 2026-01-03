#pragma once
#include "json.h"

JsonValue JsonGetNestedValue(const JsonValue & section, const char * key);
void JsonSetNestedValue(JsonValue & section, const char * key, const JsonValue & value);
