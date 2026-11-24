#pragma once
#include <array>
#include <cstddef>
#include <string_view>

enum class DialogueSpeaker { Prince, Fox, Count };

struct DialogueLine {
    DialogueSpeaker speaker;
    std::string_view text;
};

/**
 * @param speaker Personaggio che pronuncia la battuta.
 * @return Nome da visualizzare per il personaggio.
 *
 * Associa il personaggio al nome da mostrare sopra la battuta.
 */
inline const char* speakerName(DialogueSpeaker speaker)
{
    static constexpr std::array names{"Piccolo Principe", "Volpe"};
    static_assert(names.size() == static_cast<std::size_t>(DialogueSpeaker::Count));
    return names.at(static_cast<std::size_t>(speaker));
}
