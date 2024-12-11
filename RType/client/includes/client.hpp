#pragma once

#include "raylib.h"
#include "rlgl.h"
#include "raylib.h"
#include "raymath.h"

#include "ECS.hpp"
#include "Components.hpp"
#include "TexturesManager.hpp"
#include "ClientEntitiesManager.hpp"

#include "Menus.hpp"
#include "ShadersManager.hpp"

#include "Systems/ClientRelatives.hpp"
#include "Systems/EnemiesRelatives.hpp"
#include "Systems/SpriteRelatives.hpp"
#include "Systems/Generics.hpp"
#include "Systems/BackgroundScrollSystem.hpp"
#include "Systems/ClientManageNetworkSystem.hpp"

#include "Components/GameComponents.hpp"
#include "Systems/Game.hpp"
#include "Systems/CollisionSystem.hpp"

#include "NetworkComponent.hpp"
#include "NetworkSystems.hpp"

void initCoordinator();
void initMenus(std::shared_ptr<MenuManager> manager, int windowWidth, int windowHeight);
