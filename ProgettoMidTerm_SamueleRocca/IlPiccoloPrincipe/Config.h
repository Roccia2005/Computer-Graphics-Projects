#pragma once
#include <cstddef>

namespace GameConfig {

inline constexpr char GAME_TITLE[] = "Il Piccolo Principe";
inline constexpr float DEFAULT_FLOOR_Y = -48.0f;
inline constexpr float PLAYER_WIDTH = 6.0f;
inline constexpr float PLAYER_HITBOX_WIDTH = 5.0f;
inline constexpr float PLAYER_HEIGHT = 7.0f;
inline constexpr float PLAYER_RENDER_SCALE = 1.08f;
inline constexpr float MOVE_SPEED = 43.0f;
inline constexpr float JUMP_SPEED = 75.0f;
inline constexpr float GRAVITY = -135.0f;
inline constexpr float CRATE_PUSH_SPEED_FACTOR = 0.5f;
inline constexpr float CRATE_MAX_STEP_HEIGHT = 1.5f;
inline constexpr float STAR_RADIUS = 3.0f;
inline constexpr float STAR_BOB_AMPLITUDE = 2.0f;
inline constexpr float STAR_BOB_FREQUENCY = 3.0f;
inline constexpr float STAR_PHASE_STEP = 0.8f;
/**
 * @param index Indice dell'elemento, a partire da zero.
 * @return Sfasamento angolare in radianti.
 *
 * Sfasa l'oscillazione delle stelle in base alla loro posizione nell'elenco.
 */
inline constexpr float starPhase(std::size_t index)
{
    return static_cast<float>(index) * STAR_PHASE_STEP;
}
inline constexpr float STAR_ROTATION_SPEED = 1.25f;

inline constexpr float CROUCH_HEIGHT_RATIO = 0.5f;
inline constexpr float MAX_FRAME_DELTA_SECONDS = 0.03f;
inline constexpr float DOOR_WIDTH = 12.0f;
inline constexpr float DOOR_HEIGHT = 16.0f;
inline constexpr float DOOR_OPENING_SECONDS = 0.35f;
inline constexpr float FLOOR_THICKNESS = 6.0f;

inline constexpr float ROOM_HALF_WIDTH = 100.0f;
inline constexpr float ROOM_HALF_HEIGHT = 60.0f;
inline constexpr float PLATE_WIDTH = 8.0f;
inline constexpr float PLATE_HEIGHT = 1.2f;
inline constexpr float PLATE_PRESS_DISTANCE = 0.3f;
inline constexpr float GATE_WIDTH = 6.0f;
inline constexpr float GATE_TRAVEL_Y = 12.0f;
inline constexpr float GATE_TRAVEL_SECONDS = 0.48f;

}
