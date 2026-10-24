#include "poker/GameState.hpp"

#include <stdexcept>

namespace poker {

namespace {

void validatePlayer(int player) {
    if (player < 0 || player >= GameState::NUM_PLAYERS) {
        throw std::out_of_range("Invalid player index");
    }
}

} // namespace

// ============================================================================
// Action
// ============================================================================

std::string Action::toString() const {
    /*
     * Compact notation used in history strings and
     * information-set keys:
     *
     *     C  = check
     *     B  = bet
     *     c  = call
     *     F  = fold
     *
     * Non-zero amounts are appended, e.g. "B1", "c1".
     */
    std::string result;

    switch (type) {
        case ActionType::Check:
            result = "C";
            break;
        case ActionType::Bet:
            result = "B";
            break;
        case ActionType::Call:
            result = "c";
            break;
        case ActionType::Fold:
            result = "F";
            break;
    }

    if (amount != 0) {
        result += std::to_string(amount);
    }

    return result;
}

// ============================================================================
// Player / card state
// ============================================================================

void GameState::setPrivateCard(int player, Card card) {
    validatePlayer(player);

    privateCards_[player] = card;
    hasCard_[player] = true;
}

Card GameState::privateCard(int player) const {
    validatePlayer(player);

    if (!hasCard_[player]) {
        throw std::logic_error(
            "Player has not been dealt a private card"
        );
    }

    return privateCards_[player];
}

bool GameState::hasPrivateCard(int player) const {
    validatePlayer(player);

    return hasCard_[player];
}

// ============================================================================
// Betting state
// ============================================================================

int GameState::currentPlayer() const noexcept {
    return currentPlayer_;
}

void GameState::setCurrentPlayer(int player) noexcept {
    currentPlayer_ = player;
}

int GameState::pot() const noexcept {
    return pot_;
}

int GameState::playerChips(int player) const {
    validatePlayer(player);

    return chips_[player];
}

void GameState::setPlayerChips(int player, int chips) {
    validatePlayer(player);

    if (chips < 0) {
        throw std::invalid_argument(
            "Chip count cannot be negative"
        );
    }

    chips_[player] = chips;
}

int GameState::playerContribution(int player) const {
    validatePlayer(player);

    return contributions_[player];
}

void GameState::addContribution(int player, int amount) {
    validatePlayer(player);

    if (amount < 0) {
        throw std::invalid_argument(
            "Contribution cannot be negative"
        );
    }

    if (amount > chips_[player]) {
        throw std::invalid_argument(
            "Contribution exceeds player's remaining chips"
        );
    }

    /*
     * Chips move from the player's stack into the pot.
     */
    chips_[player] -= amount;
    contributions_[player] += amount;
    pot_ += amount;
}

bool GameState::hasFolded(int player) const {
    validatePlayer(player);

    return folded_[player];
}

void GameState::setFolded(int player, bool folded) {
    validatePlayer(player);

    folded_[player] = folded;
}

// ============================================================================
// Action history
// ============================================================================

const std::vector<Action>& GameState::history() const noexcept {
    return history_;
}

void GameState::addAction(const Action& action) {
    history_.push_back(action);
}

// ============================================================================
// Game status
// ============================================================================

bool GameState::isTerminal() const noexcept {
    return terminal_;
}

void GameState::setTerminal(bool terminal) noexcept {
    terminal_ = terminal;
}

int GameState::winner() const noexcept {
    return winner_;
}

void GameState::setWinner(int player) noexcept {
    winner_ = player;
}

// ============================================================================
// Utility
// ============================================================================

int GameState::utility(int player) const {
    validatePlayer(player);

    if (!terminal_) {
        throw std::logic_error(
            "Utility is only defined for terminal states"
        );
    }

    /*
     * No winner on a terminal state means a split pot.
     */
    if (winner_ < 0) {
        return 0;
    }

    /*
     * The winner gains everything the opponent put in;
     * the loser forfeits their own contribution.
     */
    const int opponent = 1 - player;

    if (winner_ == player) {
        return contributions_[opponent];
    }

    return -contributions_[player];
}

// ============================================================================
// Serialization
// ============================================================================

std::string GameState::historyString() const {
    std::string result;

    for (std::size_t i = 0; i < history_.size(); ++i) {
        if (i > 0) {
            result += " ";
        }

        result += history_[i].toString();
    }

    return result;
}

} // namespace poker
