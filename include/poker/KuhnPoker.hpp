#pragma once

#include "GameState.hpp"

#include <array>
#include <cstdint>
#include <random>
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
     * Seeds the shuffling generator from std::random_device.
     */
    KuhnPoker();

    /*
     * Seeds the shuffling generator explicitly, so that the
     * sequence of deals (and therefore CFR training) is
     * reproducible.
     */
    explicit KuhnPoker(std::uint32_t seed);

    /*
     * The three-card Kuhn deck: J, Q, K.
     */
    static std::array<Card, 3> deck();

    /*
     * Creates and deals a fresh Kuhn Poker game.
     *
     * The deck is shuffled and each player receives one
     * private card.
     */
    GameState initialState() const;

    /*
     * Creates a game with the given private cards.
     *
     * Throws std::invalid_argument if both cards are equal.
     */
    GameState initialState(
        Card card0,
        Card card1
    ) const;

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

    /*
     * Mutable so that dealing stays a const operation on the
     * rules object.
     */
    mutable std::mt19937 generator_;
};

} // namespace poker