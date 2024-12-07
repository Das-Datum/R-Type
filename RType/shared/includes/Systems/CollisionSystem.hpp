#pragma once
#include "../../../../ECS/includes/ECS.hpp"
#include "../Components/GameComponents.hpp"
#include "raylib.h"
#include "raymath.h"
#include <cmath>
#include <vector>

class CollisionSystem : public System {
  public:
    void update() {
        std::vector<Entity> entityList(entities.begin(), entities.end());

        for (size_t i = 0; i < entityList.size(); ++i) {
            for (size_t j = i + 1; j < entityList.size(); ++j) {
                Entity entityA = entityList[i];
                auto &transformA = gCoordinator.getComponent<TransformComponent>(entityA);
                auto &colliderA = gCoordinator.getComponent<CollisionComponent>(entityA);
                std::vector<Vector2> polyA = getPolygon(transformA, colliderA);

                Entity entityB = entityList[j];
                auto &transformB = gCoordinator.getComponent<TransformComponent>(entityB);
                auto &colliderB = gCoordinator.getComponent<CollisionComponent>(entityB);
                std::vector<Vector2> polyB = getPolygon(transformB, colliderB);

                if (checkPolygonCollision(polyA, polyB)) {
                    printf("Collision detected between entities %d and %d\n", entityA, entityB);
                }
            }
        }
    }

  private:
    std::vector<Vector2> getPolygon(const TransformComponent &transform, const CollisionComponent &collider) {
        Vector2 pivot = transform.position;

        float w = collider.collider.width;
        float h = collider.collider.height;
        float centerX = pivot.x + collider.collider.x;
        float centerY = pivot.y + collider.collider.y;
        float halfW = w * 0.5f;
        float halfH = h * 0.5f;

        Vector2 corners[4] = {
            {centerX - halfW, centerY - halfH}, // topLeft
            {centerX + halfW, centerY - halfH}, // topRight
            {centerX + halfW, centerY + halfH}, // bottomRight
            {centerX - halfW, centerY + halfH}  // bottomLeft
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

    bool checkPolygonCollision(const std::vector<Vector2> &polyA, const std::vector<Vector2> &polyB) {
        return satCheck(polyA, polyB) && satCheck(polyB, polyA);
    }

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
