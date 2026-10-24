#include "poker/KuhnPoker.hpp"

#include <algorithm>
#include <array>
#include <random>
#include <stdexcept>

namespace poker {

namespace {

bool isBetAction(const Action& action) {
    return action.type == ActionType::Bet;
}

bool isCheckAction(const Action& action) {
    return action.type == ActionType::Check;
}

bool isCallAction(const Action& action) {
    return action.type == ActionType::Call;
}

bool isFoldAction(const Action& action) {
    return action.type == ActionType::Fold;
}

} // namespace

KuhnPoker::KuhnPoker()
    : KuhnPoker(std::random_device{}()) {}

KuhnPoker::KuhnPoker(std::uint32_t seed)
    : generator_(seed) {}

std::array<Card, 3> KuhnPoker::deck() {
    /*
     * Kuhn Poker uses only J, Q and K.
     *
     * Suits are irrelevant, so all cards use Clubs.
     */
    return {
        Card{Rank::Jack, Suit::Clubs},
        Card{Rank::Queen, Suit::Clubs},
        Card{Rank::King, Suit::Clubs}
    };
}

GameState KuhnPoker::initialState() const {
    std::array<Card, 3> cards = deck();

    std::shuffle(
        cards.begin(),
        cards.end(),
        generator_
    );

    return initialState(cards[0], cards[1]);
}

GameState KuhnPoker::initialState(
    Card card0,
    Card card1
) const {
    if (card0 == card1) {
        throw std::invalid_argument(
            "Players cannot hold the same card"
        );
    }

    GameState state;

    state.setPrivateCard(0, card0);
    state.setPrivateCard(1, card1);

    /*
     * Each player antes one chip.
     *
     * GameState starts with a pot of 2 and each player's
     * contribution initialized to 1.
     */
    state.setCurrentPlayer(0);

    return state;
}

std::vector<Action> KuhnPoker::legalActions(
    const GameState& state
) const {
    if (state.isTerminal()) {
        return {};
    }

    const auto& history = state.history();

    /*
     * First action:
     *
     *     Check
     *     Bet
     */
    if (history.empty()) {
        return {
            Action{ActionType::Check},
            Action{ActionType::Bet, 1}
        };
    }

    const Action& previous = history.back();

    /*
     * If somebody checked, the next player may:
     *
     *     Check
     *     Bet
     *
     * If somebody bet, the next player may:
     *
     *     Call
     *     Fold
     */
    if (isCheckAction(previous)) {
        return {
            Action{ActionType::Check},
            Action{ActionType::Bet, 1}
        };
    }

    if (isBetAction(previous)) {
        return {
            Action{ActionType::Call, 1},
            Action{ActionType::Fold}
        };
    }

    /*
     * Kuhn Poker never reaches a state where Call or Fold
     * is followed by another decision.
     */
    return {};
}

void KuhnPoker::applyAction(
    GameState& state,
    const Action& action
) const {
    if (state.isTerminal()) {
        throw std::invalid_argument(
            "Cannot act on a terminal state"
        );
    }

    const auto legal = legalActions(state);

    const bool legalAction =
        std::find(
            legal.begin(),
            legal.end(),
            action
        ) != legal.end();

    if (!legalAction) {
        throw std::invalid_argument(
            "Illegal action: " + action.toString()
        );
    }

    const int player = state.currentPlayer();
    const int opponent = otherPlayer(player);

    state.addAction(action);

    if (isCheckAction(action)) {
        /*
         * Check-check ends the game in Kuhn Poker.
         */
        if (state.history().size() == 2) {
            state.setTerminal(true);
            state.setWinner(
                determineWinner(state)
            );
            return;
        }

        state.setCurrentPlayer(opponent);
        return;
    }

    if (isBetAction(action)) {
        state.addContribution(
            player,
            action.amount
        );

        state.setCurrentPlayer(opponent);
        return;
    }

    if (isCallAction(action)) {
        state.addContribution(
            player,
            action.amount
        );

        state.setTerminal(true);
        state.setWinner(
            determineWinner(state)
        );

        return;
    }

    if (isFoldAction(action)) {
        state.setTerminal(true);
        state.setWinner(opponent);

        return;
    }

    throw std::invalid_argument(
        "Unsupported action"
    );
}

bool KuhnPoker::isTerminal(
    const GameState& state
) const {
    return state.isTerminal();
}

int KuhnPoker::utility(
    const GameState& state,
    int player
) const {
    return state.utility(player);
}

int KuhnPoker::determineWinner(
    const GameState& state
) const {
    if (!state.isTerminal()) {
        /*
         * This function is also used immediately before setting
         * terminal=true for showdowns, so infer the result from
         * the current cards when possible.
         */
    }

    if (state.hasFolded(0)) {
        return 1;
    }

    if (state.hasFolded(1)) {
        return 0;
    }

    const Card card0 = state.privateCard(0);
    const Card card1 = state.privateCard(1);

    /*
     * Kuhn cards have ordering:
     *
     *     J < Q < K
     */
    const int rank0 =
        static_cast<int>(card0.rank);

    const int rank1 =
        static_cast<int>(card1.rank);

    if (rank0 > rank1) {
        return 0;
    }

    return 1;
}

bool KuhnPoker::isBettingAction(
    ActionType action
) {
    return action == ActionType::Bet;
}

bool KuhnPoker::hasBet(
    const GameState& state
) {
    for (const auto& action : state.history()) {
        if (action.type == ActionType::Bet) {
            return true;
        }
    }

    return false;
}

} // namespace poker