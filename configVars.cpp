#include "configVars.h"

bool ConfigVars::isLoaded = false;
std::unordered_map<std::string, bool> ConfigVars::boolDict;
std::unordered_map<std::string, int> ConfigVars::intDict;
std::unordered_map<std::string, float> ConfigVars::floatDict;
std::unordered_map<std::string, std::string> ConfigVars::stringDict;

void ConfigVars::load(const std::string& configFile) {
    std::cout << "Loading config vars from file '" << configFile << "'..." << std::endl;
    try {
        YAML::Node config = YAML::LoadFile(configFile);

        for (auto it = config.begin(); it != config.end(); ++it) {
            const std::string& key = it->first.as<std::string>();
            const YAML::Node& valueNode = it->second;

            // TODO: Find a way to distinguish float from int
            // and bring this functionality back
            //try {
            //    floatDict[key] = valueNode.as<float>();
            //    std::cout << "'" << key << "' is a float" << std::endl;
            //} catch (const YAML::BadConversion& e) {
                try {
                    intDict[key] = valueNode.as<int>();
                    std::cout << "'" << key << "' is an int with value '" << intDict[key] << "'" << std::endl;
                } catch (const YAML::BadConversion& e) {
                    std::string myString = valueNode.as<std::string>();

                    if (myString == "true") {
                        boolDict[key] = true;
                        std::cout << "'" << key << "' is a bool with value 'true'" << std::endl;
                    } else if (myString == "false") {
                        boolDict[key] = false;
                        std::cout << "'" << key << "' is a bool with value 'false'" << std::endl;
                    } else {
                        stringDict[key] = myString;
                        std::cout << "'" << key << "' is a string with value '" << stringDict[key] << "'" << std::endl;
                    }
                }
            //}
        }
        isLoaded = true;
    } catch (const YAML::Exception& ex) {
        std::cerr << "Error loading configuration file: " << ex.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error occurred while loading configuration file." << std::endl;
        exit(1);
    }
}

//TODO: Remerge the getters using templates

const int& ConfigVars::getInt(const std::string& key){
    try {
        if (!isLoaded) {
            throw std::runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = intDict.find(key);
        if (it != intDict.end()) {
            return it->second;
        }

        throw std::runtime_error("Variable not found: " + key);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error occurred while retrieving configuration value." << std::endl;
        exit(1);
    }
}

const float& ConfigVars::getFloat(const std::string& key) {
    try {
        if (!isLoaded) {
            throw std::runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = floatDict.find(key);
        if (it != floatDict.end()) {
            return it->second;
        }

        throw std::runtime_error("Variable not found: " + key);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error occurred while retrieving configuration value." << std::endl;
        exit(1);
    }
}

const bool& ConfigVars::getBool(const std::string& key) {
    try {
        if (!isLoaded) {
            throw std::runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = boolDict.find(key);
        if (it != boolDict.end()) {
            return it->second;
        }

        throw std::runtime_error("Variable not found: " + key);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error occurred while retrieving configuration value." << std::endl;
        exit(1);
    }
}

const std::string& ConfigVars::getString(const std::string& key) {
    try {
        if (!isLoaded) {
            throw std::runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = stringDict.find(key);
        if (it != stringDict.end()) {
            return it->second;
        }

        throw std::runtime_error("Variable not found: " + key);
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error occurred while retrieving configuration value." << std::endl;
        exit(1);
    }
}

const char* ConfigVars::getCString(const std::string& key) {
    try {
        auto it = stringDict.find(key);
        if (it != stringDict.end()) {
            return it->second.c_str();
        } else {
            throw std::runtime_error("Error: Variable not found: " + key);
        }
    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        exit(1);
    } catch (...) {
        std::cerr << "Unknown error occurred while retrieving configuration value." << std::endl;
        exit(1);
    }
}
