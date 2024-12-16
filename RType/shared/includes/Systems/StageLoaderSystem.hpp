#pragma once
#include "ECS.hpp"
#include "StageLoader.hpp"

class StageLoaderSystem : public System {
    public:
        void update(double frameTime) {
            auto &loader = StageLoader::getInstance();
            double remainingTime = loader.getRemainingTime();
            if (remainingTime <= 0) {
                loader.resetConfig();
                // loadConfig("config/stage1.json", gCoordinator.getSystem<AEntitiesManager>());
            }
            loader.setRemainingTime(remainingTime - frameTime);
        }

        void loadConfig(const std::string& config_filepath, AEntitiesManager& manager) {
            auto &loader = StageLoader::getInstance();
            loader.loadConfig(config_filepath);
            loader.genWaves();
            loader.genMobsEntities(manager);
        }
};