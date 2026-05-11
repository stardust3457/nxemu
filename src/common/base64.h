#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string>

std::string base64_encode(const uint8_t * data, size_t len);
