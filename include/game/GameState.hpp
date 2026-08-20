#pragma once

#include "../cards/Card.hpp"
#include "Action.hpp"

#include <vector>

namespace poker
{

    enum class Player
    {
        Player1,
        Player2
    };

    class GameState
    {
    public:
        GameState(Card myCard, Player player);

        Card myCard() const;
        Player currentPlayer() const;

        const std::vector<Action> &history() const;

        void applyAction(Action action);

        bool isTerminal() const;

    private:
        Card myCard_;
        Player currentPlayer_;

        std::vector<Action> history_;
    };

} // namespace poker