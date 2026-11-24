#include "LevelCatalog.h"
#include "Config.h"
#include "LevelDefinition.h"
#include <initializer_list>
#include <iterator>
#include <stdexcept>

namespace {

namespace LevelConfig {

struct PlatformBounds {
    float left;
    float right;
    float top;
    float thickness;
};

struct StarPosition {
    float x;
    float y;
};

namespace Level1 {

inline constexpr float ROOM_HALF_WIDTH = 120.0f;
inline constexpr float ROOM_HALF_HEIGHT = 76.0f;
inline constexpr float FLOOR_Y = GameConfig::DEFAULT_FLOOR_Y;
inline constexpr float MAIN_ROUTE_X_OFFSET = -20.0f;

inline constexpr float PLAYER_1_SPAWN_X = -92.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float PLAYER_1_SPAWN_FEET_Y = FLOOR_Y;
inline constexpr float PLAYER_2_SPAWN_X = -82.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float PLAYER_2_SPAWN_FEET_Y = FLOOR_Y;

inline constexpr float PLATFORM_3_TOP_Y = -10.0f;
inline constexpr float PLATFORM_4_TOP_Y = 5.0f;
inline constexpr float PLATFORM_5_TOP_Y = 22.0f;

inline constexpr float DOOR_X = 71.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float DOOR_BASE_Y = PLATFORM_5_TOP_Y;

inline constexpr float UPPER_WALL_1_HEIGHT = 14.0f;
inline constexpr float UPPER_WALL_2_HEIGHT = 25.0f;
inline constexpr float UPPER_WALL_3_X = 29.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float UPPER_WALL_3_HEIGHT = 36.0f;
inline constexpr float UPPER_CRATE_X = 2.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float UPPER_CRATE_WIDTH = 8.0f;
inline constexpr float UPPER_CRATE_HEIGHT = 6.0f;

inline constexpr float EXIT_CRATE_X = (UPPER_WALL_3_X + DOOR_X) * 0.5f;
inline constexpr float EXIT_CRATE_HEIGHT = UPPER_WALL_3_HEIGHT * 0.5f;

inline constexpr float BONUS_WALL_RIGHT_X = 86.0f;
inline constexpr float BONUS_WALL_WIDTH = 10.0f;
inline constexpr float BONUS_WALL_BASE_Y = -24.0f;
inline constexpr float BONUS_WALL_TOP_Y = 100.0f;
inline constexpr float BONUS_STEP_WIDTH = 10.0f;
inline constexpr float BONUS_STEP_THICKNESS = 3.0f;
inline constexpr float BONUS_FIRST_STEP_TOP_Y = FLOOR_Y + 35.0f;
inline constexpr float BONUS_STEP_RISE = 13.0f;

/**
 * @param step Indice del gradino nella salita bonus.
 * @return Bordi, quota e spessore del gradino.
 *
 * Alterna i gradini della salita bonus tra i due lati del passaggio.
 */
constexpr PlatformBounds bonusStep(std::size_t step)
{
    const float left = step % 2 == 0 ? BONUS_WALL_RIGHT_X : ROOM_HALF_WIDTH - BONUS_STEP_WIDTH;
    const float top = BONUS_FIRST_STEP_TOP_Y + static_cast<float>(step) * BONUS_STEP_RISE;
    return {left, left + BONUS_STEP_WIDTH, top, BONUS_STEP_THICKNESS};
}

inline constexpr PlatformBounds BONUS_LAST_STEP = bonusStep(5);

inline constexpr PlatformBounds PLATFORMS[] = {
    {-25.0f + MAIN_ROUTE_X_OFFSET, -5.0f + MAIN_ROUTE_X_OFFSET, -19.0f, 6.0f},
    {-2.0f + MAIN_ROUTE_X_OFFSET, 16.0f + MAIN_ROUTE_X_OFFSET, -17.0f, 7.0f},
    {21.0f + MAIN_ROUTE_X_OFFSET, 96.0f + MAIN_ROUTE_X_OFFSET, PLATFORM_3_TOP_Y, 12.0f},
    {-100.0f + MAIN_ROUTE_X_OFFSET, -66.0f + MAIN_ROUTE_X_OFFSET, PLATFORM_4_TOP_Y, 6.0f},
    {-47.0f + MAIN_ROUTE_X_OFFSET, 100.0f + MAIN_ROUTE_X_OFFSET, PLATFORM_5_TOP_Y, 5.0f},
    {-30.0f + MAIN_ROUTE_X_OFFSET, -22.0f + MAIN_ROUTE_X_OFFSET,
     PLATFORM_5_TOP_Y + UPPER_WALL_1_HEIGHT, UPPER_WALL_1_HEIGHT},
    {-14.0f + MAIN_ROUTE_X_OFFSET, -4.0f + MAIN_ROUTE_X_OFFSET,
     PLATFORM_5_TOP_Y + UPPER_WALL_2_HEIGHT, UPPER_WALL_2_HEIGHT},
    {UPPER_WALL_3_X - 5.0f, UPPER_WALL_3_X + 5.0f,
     PLATFORM_5_TOP_Y + UPPER_WALL_3_HEIGHT, UPPER_WALL_3_HEIGHT},
    {BONUS_WALL_RIGHT_X - BONUS_WALL_WIDTH, BONUS_WALL_RIGHT_X,
     BONUS_WALL_TOP_Y, BONUS_WALL_TOP_Y - BONUS_WALL_BASE_Y},
    bonusStep(0), bonusStep(1), bonusStep(2), bonusStep(3), bonusStep(4), BONUS_LAST_STEP
};

inline constexpr std::initializer_list<StarPosition> STARS = {
    {18.0f + MAIN_ROUTE_X_OFFSET, -40.0f},
    {-17.0f + MAIN_ROUTE_X_OFFSET, -11.0f},
    {9.0f + MAIN_ROUTE_X_OFFSET, -10.0f},
    {63.0f + MAIN_ROUTE_X_OFFSET, -2.0f},
    {UPPER_WALL_3_X, PLATFORM_5_TOP_Y + UPPER_WALL_3_HEIGHT + 7.0f},
    {16.0f + MAIN_ROUTE_X_OFFSET, PLATFORM_5_TOP_Y + 5.0f},
    {(BONUS_LAST_STEP.left + BONUS_LAST_STEP.right) * 0.5f, BONUS_LAST_STEP.top + 7.0f}
};

inline constexpr float PRESSURE_PLATE_A_BASE_X = 86.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float PRESSURE_PLATE_A_BASE_Y = PLATFORM_3_TOP_Y;
inline constexpr float PRESSURE_PLATE_B_BASE_X = -87.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float PRESSURE_PLATE_B_BASE_Y = PLATFORM_4_TOP_Y;
using GameConfig::PLATE_WIDTH;
using GameConfig::PLATE_HEIGHT;
using GameConfig::PLATE_PRESS_DISTANCE;

inline constexpr float MOVING_WALL_CLOSED_CENTER_X = -90.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float MOVING_WALL_OPEN_CENTER_X = -54.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float MOVING_WALL_CENTER_Y = -13.0f;
inline constexpr float MOVING_WALL_WIDTH = 20.0f;
inline constexpr float MOVING_WALL_HEIGHT = 6.0f;
inline constexpr float MOVING_WALL_TRAVEL_SECONDS = 0.7f;
inline constexpr std::initializer_list<std::size_t> MOVING_WALL_PRESSURE_PLATE_INDICES{0, 1};

inline constexpr float ENEMY_START_X = 36.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float ENEMY_END_X = PRESSURE_PLATE_A_BASE_X + PLATE_WIDTH * 0.5f;
inline constexpr float ENEMY_CENTER_Y = PLATFORM_3_TOP_Y + 6.0f;
inline constexpr float ENEMY_WIDTH = 7.0f;
inline constexpr float ENEMY_HEIGHT = 7.0f;
inline constexpr float ENEMY_SPEED = 10.0f;

inline constexpr float CRATE_BASE_X = 65.0f + MAIN_ROUTE_X_OFFSET;
inline constexpr float CRATE_BASE_Y = FLOOR_Y;
inline constexpr float CRATE_WIDTH = 8.0f;
inline constexpr float CRATE_HEIGHT = 11.0f;

}

namespace Level2 {

using GameConfig::ROOM_HALF_WIDTH;
using GameConfig::ROOM_HALF_HEIGHT;
inline constexpr float FLOOR_Y = GameConfig::DEFAULT_FLOOR_Y;
inline constexpr float SPAWN_TOP_Y = 29.0f;
inline constexpr float PLAYER_1_SPAWN_X = -83.0f;
inline constexpr float PLAYER_2_SPAWN_X = -70.0f;
inline constexpr float EXIT_X = 89.0f;
inline constexpr float EXIT_FLOOR_Y = 36.0f;

inline constexpr float PLATE_A_X = -74.0f;
inline constexpr float PLATE_A_FLOOR_Y = -28.0f;
inline constexpr float PLATE_B_X = 92.0f;
using GameConfig::PLATE_WIDTH;
using GameConfig::PLATE_HEIGHT;
using GameConfig::PLATE_PRESS_DISTANCE;

inline constexpr float GATE_X = -9.0f;
inline constexpr float SERVICE_FLOOR_Y = -13.0f;
inline constexpr float LOWER_GATE_HEIGHT = 21.0f;
inline constexpr float SERVICE_GATE_HEIGHT = 18.0f;
using GameConfig::GATE_WIDTH;
using GameConfig::GATE_TRAVEL_Y;
using GameConfig::GATE_TRAVEL_SECONDS;
inline constexpr float SHAFT_WALL_LEFT_X = -39.0f;
inline constexpr float SHAFT_LEFT_X = -32.0f;
inline constexpr float RECOVERY_FLOOR_END_X = 52.0f;
inline constexpr float RIGHT_STEP_X = 66.0f;
inline constexpr float RIGHT_STEP_LOWER_Y = -32.0f;
inline constexpr float RIGHT_STEP_RAISED_Y = -25.0f;
inline constexpr float RIGHT_STEP_WIDTH = 16.0f;
inline constexpr float RIGHT_STEP_HEIGHT = 4.0f;
inline constexpr float RIGHT_STEP_TRAVEL_SECONDS = 0.7f;

inline constexpr PlatformBounds PLATFORMS[] = {
    {-ROOM_HALF_WIDTH, SHAFT_LEFT_X, SPAWN_TOP_Y, 5.0f},
    {SHAFT_WALL_LEFT_X, SHAFT_LEFT_X, SPAWN_TOP_Y, 72.2f},
    {-ROOM_HALF_WIDTH, -90.0f, 24.0f, 17.0f},
    {-90.0f, -66.0f, 24.0f, 24.25f},
    {-66.0f, SHAFT_WALL_LEFT_X, 24.0f, 15.0f},
    {-ROOM_HALF_WIDTH, -55.0f, -5.0f, 5.0f},
    {-87.0f, SHAFT_WALL_LEFT_X, PLATE_A_FLOOR_Y, 3.0f},
    {SHAFT_LEFT_X, -25.0f, -31.0f, 4.0f},
    {-12.0f, -6.0f, SERVICE_FLOOR_Y, 15.0f},
    {-12.0f, -6.0f, 86.0f, 82.0f},
    {-12.0f, RECOVERY_FLOOR_END_X, SERVICE_FLOOR_Y, 4.0f},
    {RECOVERY_FLOOR_END_X, 57.0f, 22.0f, 56.0f},
    {9.0f, 22.0f, ROOM_HALF_HEIGHT, 38.0f},
    {13.0f, 24.0f, -2.0f, 4.0f},
    {30.0f, 42.0f, 10.0f, 4.0f},
    {56.0f, 67.0f, -8.0f, 4.0f},
    {75.0f, 88.0f, 7.0f, 4.0f},
    {49.0f, 65.0f, 22.0f, 4.0f},
    {76.0f, ROOM_HALF_WIDTH, EXIT_FLOOR_Y, 5.0f}
};

inline constexpr std::initializer_list<StarPosition> STARS = {
    {-94.0f, 0.0f}, {-94.0f, -39.0f}, {-61.0f, -23.0f}, {3.0f, -39.0f},
    {18.0f, 5.0f}, {57.0f, 29.0f}, {83.0f, 14.0f}, {96.0f, 43.0f}
};

inline constexpr float LEFT_ENEMY_X = -94.0f;
inline constexpr float LEFT_ENEMY_START_Y = -15.0f;
inline constexpr float LEFT_ENEMY_END_Y = -42.0f;
inline constexpr float LEFT_ENEMY_SPEED = 10.5f;
inline constexpr float SERVICE_ENEMY_START_X = 34.0f;
inline constexpr float SERVICE_ENEMY_END_X = 48.0f;
inline constexpr float SERVICE_ENEMY_Y = 14.0f;
inline constexpr float SERVICE_ENEMY_SPEED = 14.0f;
inline constexpr float ENEMY_SIZE = 7.0f;

inline constexpr float CRATE_X = 32.0f;
inline constexpr float CRATE_WIDTH = 8.0f;
inline constexpr float CRATE_HEIGHT = 11.0f;

}

namespace Level3 {

using GameConfig::ROOM_HALF_WIDTH;
using GameConfig::ROOM_HALF_HEIGHT;
inline constexpr float FLOOR_Y = GameConfig::DEFAULT_FLOOR_Y;
inline constexpr float UPPER_FLOOR_Y = 7.0f;
inline constexpr float PLAYER_1_SPAWN_X = -96.0f;
inline constexpr float PLAYER_2_SPAWN_X = -89.0f;
inline constexpr float EXIT_X = 92.0f;

inline constexpr float PLATE_A_X = -57.0f;
inline constexpr float PLATE_B_X = -25.0f;
inline constexpr float PLATE_C_X = 53.0f;
inline constexpr float PLATE_D_X = 49.0f;
using GameConfig::PLATE_WIDTH;
using GameConfig::PLATE_HEIGHT;
using GameConfig::PLATE_PRESS_DISTANCE;

inline constexpr float LOWER_GATE_A_X = -49.0f;
inline constexpr float UPPER_GATE_B_X = -9.0f;
inline constexpr float LOWER_GATE_C_X = 70.0f;
inline constexpr float UPPER_GATE_D_X = 76.0f;
inline constexpr float LOWER_GATE_HEIGHT = 40.0f;
inline constexpr float UPPER_GATE_HEIGHT = 51.0f;
using GameConfig::GATE_WIDTH;
using GameConfig::GATE_TRAVEL_Y;
using GameConfig::GATE_TRAVEL_SECONDS;

inline constexpr float CRATE_SHELF_Y = 17.0f;
inline constexpr float UPPER_CRATE_X = 30.0f;
inline constexpr float LOWER_CRATE_X = 7.0f;
inline constexpr float CRATE_SIZE = 6.0f;

inline constexpr float ENEMY_START_X = -6.0f;
inline constexpr float ENEMY_END_X = 2.0f;
inline constexpr float ENEMY_Y = -39.5f;
inline constexpr float ENEMY_SIZE = 7.0f;
inline constexpr float ENEMY_SPEED = 10.0f;

inline constexpr PlatformBounds PLATFORMS[] = {
    {-ROOM_HALF_WIDTH, -84.0f, UPPER_FLOOR_Y, 5.0f},
    {-66.0f, 83.0f, UPPER_FLOOR_Y, 15.0f},
    {-ROOM_HALF_WIDTH, -92.0f, -29.0f, 4.0f},
    {-88.0f, -81.0f, -11.0f, 4.0f},
    {18.0f, 38.0f, CRATE_SHELF_Y, 4.0f},
    {15.0f, 29.0f, -16.0f, 25.6f}
};

inline constexpr std::initializer_list<StarPosition> STARS = {
    {-78.0f, 17.0f}, {22.0f, 26.0f}, {62.0f, 17.0f},
    {-68.0f, -39.0f}, {-15.0f, -39.0f}, {62.0f, -39.0f}
};

}

}

namespace TutorialConfig {

using GameConfig::ROOM_HALF_WIDTH;
using GameConfig::ROOM_HALF_HEIGHT;
inline constexpr float FLOOR_Y = GameConfig::DEFAULT_FLOOR_Y;
inline constexpr float PLAYER_1_SPAWN_X = -91.0f;
inline constexpr float PLAYER_2_SPAWN_X = -77.0f;
inline constexpr float EXIT_X = 91.0f;

inline constexpr float PRACTICE_PLATFORM_X = -60.0f;
inline constexpr float PRACTICE_PLATFORM_WIDTH = 12.0f;
inline constexpr float PRACTICE_PLATFORM_HEIGHT = 7.0f;
inline constexpr float STAR_HEIGHT = 14.0f;
inline constexpr float LOW_PASSAGE_X = 26.0f;
inline constexpr float LOW_PASSAGE_WIDTH = 14.0f;
inline constexpr float LOW_PASSAGE_CLEARANCE = 4.5f;
inline constexpr float LOW_PASSAGE_THICKNESS = 4.0f;

inline constexpr float CRATE_X = -35.0f;
inline constexpr float CRATE_SIZE = 6.0f;
inline constexpr float PLATE_X = -24.0f;
inline constexpr float PLATE_WIDTH = 10.0f;
inline constexpr float PLATE_HEIGHT = GameConfig::CRATE_MAX_STEP_HEIGHT;
using GameConfig::PLATE_PRESS_DISTANCE;
inline constexpr float WALL_X = -5.0f;
inline constexpr float WALL_WIDTH = 7.0f;
inline constexpr float WALL_HEIGHT = 16.0f;
inline constexpr float WALL_CLEARANCE = 12.0f;
inline constexpr float WALL_TRAVEL_SECONDS = 0.6f;

inline constexpr float ENEMY_START_X = 55.0f;
inline constexpr float ENEMY_END_X = 75.0f;
inline constexpr float ENEMY_CENTER_Y = FLOOR_Y + 6.0f;
inline constexpr float ENEMY_SIZE = 8.0f;
inline constexpr float ENEMY_SPEED = 14.0f;

}

/**
 * @param level Definizione della stanza da completare.
 * @param platforms Bordi e spessori delle piattaforme da importare.
 * @param stars Posizioni delle stelle nella mappa.
 *
 * Converte i bordi delle piattaforme in posizioni di base e dimensioni, quindi copia le
 * posizioni delle stelle.
 */
template<std::size_t PlatformCount>
void loadRoomGeometry(LevelDefinition& level,
                      const LevelConfig::PlatformBounds (&platforms)[PlatformCount],
                      std::initializer_list<LevelConfig::StarPosition> stars)
{
    level.platforms.reserve(PlatformCount);
    for (const auto& platform : platforms) {
        level.platforms.push_back({
            {(platform.left + platform.right) * 0.5f, platform.top - platform.thickness},
            {platform.right - platform.left, platform.thickness}
        });
    }

    level.stars.reserve(stars.size());
    for (const auto& star : stars) {
        level.stars.push_back({star.x, star.y});
    }
}

/**
 * @return Definizione del primo livello.
 *
 * Configura piattaforme, stelle, pedane e ostacoli della prima stanza.
 */
LevelDefinition level1()
{
    using namespace LevelConfig::Level1;

    LevelDefinition level{};
    level.timeLimitSeconds = 180.0f;
    level.requireAllStarsToExit = true;
    level.roomHalfSize = {ROOM_HALF_WIDTH, ROOM_HALF_HEIGHT};
    level.floorY = FLOOR_Y;
    level.spawnFeet = {{{PLAYER_1_SPAWN_X, PLAYER_1_SPAWN_FEET_Y},
                        {PLAYER_2_SPAWN_X, PLAYER_2_SPAWN_FEET_Y}}};
    level.doorBase = {DOOR_X, DOOR_BASE_Y};
    loadRoomGeometry(level, PLATFORMS, STARS);

    level.pressurePlates = {
        {{PRESSURE_PLATE_A_BASE_X, PRESSURE_PLATE_A_BASE_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE},
        {{PRESSURE_PLATE_B_BASE_X, PRESSURE_PLATE_B_BASE_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE}
    };
    level.movingWalls = {
        {{MOVING_WALL_CLOSED_CENTER_X, MOVING_WALL_CENTER_Y},
         {MOVING_WALL_OPEN_CENTER_X, MOVING_WALL_CENTER_Y},
         {MOVING_WALL_WIDTH, MOVING_WALL_HEIGHT}, MOVING_WALL_TRAVEL_SECONDS, MOVING_WALL_PRESSURE_PLATE_INDICES}
    };
    level.enemies = {
        {{ENEMY_START_X, ENEMY_CENTER_Y}, {ENEMY_END_X, ENEMY_CENTER_Y}, {ENEMY_WIDTH, ENEMY_HEIGHT}, ENEMY_SPEED}
    };
    level.crates = {
        {{CRATE_BASE_X, CRATE_BASE_Y}, {CRATE_WIDTH, CRATE_HEIGHT}},
        {{UPPER_CRATE_X, PLATFORM_5_TOP_Y}, {UPPER_CRATE_WIDTH, UPPER_CRATE_HEIGHT}},
        {{EXIT_CRATE_X, DOOR_BASE_Y}, {UPPER_CRATE_WIDTH, EXIT_CRATE_HEIGHT}}
    };
    return level;
}

/**
 * @return Definizione del secondo livello.
 *
 * Nel livello 2, A comanda il portone basso e B apre la via di ritorno.
 */
LevelDefinition level2()
{
    using namespace LevelConfig::Level2;

    LevelDefinition level{};
    level.timeLimitSeconds = 150.0f;
    level.requireAllStarsToExit = true;
    level.roomHalfSize = {ROOM_HALF_WIDTH, ROOM_HALF_HEIGHT};
    level.floorY = FLOOR_Y;
    level.spawnFeet = {{{PLAYER_1_SPAWN_X, SPAWN_TOP_Y}, {PLAYER_2_SPAWN_X, SPAWN_TOP_Y}}};
    level.doorBase = {EXIT_X, EXIT_FLOOR_Y};

    loadRoomGeometry(level, PLATFORMS, STARS);

    level.pressurePlates = {
        {{PLATE_A_X, PLATE_A_FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE},
        {{PLATE_B_X, FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE}
    };
    const float lowerGateCenter = FLOOR_Y + LOWER_GATE_HEIGHT * 0.5f;
    const float serviceGateCenter = SERVICE_FLOOR_Y + SERVICE_GATE_HEIGHT * 0.5f;
    level.movingWalls = {
        {{GATE_X, lowerGateCenter}, {GATE_X, lowerGateCenter + GATE_TRAVEL_Y},
         {GATE_WIDTH, LOWER_GATE_HEIGHT}, GATE_TRAVEL_SECONDS, {0}},
        {{GATE_X, serviceGateCenter}, {GATE_X, serviceGateCenter + GATE_TRAVEL_Y},
         {GATE_WIDTH, SERVICE_GATE_HEIGHT}, GATE_TRAVEL_SECONDS, {1}},
        {{RIGHT_STEP_X, RIGHT_STEP_LOWER_Y}, {RIGHT_STEP_X, RIGHT_STEP_RAISED_Y},
         {RIGHT_STEP_WIDTH, RIGHT_STEP_HEIGHT}, RIGHT_STEP_TRAVEL_SECONDS, {1}}
    };
    level.enemies = {
        {{LEFT_ENEMY_X, LEFT_ENEMY_START_Y}, {LEFT_ENEMY_X, LEFT_ENEMY_END_Y},
         {ENEMY_SIZE, ENEMY_SIZE}, LEFT_ENEMY_SPEED},
        {{SERVICE_ENEMY_START_X, SERVICE_ENEMY_Y}, {SERVICE_ENEMY_END_X, SERVICE_ENEMY_Y},
         {ENEMY_SIZE, ENEMY_SIZE}, SERVICE_ENEMY_SPEED}
    };
    level.crates = {{{CRATE_X, FLOOR_Y}, {CRATE_WIDTH, CRATE_HEIGHT}}};
    return level;
}

/**
 * @return Definizione del terzo livello.
 *
 * Nel livello 3 i due percorsi si aprono a vicenda.
 */
LevelDefinition level3()
{
    using namespace LevelConfig::Level3;

    LevelDefinition level{};
    level.timeLimitSeconds = 120.0f;
    level.requireAllStarsToExit = true;
    level.roomHalfSize = {ROOM_HALF_WIDTH, ROOM_HALF_HEIGHT};
    level.floorY = FLOOR_Y;
    level.spawnFeet = {{{PLAYER_1_SPAWN_X, UPPER_FLOOR_Y}, {PLAYER_2_SPAWN_X, UPPER_FLOOR_Y}}};
    level.doorBase = {EXIT_X, FLOOR_Y};
    loadRoomGeometry(level, PLATFORMS, STARS);

    level.pressurePlates = {
        {{PLATE_A_X, UPPER_FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE},
        {{PLATE_B_X, FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE},
        {{PLATE_C_X, UPPER_FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE},
        {{PLATE_D_X, FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE}
    };
    const float lowerGateCenter = FLOOR_Y + LOWER_GATE_HEIGHT * 0.5f;
    const float upperGateCenter = UPPER_FLOOR_Y + UPPER_GATE_HEIGHT * 0.5f;
    level.movingWalls = {
        {{LOWER_GATE_A_X, lowerGateCenter}, {LOWER_GATE_A_X, lowerGateCenter + GATE_TRAVEL_Y},
         {GATE_WIDTH, LOWER_GATE_HEIGHT}, GATE_TRAVEL_SECONDS, {0}},
        {{UPPER_GATE_B_X, upperGateCenter}, {UPPER_GATE_B_X, upperGateCenter + GATE_TRAVEL_Y},
         {GATE_WIDTH, UPPER_GATE_HEIGHT}, GATE_TRAVEL_SECONDS, {1}},
        {{LOWER_GATE_C_X, lowerGateCenter}, {LOWER_GATE_C_X, lowerGateCenter + GATE_TRAVEL_Y},
         {GATE_WIDTH, LOWER_GATE_HEIGHT}, GATE_TRAVEL_SECONDS, {2}},
        {{UPPER_GATE_D_X, upperGateCenter}, {UPPER_GATE_D_X, upperGateCenter + GATE_TRAVEL_Y},
         {GATE_WIDTH, UPPER_GATE_HEIGHT}, GATE_TRAVEL_SECONDS, {3}}
    };
    level.crates = {
        {{UPPER_CRATE_X, CRATE_SHELF_Y}, {CRATE_SIZE, CRATE_SIZE}},
        {{LOWER_CRATE_X, FLOOR_Y}, {CRATE_SIZE, CRATE_SIZE}}
    };
    level.enemies = {
        {{ENEMY_START_X, ENEMY_Y}, {ENEMY_END_X, ENEMY_Y}, {ENEMY_SIZE, ENEMY_SIZE}, ENEMY_SPEED}
    };
    return level;
}

const LevelDefinition levels[] = {
    level1(),
    level2(),
    level3()
};

/**
 * @return Definizione della stanza di prova.
 *
 * Compone una stanza di prova con gli stessi oggetti della campagna.
 */
LevelDefinition tutorialRoom()
{
    using namespace TutorialConfig;

    LevelDefinition room{};
    room.requireAllStarsToExit = false;
    room.timeLimitSeconds = std::nullopt;
    room.roomHalfSize = {ROOM_HALF_WIDTH, ROOM_HALF_HEIGHT};
    room.floorY = FLOOR_Y;
    room.spawnFeet = {{{PLAYER_1_SPAWN_X, FLOOR_Y}, {PLAYER_2_SPAWN_X, FLOOR_Y}}};
    room.doorBase = {EXIT_X, FLOOR_Y};
    room.platforms = {
        {{PRACTICE_PLATFORM_X, FLOOR_Y}, {PRACTICE_PLATFORM_WIDTH, PRACTICE_PLATFORM_HEIGHT}},
        {{LOW_PASSAGE_X, FLOOR_Y + LOW_PASSAGE_CLEARANCE}, {LOW_PASSAGE_WIDTH, LOW_PASSAGE_THICKNESS}}
    };
    room.stars = {{PRACTICE_PLATFORM_X, FLOOR_Y + STAR_HEIGHT}};
    room.pressurePlates = {{{PLATE_X, FLOOR_Y}, PLATE_WIDTH, PLATE_HEIGHT, PLATE_PRESS_DISTANCE}};
    const float wallCenterY = FLOOR_Y + WALL_HEIGHT * 0.5f;
    room.movingWalls = {
        {{WALL_X, wallCenterY}, {WALL_X, wallCenterY + WALL_CLEARANCE},
         {WALL_WIDTH, WALL_HEIGHT}, WALL_TRAVEL_SECONDS, {0}}
    };
    room.enemies = {{{ENEMY_START_X, ENEMY_CENTER_Y}, {ENEMY_END_X, ENEMY_CENTER_Y},
                    {ENEMY_SIZE, ENEMY_SIZE}, ENEMY_SPEED}};
    room.crates = {{{CRATE_X, FLOOR_Y}, {CRATE_SIZE, CRATE_SIZE}}};
    return room;
}
}

namespace Levels {

const LevelDefinition& get(std::size_t index)
{
    if (index >= count()) {
        throw std::out_of_range("Indice livello non valido");
    }
    return levels[index];
}

std::size_t count()
{
    return std::size(levels);
}

const LevelDefinition& tutorial()
{
    static const LevelDefinition definition = tutorialRoom();
    return definition;
}

}
