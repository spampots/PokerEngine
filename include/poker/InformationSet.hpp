#pragma once

#include "GameState.hpp"

#include <string>

namespace poker {

/*
 * An information set represents everything a player is legally
 * allowed to know when making a decision.
 *
 * In Kuhn Poker this consists of:
 *
 *     - the player's private card
 *     - the public betting history
 *
 * The opponent's private card is intentionally excluded.
 */
class InformationSet {
public:
    InformationSet() = default;

    explicit InformationSet(std::string key);

    /*
     * Construct an information-set key from a game state.
     *
     * Example:
     *
     *     "K|"
     *     "Q|B"
     *     "J|CB"
     */
    static InformationSet fromState(
        const GameState& state,
        int player
    );

    const std::string& key() const noexcept;

    bool operator==(const InformationSet& other) const noexcept;

    bool operator!=(const InformationSet& other) const noexcept;

private:
    std::string key_;
};

} // namespace poker