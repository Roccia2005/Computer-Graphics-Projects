#pragma once
#include "Config.h"
#include <glm/glm.hpp>
#include <array>
#include <cstddef>
#include <optional>
#include <vector>

struct PlatformDefinition {
    glm::vec2 basePosition;
    glm::vec2 size;
};

struct PressurePlateDefinition {
    glm::vec2 basePosition;
    float width;
    float height;
    float pressDistance;
};

struct MovingWallDefinition {
    glm::vec2 closedCenter;
    glm::vec2 openCenter;
    glm::vec2 size;
    float travelSeconds;
    std::vector<std::size_t> pressurePlateIndices;
};

struct EnemyDefinition {
    glm::vec2 startCenter;
    glm::vec2 endCenter;
    glm::vec2 size;
    float speed;
};

struct CrateDefinition {
    glm::vec2 basePosition;
    glm::vec2 size;
};

struct LevelDefinition {
    glm::vec2 roomHalfSize;
    std::array<glm::vec2, 2> spawnFeet;
    glm::vec2 doorBase;
    std::vector<PlatformDefinition> platforms;
    std::vector<glm::vec2> stars;
    std::vector<PressurePlateDefinition> pressurePlates;
    float floorY = GameConfig::DEFAULT_FLOOR_Y;
    std::vector<MovingWallDefinition> movingWalls{};
    std::vector<EnemyDefinition> enemies{};
    std::vector<CrateDefinition> crates{};
    bool requireAllStarsToExit = true;
    std::optional<float> timeLimitSeconds = std::nullopt;
};
