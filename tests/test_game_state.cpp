#include <gtest/gtest.h>

#include "poker/GameState.hpp"

using namespace poker;

TEST(GameStateTest, InitialStateHasTwoAntes) {
    GameState state;

    EXPECT_EQ(state.pot(), 2);
    EXPECT_EQ(state.playerContribution(0), 1);
    EXPECT_EQ(state.playerContribution(1), 1);
}

TEST(GameStateTest, PrivateCardsCanBeAssigned) {
    GameState state;

    Card king{Rank::King, Suit::Clubs};

    state.setPrivateCard(0, king);

    EXPECT_TRUE(state.hasPrivateCard(0));
    EXPECT_EQ(state.privateCard(0), king);
}

TEST(GameStateTest, ContributionsIncreasePot) {
    GameState state;

    state.addContribution(0, 1);

    EXPECT_EQ(state.playerContribution(0), 2);
    EXPECT_EQ(state.pot(), 3);
}

TEST(GameStateTest, ActionsAreRecorded) {
    GameState state;

    state.addAction(
        Action{ActionType::Check}
    );

    state.addAction(
        Action{ActionType::Bet, 1}
    );

    ASSERT_EQ(state.history().size(), 2);

    EXPECT_EQ(
        state.history()[0].type,
        ActionType::Check
    );

    EXPECT_EQ(
        state.history()[1].type,
        ActionType::Bet
    );
}

TEST(GameStateTest, HistoryStringIsCorrect) {
    GameState state;

    state.addAction(
        Action{ActionType::Check}
    );

    state.addAction(
        Action{ActionType::Bet, 1}
    );

    EXPECT_EQ(
        state.historyString(),
        "C B1"
    );
}

TEST(GameStateTest, TerminalStateStoresWinner) {
    GameState state;

    state.setTerminal(true);
    state.setWinner(0);

    EXPECT_TRUE(state.isTerminal());
    EXPECT_EQ(state.winner(), 0);
}

TEST(GameStateTest, UtilityForWinnerIsPositive) {
    GameState state;

    state.setTerminal(true);
    state.setWinner(0);

    EXPECT_GT(state.utility(0), 0);
    EXPECT_LT(state.utility(1), 0);
}