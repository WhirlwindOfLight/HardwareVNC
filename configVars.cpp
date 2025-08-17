#include "configVars.h"

bool ConfigVars::isLoaded = false;
unordered_map<string, bool> ConfigVars::boolDict;
unordered_map<string, int> ConfigVars::intDict;
unordered_map<string, float> ConfigVars::floatDict;
unordered_map<string, string> ConfigVars::stringDict;

void ConfigVars::load(const string& configFile) {
    cout << "Loading config vars from file '" << configFile << "'..." << endl;
    try {
        YAML::Node config = YAML::LoadFile(configFile);

        for (auto it = config.begin(); it != config.end(); ++it) {
            const string& key = it->first.as<string>();
            const YAML::Node& valueNode = it->second;

            // TODO: Find a way to distinguish float from int
            // and bring this functionality back
            //try {
            //    floatDict[key] = valueNode.as<float>();
            //    cout << "'" << key << "' is a float" << endl;
            //} catch (const YAML::BadConversion& e) {
                try {
                    intDict[key] = valueNode.as<int>();
                    cout << "'" << key << "' is an int with value '" << intDict[key] << "'" << endl;
                } catch (const YAML::BadConversion& e) {
                    string myString = valueNode.as<string>();

                    if (myString == "true") {
                        boolDict[key] = true;
                        cout << "'" << key << "' is a bool with value 'true'" << endl;
                    } else if (myString == "false") {
                        boolDict[key] = false;
                        cout << "'" << key << "' is a bool with value 'false'" << endl;
                    } else {
                        stringDict[key] = myString;
                        cout << "'" << key << "' is a string with value '" << stringDict[key] << "'" << endl;
                    }
                }
            //}
        }
        isLoaded = true;
    } catch (const YAML::Exception& ex) {
        cerr << "Error loading configuration file: " << ex.what() << endl;
        exit(1);
    } catch (...) {
        cerr << "Unknown error occurred while loading configuration file." << endl;
        exit(1);
    }
}

//TODO: Remerge the getters using templates

const int& ConfigVars::getInt(const string& key){
    try {
        if (!isLoaded) {
            throw runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = intDict.find(key);
        if (it != intDict.end()) {
            return it->second;
        }

        throw runtime_error("Variable not found: " + key);
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        exit(1);
    } catch (...) {
        cerr << "Unknown error occurred while retrieving configuration value." << endl;
        exit(1);
    }
}

const float& ConfigVars::getFloat(const string& key) {
    try {
        if (!isLoaded) {
            throw runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = floatDict.find(key);
        if (it != floatDict.end()) {
            return it->second;
        }

        throw runtime_error("Variable not found: " + key);
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        exit(1);
    } catch (...) {
        cerr << "Unknown error occurred while retrieving configuration value." << endl;
        exit(1);
    }
}

const bool& ConfigVars::getBool(const string& key) {
    try {
        if (!isLoaded) {
            throw runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = boolDict.find(key);
        if (it != boolDict.end()) {
            return it->second;
        }

        throw runtime_error("Variable not found: " + key);
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        exit(1);
    } catch (...) {
        cerr << "Unknown error occurred while retrieving configuration value." << endl;
        exit(1);
    }
}

const string& ConfigVars::getString(const string& key) {
    try {
        if (!isLoaded) {
            throw runtime_error("Configuration not loaded. Call 'load' function before accessing variables.");
        }

        auto it = stringDict.find(key);
        if (it != stringDict.end()) {
            return it->second;
        }

        throw runtime_error("Variable not found: " + key);
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        exit(1);
    } catch (...) {
        cerr << "Unknown error occurred while retrieving configuration value." << endl;
        exit(1);
    }
}

const char* ConfigVars::getCString(const string& key) {
    try {
        auto it = stringDict.find(key);
        if (it != stringDict.end()) {
            return it->second.c_str();
        } else {
            throw runtime_error("Error: Variable not found: " + key);
        }
    } catch (const exception& ex) {
        cerr << "Error: " << ex.what() << endl;
        exit(1);
    } catch (...) {
        cerr << "Unknown error occurred while retrieving configuration value." << endl;
        exit(1);
    }
}
