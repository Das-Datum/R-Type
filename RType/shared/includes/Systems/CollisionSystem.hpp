#pragma once
#include "ECS.hpp"
#include "Components/GameComponents.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <vector>
#include <unordered_map>
#include <algorithm>

/**
 * @class CollisionSystem
 * @brief System for handling collision detection and response.
 */
class CollisionSystem : public System {
private:
    struct AABB {
        Vector2 min;
        Vector2 max;
    };

    struct GridCell {
        std::vector<Entity> entities;
    };

    const float WORLD_WIDTH = 1920.0f;
    const float WORLD_HEIGHT = 1080.0f;
    float cellSize = 100.0f;
    std::unordered_map<int64_t, GridCell> grid;
    std::unordered_map<Entity, std::vector<Vector2>> polygonCache;

    /**
     * @brief Get the key for a grid cell based on its coordinates.
     * @param cellX X coordinate of the cell
     * @param cellY Y coordinate of the cell
     * @return Key for the grid cell
     */
    int64_t getGridKey(int cellX, int cellY) {
        return (static_cast<int64_t>(cellX) << 32) | static_cast<int64_t>(cellY);
    }

public:
    /**
     * @brief Update the collision system.
     * @param onCollision Callback function to handle collisions
     */
    void update(std::function<void(Entity, Entity)> onCollision = nullptr) {
        std::vector<Entity> entityList(entities.begin(), entities.end());
        updateGrid(entityList);
        checkCollisions(onCollision);
        polygonCache.clear();
    }

    /**
     * @brief Draw the spatial grid for debugging purposes.
     *
     * @param viewportX X coordinate of the viewport
     * @param viewportY Y coordinate of the viewport
     * @param viewportWidth Width of the viewport
     * @param viewportHeight Height of the viewport
     */
    void drawGrid(float viewportX, float viewportY, float viewportWidth, float viewportHeight) {
        float scaleX = viewportWidth / WORLD_WIDTH;
        float scaleY = viewportHeight / WORLD_HEIGHT;

        for (float x = 0; x <= WORLD_WIDTH; x += cellSize) {
            float viewX = viewportX + (x * scaleX);
            DrawLine(
                static_cast<int>(viewX),
                static_cast<int>(viewportY),
                static_cast<int>(viewX),
                static_cast<int>(viewportY + viewportHeight),
                GREEN
            );
        }

        for (float y = 0; y <= WORLD_HEIGHT; y += cellSize) {
            float viewY = viewportY + (y * scaleY);
            DrawLine(
                static_cast<int>(viewportX),
                static_cast<int>(viewY),
                static_cast<int>(viewportX + viewportWidth),
                static_cast<int>(viewY),
                GREEN
            );
        }

        for (const auto& pair : grid) {
            int64_t key = pair.first;
            int cellX = static_cast<int>(key >> 32);
            int cellY = static_cast<int>(key & 0xFFFFFFFF);

            float viewX = viewportX + ((cellX * cellSize) * scaleX);
            float viewY = viewportY + ((cellY * cellSize) * scaleY);

            if (!pair.second.entities.empty()) {
                DrawText(
                    TextFormat("%d", static_cast<int>(pair.second.entities.size())),
                    static_cast<int>(viewX + 5),
                    static_cast<int>(viewY + 5),
                    20,
                    RED
                );
            }
        }
    }

private:
    /**
     * @brief Calculate the optimal size for grid cells based on entity sizes.
     * @param entityList List of entities to consider
     */
    void calculateOptimalCellSize(const std::vector<Entity>& entityList) {
        if (entityList.empty()) return;
        float totalWidth = 0.0f;
        float totalHeight = 0.0f;
        int count = 0;

        for (auto entity : entityList) {
            if (!gCoordinator.hasComponent<CollisionComponent>(entity)) continue;
            if (gCoordinator.hasComponent<SpawnComponent>(entity)) continue;
            auto& collider = gCoordinator.getComponent<CollisionComponent>(entity);
            totalWidth += collider.collider.width * WORLD_WIDTH;
            totalHeight += collider.collider.height * WORLD_HEIGHT;
            count++;
        }

        if (count == 0) return;

        float avgSize = (totalWidth + totalHeight) / (2.0f * count);
        cellSize = avgSize * 2.0f;
        float minCellSize = std::min(WORLD_WIDTH, WORLD_HEIGHT) / 20.0f;
        float maxCellSize = std::min(WORLD_WIDTH, WORLD_HEIGHT) / 4.0f;
        cellSize = std::clamp(cellSize, minCellSize, maxCellSize);
    }

