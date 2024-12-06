#pragma once
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include "../../../ECS/includes/ECS.hpp"

struct stageConfig {
    int numero;
    std::string background_path;
    std::string music_path;
    int time;
    std::vector<std::string> mobs_types;
};

struct Mob {
    float x;
    float y;
    int typeIndex;
    float apparition_time;
};

class StageLoader {
    #define WAVE_TIME_INTERVAL 3.0f
    #define WAVE_MIN_DURATION 5.0f
    #define WAVE_MAX_DURATION 20.0f
    #define VALID_MOBS_TYPES std::vector<std::string>{"asteroid", "classic", "ship"}
    #define MAX_WIDTH 1920
    #define MAX_HEIGHT 1080

    public:
        StageLoader(const std::string& config_filepath, std::size_t seed) : _config_filepath(config_filepath) {
            std::srand(seed);
        }
        ~StageLoader() = default;

        /**
         * @brief Load the config file and store it in the _config attribute
         *
         * @param void
         *
         * @throw std::runtime_error if the file can't be opened
         * @throw std::runtime_error if no valid mob type is found
         *
         * @return void
         */
        void loadConfig() {
            std::cout << "Loading config file: " << _config_filepath << std::endl;
            std::ifstream file(_config_filepath);

            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << _config_filepath << std::endl;
                throw std::runtime_error("Failed to open file");
            }

            nlohmann::json j;
            file >> j;

