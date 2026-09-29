#pragma once

#include "Card.hpp"

#include <cstddef>
#include <string>
#include <vector>

namespace poker {

enum class ActionType {
    Check,
    Bet,
    Call,
    Fold
};

struct Action {
    ActionType type;
    int amount = 0;

    constexpr Action(
        ActionType actionType,
        int actionAmount = 0
    ) noexcept
        : type(actionType),
          amount(actionAmount) {}

    std::string toString() const;

    constexpr bool operator==(const Action& other) const noexcept {
        return type == other.type && amount == other.amount;
    }
};

class GameState {
public:
    static constexpr int NUM_PLAYERS = 2;

    GameState() = default;

    // -----------------------------------------------------------------
    // Player / card state
    // -----------------------------------------------------------------

    void setPrivateCard(int player, Card card);

    Card privateCard(int player) const;

    bool hasPrivateCard(int player) const;

    // -----------------------------------------------------------------
    // Betting state
    // -----------------------------------------------------------------

    int currentPlayer() const noexcept;

    void setCurrentPlayer(int player) noexcept;

    int pot() const noexcept;

    int playerChips(int player) const;

    void setPlayerChips(int player, int chips);

    int playerContribution(int player) const;

    void addContribution(int player, int amount);

    bool hasFolded(int player) const;

    void setFolded(int player, bool folded);

    // -----------------------------------------------------------------
    // Action history
    // -----------------------------------------------------------------

    const std::vector<Action>& history() const noexcept;

    void addAction(const Action& action);

    // -----------------------------------------------------------------
    // Game status
    // -----------------------------------------------------------------

    bool isTerminal() const noexcept;

    void setTerminal(bool terminal) noexcept;

    int winner() const noexcept;

    void setWinner(int player) noexcept;

    // -----------------------------------------------------------------
    // Utility
    // -----------------------------------------------------------------

    int utility(int player) const;

    // -----------------------------------------------------------------
    // Serialization
    // Useful for debugging / testing / information-set construction.
    // -----------------------------------------------------------------

    std::string historyString() const;

private:
    Card privateCards_[NUM_PLAYERS]{
        Card{Rank::Two, Suit::Clubs},
        Card{Rank::Two, Suit::Clubs}
    };

    bool hasCard_[NUM_PLAYERS]{
        false,
        false
    };

    int chips_[NUM_PLAYERS]{
        1,
        1
    };

    int contributions_[NUM_PLAYERS]{
        1,
        1
    };

    bool folded_[NUM_PLAYERS]{
        false,
        false
    };

    int currentPlayer_ = 0;

    int pot_ = 2;

    bool terminal_ = false;

    int winner_ = -1;

    std::vector<Action> history_;
};

} // namespace poker