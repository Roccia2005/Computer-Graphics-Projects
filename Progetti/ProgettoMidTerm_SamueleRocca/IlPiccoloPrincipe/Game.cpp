#include "Game.h"
#include "LevelCatalog.h"

Game::Game() : m_level(Levels::get(0))
{
}

void Game::update(float dt, const PlayerInput& input1, const PlayerInput& input2,
                  float currentTime)
{
    if (m_finished) {
        return;
    }
    m_level.update(dt, input1, input2, currentTime);

    if (!m_level.isFinished()) {
        return;
    }
    const auto nextIndex = m_levelIndex + 1;
    if (nextIndex == Levels::count()) {
        m_finished = true;
        return;
    }

    m_level = Level(Levels::get(nextIndex));
    m_levelIndex = nextIndex;
}

void Game::restartCurrentLevel()
{
    m_level.reset();
    m_finished = false;
}
