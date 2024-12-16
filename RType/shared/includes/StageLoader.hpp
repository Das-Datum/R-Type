#pragma once
#include <iostream>
#include <map>
#include <nlohmann/json.hpp>
#include <math.h>
#include "AEntitiesManager.hpp"

struct StageConfig {
    int numero;
    std::string background_path;
    std::string music_path;
    int time;
    std::vector<std::string> mobs_types;
};

class StageLoader {
    #define WAVE_TIME_INTERVAL 3.0f
    #define WAVE_MIN_DURATION 5.0f
    #define WAVE_MAX_DURATION 20.0f

    public:
        static StageLoader& getInstance() {
            static StageLoader instance;
            return instance;
        }

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
        void loadConfig(const std::string& config_filepath) {
            this->_config_filepath = config_filepath;
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
            _remainingTime = static_cast<double>(_config.time);
            _seed = j["seed"];
            for (auto type : j["mobs_types"]) {
                if (this->isMobTypeValid(type)) {
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
            _waveCount = _config.time / 15 + this->lcg<int>(-2, 3);

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
        void genMobsEntities(AEntitiesManager& manager) {
            for (std::size_t i = 0; i < _waveCount; i++) {
                std::size_t waveMobsCount = _wavesMobsCount[i];
                int enemiesTotal = 0;
                for (std::size_t j = 0; j < i; j++) enemiesTotal += _wavesMobsCount[j];

                float wave_beginning = 0.0f;
                for (std::size_t w = 0; w < i; w++) wave_beginning += static_cast<float>(_wavesDurations[w]) + WAVE_TIME_INTERVAL;

                float wave_ending = wave_beginning + static_cast<float>(_wavesDurations[i]);

                for (std::size_t m = 0; m < waveMobsCount; m++) {
                    float enemyPosX = 0.9;
                    float enemyPosY = this->lcg<float>(0.1, 0.9);
                    float enemySpawnTime = this->lcg<double>(wave_beginning, wave_ending);
                    int typeIndex = _wavesMobsTypes[i][(this->lcg<int>(0, _wavesMobsTypes[i].size()))];
                    EnemyType enemyType = this->getEnemyTypeByName(_config.mobs_types[typeIndex]);

                    bool shootAtPlayer = false;
                    if (enemyType.isShooting) {
                        shootAtPlayer = this->lcg<int>(0, 2);
                    }

                    BehaviorType enemyMovementBehavior = BehaviorType::None;
                    int hasEnemySpecificMove = this->lcg<int>(0, 2);
                    if (hasEnemySpecificMove == 1) {
                        enemyMovementBehavior = this->genRandomMovementBehavior();
                    }

                    Enemy newEnemy(enemyPosX, enemyPosY, enemySpawnTime, enemyType, shootAtPlayer, hasEnemySpecificMove, enemyMovementBehavior);
                    newEnemy.setUniqueId(enemiesTotal + m);
                    manager.createEnemy(newEnemy);
                }
                std::cout << std::endl;
            }
        };

        void resetConfig() {
            this->_config_filepath = "";
            this->_config = StageConfig();
            this->_waveCount = 0;
            this->_wavesDurations.clear();
            this->_wavesMobsCount.clear();
            this->_wavesMobsTypes.clear();
        }

        // Getters
        StageConfig getStageConfig() const { return _config; };
        std::size_t getWaveCount() const { return _waveCount; };
        std::vector<float> getWavesDurations() const { return _wavesDurations; };
        std::vector<std::size_t> getWavesMobsCount() const { return _wavesMobsCount; };
        std::map<std::size_t, std::vector<std::size_t>> getWavesMobsTypes() const { return _wavesMobsTypes; };

        double getRemainingTime() const { return _remainingTime; };
        void setRemainingTime(double time) { _remainingTime = time; };

        // Displayers
        void printStageConfig() const {
            std::cout << " --- Stage Config --- " << std::endl;

            std::cout << "Numero: " << _config.numero << std::endl;
            std::cout << "Seed: " << _seed << std::endl;
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
        StageLoader() {
            initMobsTypes();
        }
        ~StageLoader() = default;

        std::string _config_filepath;
        StageConfig _config;

        std::vector<EnemyType> _enemyTypes;

        double _remainingTime;

        std::size_t _seed;

        // waves attributs
        std::size_t _waveCount;
        std::vector<float> _wavesDurations;
        std::vector<std::size_t> _wavesMobsCount;
        std::map<std::size_t, std::vector<std::size_t>> _wavesMobsTypes;

        /**
         * @brief Linear Congruential Generator
         *
         * @tparam T
         * @param min
         * @param max
         * @param a
         * @param c
         * @param m
         *
         * @return T
         */
        template <typename T>
        T lcg(double min = 0, double max = 1, int a = 1664525, int c = 1013904223, int m = pow(2, 8)) {
            _seed = (a * _seed + c) % m;
            T scaled_value = min + (_seed * (max - min) / (m - 1.0));
            return scaled_value;
        }

        /**
         * @brief Get the Enemy Type By Name object
         *
         * @param name
         * @return EnemyType
         */
        EnemyType getEnemyTypeByName(const std::string& name) {
            for (const EnemyType& type : _enemyTypes) {
                if (type.name == name)
                    return type;
            }
            std::cerr << "getEnemyTypeByName(): Cannot find enemy type of name " << name << std::endl;
            return _enemyTypes[0];
        }

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
            for (const EnemyType& validType : _enemyTypes) {
                if (type == validType.name) {
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
                float random_duration = (this->lcg<int>(0, 4)) + 1;
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
                const std::size_t countMobs = this->lcg<int>(waveDuration, waveDuration * 2);
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
                std::size_t nbMobsTypesForWave = (this->lcg<int>(0, _config.mobs_types.size())) + 1;
                std::vector<std::string> remainingTypeToChoose = _config.mobs_types;

                for (std::size_t j = 0; j < nbMobsTypesForWave; j++) {
                    std::size_t randomIndex = this->lcg<int>(0, remainingTypeToChoose.size());
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

        /**
         * @brief Initialize pre-defined mob classes
         *
         * @param void
         *
         * @return void
         */
        void initMobsTypes() {
            _enemyTypes.push_back(EnemyType("asteroid", Vector2{35.0, 37.0}, 1, 300, false, true, 48.0f, 0.0f));
            _enemyTypes.push_back(EnemyType("classic", Vector2{266.0, 36.0}, 8, 100, true, true, 50.0f, 0.0f));
            _enemyTypes.push_back(EnemyType("ship", Vector2{145.0, 29.0}, 5, 100, true, true, 55.0f, 0.0f));
        }

        /**
         * @brief Generate a random float number
         *
         * @param min
         * @param max
         * @return float
         */
        // float genRandomFloat(float min, float max) {
        //     float r = static_cast<float>(rand()) / RAND_MAX;
        //     float value = min + r * (max - min);
        //     return value;
        // }

        /**
         * @brief Generate a random movement behavior
         *
         * @param void
         * @return BehaviorType
         */
        BehaviorType genRandomMovementBehavior() {
            int choice = this->lcg<int>(0, 4);
            switch (choice)
            {
            case 0:
                return BehaviorType::ChasePlayer;
            case 1:
                return BehaviorType::FleeFromPlayer;
            case 2:
                return BehaviorType::Patrol;
            default:
                return BehaviorType::RandomMovement;
            }
        }
};