#pragma once
#include <stdint.h>
#include <string>

enum SciterKeys : uint32_t;

int32_t SciterKeyToSwitchKey(SciterKeys key);
int32_t SciterKeyToVKCode(SciterKeys vkcode);
std::string KeyCodeToString(int32_t keyCode);
