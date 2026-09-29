#pragma once

#include "GameState.hpp"

#include <vector>

namespace poker {

/*
 * Rule implementation for Kuhn Poker.
 *
 * Kuhn Poker:
 *
 *     - 2 players
 *     - 3 cards: J, Q, K
 *     - each player antes 1 chip
 *     - one private card per player
 *     - one betting round
 *     - actions: Check, Bet, Call, Fold
 */
class KuhnPoker {
public:
    static constexpr int NUM_PLAYERS = 2;
    static constexpr int INITIAL_CHIPS = 1;

    /*
     * Creates and deals a fresh Kuhn Poker game.
     *
     * The deck is shuffled and each player receives one
     * private card.
     */
    GameState initialState() const;

    /*
     * Returns all actions legally available from the current state.
     */
    std::vector<Action> legalActions(
        const GameState& state
    ) const;

    /*
     * Applies an action and advances the game.
     *
     * Throws std::invalid_argument for an illegal action.
     */
    void applyAction(
        GameState& state,
        const Action& action
    ) const;

    /*
     * Returns whether the current state is terminal.
     */
    bool isTerminal(
        const GameState& state
    ) const;

    /*
     * Returns the utility/payoff for a player.
     *
     * Positive = player wins chips.
     * Negative = player loses chips.
     * Zero = no change.
     */
    int utility(
        const GameState& state,
        int player
    ) const;

    /*
     * Returns the winner when the game ends.
     *
     * Returns -1 for a showdown that still needs evaluation
     * or a non-terminal state.
     */
    int determineWinner(
        const GameState& state
    ) const;

private:
    static bool isBettingAction(ActionType action);

    static bool hasBet(const GameState& state);

    static int otherPlayer(int player) noexcept {
        return 1 - player;
    };
};

} // namespace poker