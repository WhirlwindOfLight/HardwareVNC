#ifndef CONFIGVARS_H
#define CONFIGVARS_H

#include <yaml-cpp/yaml.h>
#include "stdDecls.h"

class ConfigVars {
private:
    static bool isLoaded;
    static unordered_map<string, bool> boolDict;
    static unordered_map<string, int> intDict;
    static unordered_map<string, float> floatDict;
    static unordered_map<string, string> stringDict;

public:
    static void load(const string& configFile);
    
    static const int& getInt(const string& key);
    static const float& getFloat(const string& key);
    static const bool& getBool(const string& key);
    static const string& getString(const string& key);
    static const char* getCString(const string& key);
};

#endif
