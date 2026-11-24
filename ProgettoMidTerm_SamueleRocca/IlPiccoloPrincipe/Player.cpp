#include "Player.h"

void Player::spawnAtFeet(glm::vec2 feet)
{
    m_spawnPosition = feet + glm::vec2{0.0f, GameConfig::PLAYER_HEIGHT * 0.5f};
    reset();
}

void Player::reset()
{
    m_position = m_spawnPosition;
    m_velocity = {0.0f, 0.0f};
    m_onGround = true;
    m_height = GameConfig::PLAYER_HEIGHT;
}

void Player::applyInput(const PlayerInput& input)
{
    m_velocity.x = (static_cast<float>(input.right) - static_cast<float>(input.left))
               * GameConfig::MOVE_SPEED;
    if (input.jumpHeld && m_onGround) {
        m_velocity.y = GameConfig::JUMP_SPEED;
        m_onGround = false;
    }
}

float Player::requestedHeight(const PlayerInput& input) const
{
    return input.down
        ? GameConfig::PLAYER_HEIGHT * GameConfig::CROUCH_HEIGHT_RATIO
        : GameConfig::PLAYER_HEIGHT;
}

AABB Player::bounds() const
{
    return boundsAtHeight(m_height);
}

AABB Player::boundsAtHeight(float height) const
{
    const glm::vec2 center = m_position + glm::vec2{0.0f, (height - m_height) * 0.5f};
    return makeAABB(center, {GameConfig::PLAYER_HITBOX_WIDTH, height});
}

void Player::setHeight(float height)
{
    m_position.y = boundsAtHeight(height).center.y;
    m_height = height;
}

void Player::advanceVertically(float dt)
{
    m_velocity.y += GameConfig::GRAVITY * dt;
    m_position.y += m_velocity.y * dt;
    m_onGround = false;
}

void Player::landOn(float surfaceY)
{
    m_position.y = surfaceY + m_height * 0.5f;
    m_velocity.y = 0.0f;
    m_onGround = true;
}

void Player::hitCeilingAt(float surfaceY)
{
    m_position.y = surfaceY - m_height * 0.5f;
    m_velocity.y = 0.0f;
}

void Player::translate(glm::vec2 delta)
{
    m_position += delta;
}

void Player::stopHorizontalMotion()
{
    m_velocity.x = 0.0f;
}

void Player::stopVerticalMotion()
{
    m_velocity.y = 0.0f;
}
