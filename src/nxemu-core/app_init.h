#pragma once

__interface INotification;

bool AppInit(INotification * notification, const char * configPath);
void AppCleanup();