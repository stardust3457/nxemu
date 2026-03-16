#pragma once
#include <nxemu-module-spec/base.h>

nxinterface INotification;

bool AppInit(INotification * notification, const char * configPath);
void AppCleanup();