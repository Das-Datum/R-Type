#include "ECS.hpp"

#include "NetworkComponent.hpp"
#include "NetworkSystems.hpp"
#include "StageLoader.hpp"
#include "ServerEntitiesManager.hpp"

#include "Systems/Game.hpp"
#include "Systems/ServerManageNetworkSystem.hpp"
#include "Systems/CollisionSystem.hpp"
#include "Systems/VelocitySystem.hpp"

#include <string>

void initCoordinator();