            _config.numero = j["numero"];
            _config.background_path = j["background_path"];
            _config.music_path = j["music_path"];
            _config.time = j["time"];
            for (auto type : j["mobs_types"]) {
                if (isMobTypeValid(type)) {
                    _config.mobs_types.push_back(type);
                } else {
                    std::cerr << "Invalid mob type: " << type << std::endl;
                }
            }
            if (_config.mobs_types.size() == 0) {
                throw std::runtime_error("No valid mob type found");
            }
        };

        /**
         * @brief Generate waves of mobs
         *
         * @param void
         *
         * @return void
         */
        void genWaves() {
            _waveCount = _config.time / 15 + (rand() % 5 - 2);

            this->defineWavesDurations();
            this->defineWavesMobsCount();
            this->defineWavesMobsTypes();
        };

        /**
         * @brief Generate mobs entities
         *
         * @param gCoordinator ECS Coordinator
         *
         * @return void
         */
        void genMobsEntities(Coordinator& gCoordinator) {
            for (std::size_t i = 0; i < _waveCount; i++) {
                std::size_t waveMobsCount = _wavesMobsCount[i];
                std::cout << "Gen mobs for wave " << i << " ..." << std::endl;
                std::cout << "Wave contains " << _wavesMobsCount[i] << " mobs of type with index: ";
                for (auto type : _wavesMobsTypes[i]) {
                    std::cout << type << " / ";
                }
                std::cout << std::endl;
                float wave_beginning = 0.0f;
                for (std::size_t w = 0; w < i; w++) wave_beginning += static_cast<float>(_wavesDurations[w]) + WAVE_TIME_INTERVAL;
                float wave_ending = wave_beginning + static_cast<float>(_wavesDurations[i]);
                std::cout << "Wave begin last from " << wave_beginning << " to " << wave_ending << std::endl;
                for (std::size_t m = 0; m < waveMobsCount; m++) {
                    Mob newMob;
                    newMob.x = MAX_WIDTH;
                    newMob.y = (rand() % (MAX_HEIGHT - 20)) + 10;
                    newMob.typeIndex = _wavesMobsTypes[i][(rand() % _wavesMobsTypes[i].size())];

                    // Gen apparition time for mob
                    float r = static_cast<float>(rand()) / RAND_MAX;
                    newMob.apparition_time = wave_beginning + r * (wave_ending - wave_beginning);
                    std::cout << "\tMOB " << m << ": " << "( " << newMob.x << " ; " << newMob.y << " ) -- Type: " << newMob.typeIndex << " -- Apparition time: " << newMob.apparition_time << std::endl;
                }
                std::cout << std::endl;
            }
        };


        // Getters
        stageConfig getStageConfig() const { return _config; };
        std::size_t getWaveCount() const { return _waveCount; };
        std::vector<float> getWavesDurations() const { return _wavesDurations; };
        std::vector<std::size_t> getWavesMobsCount() const { return _wavesMobsCount; };
        std::map<std::size_t, std::vector<std::size_t>> getWavesMobsTypes() const { return _wavesMobsTypes; };

        // Displayers
        void printStageConfig() const {
            std::cout << " --- Stage Config --- " << std::endl;

            std::cout << "Numero: " << _config.numero << std::endl;
            std::cout << "Background Path: " << _config.background_path << std::endl;
            std::cout << "Music Path: " << _config.music_path << std::endl;
            std::cout << "Time: " << _config.time << std::endl;

            std::cout << " --- END --- " << std::endl;
        };
        void printWavesDurations() const {
            std::cout << "Waves durations: ";
            for (auto duration : _wavesDurations) {
                std::cout << duration << " / ";
            }
            std::cout << std::endl;
        };
        void printWavesMobsCount() const {
            std::cout << "Waves mobs count: ";
            for (auto mobsCount : _wavesMobsCount) {
                std::cout << mobsCount << " / ";
            }
            std::cout << std::endl;
        };
        void printWavesMobsTypes() const {
            std::cout << "Waves mobs types: " << std::endl;
            for (auto mobsTypes : _wavesMobsTypes) {
                std::cout << mobsTypes.first << " -> ";
                for (auto type : mobsTypes.second) {
                    std::cout << type << " / ";
                }
                std::cout << std::endl;
            }
        };

    private:
        std::string _config_filepath;
        stageConfig _config;

        // waves attributs
        std::size_t _waveCount;
        std::vector<float> _wavesDurations;
        std::vector<std::size_t> _wavesMobsCount;
        std::map<std::size_t, std::vector<std::size_t>> _wavesMobsTypes;

        /**
         * @brief Get the Sum Waves Durations object
         *
         * @return float
         */
        float getSumWavesDurations() const {
            float sum = 0;
            for (auto duration : _wavesDurations) {
                sum += duration;
            }
            return sum;
        };

        /**
         * @brief Get the Sum Waves Interval object
         *
         * @return float
         */
        float getSumWavesInterval() const {
            return _waveCount * WAVE_TIME_INTERVAL;
        };

        /**
         * @brief
         *
         * @param type
         * @return true
         * @return false
         */
        const bool isMobTypeValid(const std::string& type) const {
            for (const auto& validType : VALID_MOBS_TYPES) {
                if (type == validType) {
                    return true;
                }
            }
            return false;
        }

        /**
         * @brief Define waves durations
         *
         * @param void
         *
         * @return void
         */
        void defineWavesDurations() {
            // init duration with constant values
            for (std::size_t i = 0; i < _waveCount; i++) {
                _wavesDurations.push_back((_config.time - this->getSumWavesInterval()) / _waveCount);
            }
            // regulate duration with random values
            for (std::size_t i = 0; i < _waveCount; i++) {
                float random_duration = (rand() % 4) + 1;
                _wavesDurations[_waveCount - i - 1] += random_duration;
                _wavesDurations[i] -= random_duration;
            }
            // check if durations fill the whole time of the stage
            if (this->getSumWavesDurations() + this->getSumWavesInterval() != _config.time)
                std::cerr << "Waves durations are incorrect: " << this->getSumWavesDurations() + this->getSumWavesInterval() << " > " << _config.time << std::endl;
        }

        /**
         * @brief Define waves mobs count
         *
         * @param void
         *
         * @return void
         */
        void defineWavesMobsCount() {
            for (std::size_t i = 0; i < _waveCount; i++) {
                const std::size_t waveDuration = _wavesDurations[i];
                const std::size_t countMobs = (rand() % (waveDuration * 3 - waveDuration)) + waveDuration;
                _wavesMobsCount.push_back(countMobs);
            }
        }

        /**
         * @brief Define waves mobs types
         *
         * @param void
         *
         * @return void
         */
        void defineWavesMobsTypes() {
            for (std::size_t i = 0; i < _waveCount; i++) {
                std::vector<std::size_t> mobsTypes;
                std::size_t nbMobsTypesForWave = (rand() % _config.mobs_types.size()) + 1;
                std::vector<std::string> remainingTypeToChoose = _config.mobs_types;

                for (std::size_t j = 0; j < nbMobsTypesForWave; j++) {
                    std::size_t randomIndex = rand() % remainingTypeToChoose.size();
                    std::string type = remainingTypeToChoose[randomIndex];

                    remainingTypeToChoose.erase(remainingTypeToChoose.begin() + randomIndex);
                    auto it = std::find(_config.mobs_types.begin(), _config.mobs_types.end(), type);
                    if (it != _config.mobs_types.end()) {
                        std::size_t index = std::distance(_config.mobs_types.begin(), it);
                        mobsTypes.push_back(index);
                    }
                }

                _wavesMobsTypes[i] = mobsTypes;
            }
        }
};