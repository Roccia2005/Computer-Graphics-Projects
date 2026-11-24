#include "PlayerPhysics.h"

namespace PlayerPhysics {

Step::Step(std::array<Player, 2>& players, const std::vector<SceneCollider>& colliders,
           float roomHalfWidth, float floorY)
    : m_players(players), m_colliders(colliders), m_roomHalfWidth(roomHalfWidth), m_floorY(floorY)
{
}

void Step::prepare(const std::array<PlayerInput, 2>& inputs)
{
    for (std::size_t i = 0; i < m_players.size(); ++i) {
        m_states[i].verticalReferenceBounds = m_players[i].bounds();
        m_players[i].applyInput(inputs[i]);
    }
    captureSupports();
}

void Step::integrate(float dt)
{
    movePlayersHorizontally(dt);
    movePlayersVertically(dt);
}

}