    /**
     * @brief Update the spatial grid with the current entities.
     * @param entityList List of entities to include in the grid
     */
    void updateGrid(const std::vector<Entity>& entityList) {
        grid.clear();
        calculateOptimalCellSize(entityList);

        for (auto entity : entityList) {
            if (gCoordinator.hasComponent<SpawnComponent>(entity)) continue;

            auto& transform = gCoordinator.getComponent<TransformComponent>(entity);
            auto& collider = gCoordinator.getComponent<CollisionComponent>(entity);

            AABB bounds = getAABB(transform, collider);

            int startCellX = static_cast<int>(bounds.min.x * WORLD_WIDTH / cellSize);
            int startCellY = static_cast<int>(bounds.min.y * WORLD_HEIGHT / cellSize);
            int endCellX = static_cast<int>(bounds.max.x * WORLD_WIDTH / cellSize);
            int endCellY = static_cast<int>(bounds.max.y * WORLD_HEIGHT / cellSize);

            std::set<int64_t> uniqueCells;
            for (int x = startCellX; x <= endCellX; x++) {
                for (int y = startCellY; y <= endCellY; y++) {
                    int64_t cellKey = getGridKey(x, y);
                    if (uniqueCells.find(cellKey) == uniqueCells.end()) {
                        grid[cellKey].entities.push_back(entity);
                        uniqueCells.insert(cellKey);
                    }
                }
            }
        }
    }

    /**
     * @brief Get the axis-aligned bounding box (AABB) for an entity.
     * @param transform Transform component of the entity
     * @param collider Collision component of the entity
     * @return AABB of the entity
     */
    AABB getAABB(const TransformComponent& transform, const CollisionComponent& collider) {
        float halfW = collider.collider.width * 0.5f;
        float halfH = collider.collider.height * 0.5f;

        return {
            {transform.position.x - halfW,
             transform.position.y - halfH},
            {transform.position.x + halfW,
             transform.position.y + halfH}
        };
    }

    /**
     * @brief Check if two AABBs overlap.
     * @param a First AABB
     * @param b Second AABB
     * @return True if the AABBs overlap, false otherwise
     */
    bool checkAABBOverlap(const AABB& a, const AABB& b) {
        return (a.min.x <= b.max.x && a.max.x >= b.min.x &&
                a.min.y <= b.max.y && a.max.y >= b.min.y);
    }

    /**
     * @brief Check for collisions between entities in the grid.
     * @param onCollision Callback function to handle collisions
     */
    void checkCollisions(const std::function<void(Entity, Entity)>& onCollision) {
        std::set<std::pair<Entity, Entity>> checkedPairs;
        std::vector<std::pair<Entity, Entity>> collisions;

        for (const auto& pair : grid) {
            const auto& cell = pair.second;
            for (size_t i = 0; i < cell.entities.size(); i++) {
                for (size_t j = i + 1; j < cell.entities.size(); j++) {
                    Entity entityA = cell.entities[i];
                    Entity entityB = cell.entities[j];

                    if (checkedPairs.find({entityA, entityB}) != checkedPairs.end() ||
                        checkedPairs.find({entityB, entityA}) != checkedPairs.end()) {
                        continue;
                    }

                    checkedPairs.insert({entityA, entityB});

                    auto& transformA = gCoordinator.getComponent<TransformComponent>(entityA);
                    auto& colliderA = gCoordinator.getComponent<CollisionComponent>(entityA);
                    auto& transformB = gCoordinator.getComponent<TransformComponent>(entityB);
                    auto& colliderB = gCoordinator.getComponent<CollisionComponent>(entityB);

                    if (!checkAABBOverlap(getAABB(transformA, colliderA),
                                        getAABB(transformB, colliderB))) {
                        continue;
                    }

                    auto& polyA = getOrCreatePolygon(entityA, transformA, colliderA);
                    auto& polyB = getOrCreatePolygon(entityB, transformB, colliderB);

                    if (checkPolygonCollision(polyA, polyB)) {
                        collisions.push_back({entityA, entityB});
                    }
                }
            }
        }

        if (onCollision) {
            for (const auto& collision : collisions) {
                onCollision(collision.first, collision.second);
            }
        }
    }

