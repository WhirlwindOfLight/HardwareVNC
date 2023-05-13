#ifndef CONFIGVARS_H
#define CONFIGVARS_H

#include <yaml-cpp/yaml.h>
#include <unordered_map>
#include <string>
#include <stdexcept>
#include <iostream>

class ConfigVars {
private:
    static bool isLoaded;
    static std::unordered_map<std::string, bool> boolDict;
    static std::unordered_map<std::string, int> intDict;
    static std::unordered_map<std::string, float> floatDict;
    static std::unordered_map<std::string, std::string> stringDict;

public:
    static void load(const std::string& configFile);
    
    static const int& getInt(const std::string& key);
    static const float& getFloat(const std::string& key);
    static const bool& getBool(const std::string& key);
    static const std::string& getString(const std::string& key);
    static const char* getCString(const std::string& key);
};

#endif
