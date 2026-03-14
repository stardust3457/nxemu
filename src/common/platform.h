#pragma once

#ifndef _WIN32
#include <stdarg.h>

int _vscprintf(const char * format, va_list pargs);

#endif