    /**
     * @brief Get or create the transformed polygon for an entity.
     * @param entity Entity to get the polygon for
     * @param transform Transform component of the entity
     * @param collider Collision component of the entity
     * @return Transformed polygon of the entity
     */
    const std::vector<Vector2>& getOrCreatePolygon(Entity entity,
                                                  const TransformComponent& transform,
                                                  const CollisionComponent& collider) {
        auto it = polygonCache.find(entity);
        if (it != polygonCache.end()) {
            return it->second;
        }
        return polygonCache[entity] = getPolygon(transform, collider);
    }

    /**
     * @brief Get the transformed polygon for an entity.
     * @param transform Transform component of the entity
     * @param collider Collision component of the entity
     * @return Transformed polygon of the entity
     */
    std::vector<Vector2> getPolygon(const TransformComponent &transform, const CollisionComponent &collider) {
        Vector2 pivot = transform.position;

        float w = collider.collider.width;
        float h = collider.collider.height;
        float centerX = pivot.x + collider.collider.x;
        float centerY = pivot.y + collider.collider.y;
        float halfW = w * 0.5f;
        float halfH = h * 0.5f;

        Vector2 corners[4] = {
            {centerX - halfW, centerY - halfH},
            {centerX + halfW, centerY - halfH},
            {centerX + halfW, centerY + halfH},
            {centerX - halfW, centerY + halfH}
        };

        float angle = collider.rotation * (PI / 180.0f);
        float cosA = cosf(angle);
        float sinA = sinf(angle);

        std::vector<Vector2> polygon(4);
        for (int i = 0; i < 4; i++) {
            float rx = corners[i].x - pivot.x;
            float ry = corners[i].y - pivot.y;
            float rotatedX = rx * cosA - ry * sinA;
            float rotatedY = rx * sinA + ry * cosA;

            polygon[i] = {pivot.x + rotatedX, pivot.y + rotatedY};
        }

        return polygon;
    }

    /**
     * @brief Check for collision between two polygons using the Separating Axis Theorem (SAT).
     * @param polyA First polygon
     * @param polyB Second polygon
     * @return True if the polygons collide, false otherwise
     */
    bool checkPolygonCollision(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB) {
        return satCheck(polyA, polyB) && satCheck(polyB, polyA);
    }

    /**
     * @brief Perform the SAT check for one polygon against another.
     * @param polyA First polygon
     * @param polyB Second polygon
     * @return True if there is no separating axis, false otherwise
     */
    bool satCheck(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB) {
        for (size_t i = 0; i < polyA.size(); i++) {
            Vector2 p1 = polyA[i];
            Vector2 p2 = polyA[(i + 1) % polyA.size()];

            Vector2 edge = Vector2Subtract(p2, p1);
            Vector2 axis = {-edge.y, edge.x};

            float length = Vector2Length(axis);
            if (length != 0) {
                axis.x /= length;
                axis.y /= length;
            }

            float minA, maxA, minB, maxB;
            projectPolygon(axis, polyA, minA, maxA);
            projectPolygon(axis, polyB, minB, maxB);

            if (maxA < minB || maxB < minA) {
                return false;
            }
        }
        return true;
    }

    /**
     * @brief Project a polygon onto an axis and get the minimum and maximum values.
     * @param axis Axis to project onto
     * @param poly Polygon to project
     * @param minVal Minimum value of the projection
     * @param maxVal Maximum value of the projection
     */
    void projectPolygon(const Vector2 &axis, const std::vector<Vector2> &poly, float &minVal, float &maxVal) {
        minVal = maxVal = Vector2DotProduct(poly[0], axis);
        for (size_t i = 1; i < poly.size(); i++) {
            float proj = Vector2DotProduct(poly[i], axis);
            if (proj < minVal)
                minVal = proj;
            if (proj > maxVal)
                maxVal = proj;
        }
    }
};
