#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <Settings.h>

class SettingsManager
{
public:
    static bool load(void);
    static bool save(void);
private:
    static void init();
};

#endif