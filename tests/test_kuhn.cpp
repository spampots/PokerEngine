#include <gtest/gtest.h>

#include "poker/KuhnPoker.hpp"

using namespace poker;

TEST(KuhnPokerTest, InitialStateDealsTwoDifferentCards) {
    KuhnPoker game;

    GameState state = game.initialState();

    ASSERT_TRUE(state.hasPrivateCard(0));
    ASSERT_TRUE(state.hasPrivateCard(1));

    EXPECT_NE(
        state.privateCard(0),
        state.privateCard(1)
    );
}

TEST(KuhnPokerTest, InitialStateHasCorrectPot) {
    KuhnPoker game;

    GameState state = game.initialState();

    EXPECT_EQ(state.pot(), 2);
}

TEST(KuhnPokerTest, FirstPlayerCanCheckOrBet) {
    KuhnPoker game;

    GameState state = game.initialState();

    auto actions = game.legalActions(state);

    ASSERT_EQ(actions.size(), 2);

    EXPECT_EQ(actions[0].type, ActionType::Check);
    EXPECT_EQ(actions[1].type, ActionType::Bet);
}

TEST(KuhnPokerTest, CheckAllowsOpponentToCheckOrBet) {
    KuhnPoker game;

    GameState state = game.initialState();

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    EXPECT_FALSE(state.isTerminal());
    EXPECT_EQ(state.currentPlayer(), 1);

    auto actions = game.legalActions(state);

    ASSERT_EQ(actions.size(), 2);

    EXPECT_EQ(actions[0].type, ActionType::Check);
    EXPECT_EQ(actions[1].type, ActionType::Bet);
}

TEST(KuhnPokerTest, CheckCheckEndsGame) {
    KuhnPoker game;

    GameState state = game.initialState();

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    EXPECT_TRUE(state.isTerminal());
    EXPECT_NE(state.winner(), -1);
}

TEST(KuhnPokerTest, BetAllowsCallOrFold) {
    KuhnPoker game;

    GameState state = game.initialState();

    game.applyAction(
        state,
        Action{ActionType::Bet, 1}
    );

    EXPECT_FALSE(state.isTerminal());

    auto actions = game.legalActions(state);

    ASSERT_EQ(actions.size(), 2);

    EXPECT_EQ(actions[0].type, ActionType::Call);
    EXPECT_EQ(actions[1].type, ActionType::Fold);
}

TEST(KuhnPokerTest, BetIncreasesPot) {
    KuhnPoker game;

    GameState state = game.initialState();

    const int initialPot = state.pot();

    game.applyAction(
        state,
        Action{ActionType::Bet, 1}
    );

    EXPECT_EQ(
        state.pot(),
        initialPot + 1
    );
}

TEST(KuhnPokerTest, FoldEndsGameImmediately) {
    KuhnPoker game;

    GameState state = game.initialState();

    game.applyAction(
        state,
        Action{ActionType::Bet, 1}
    );

    game.applyAction(
        state,
        Action{ActionType::Fold}
    );

    EXPECT_TRUE(state.isTerminal());
    EXPECT_EQ(state.winner(), 0);
}

TEST(KuhnPokerTest, CallEndsGame) {
    KuhnPoker game;

    GameState state = game.initialState();

    game.applyAction(
        state,
        Action{ActionType::Bet, 1}
    );

    game.applyAction(
        state,
        Action{ActionType::Call, 1}
    );

    EXPECT_TRUE(state.isTerminal());
    EXPECT_NE(state.winner(), -1);
}

TEST(KuhnPokerTest, IllegalActionThrows) {
    KuhnPoker game;

    GameState state = game.initialState();

    /*
     * Call is not legal as the first action.
     */
    EXPECT_THROW(
        game.applyAction(
            state,
            Action{ActionType::Call, 1}
        ),
        std::invalid_argument
    );
}

TEST(KuhnPokerTest, FoldWinnerIsCorrect) {
    KuhnPoker game;

    GameState state = game.initialState();

    game.applyAction(
        state,
        Action{ActionType::Bet, 1}
    );

    game.applyAction(
        state,
        Action{ActionType::Fold}
    );

    EXPECT_EQ(state.winner(), 0);
}

TEST(KuhnPokerTest, StrongerCardWinsShowdown) {
    KuhnPoker game;

    GameState state;

    state.setPrivateCard(
        0,
        Card{Rank::King, Suit::Clubs}
    );

    state.setPrivateCard(
        1,
        Card{Rank::Jack, Suit::Clubs}
    );

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    EXPECT_TRUE(state.isTerminal());
    EXPECT_EQ(state.winner(), 0);
}

TEST(KuhnPokerTest, WeakerCardLosesShowdown) {
    KuhnPoker game;

    GameState state;

    state.setPrivateCard(
        0,
        Card{Rank::Jack, Suit::Clubs}
    );

    state.setPrivateCard(
        1,
        Card{Rank::King, Suit::Clubs}
    );

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    game.applyAction(
        state,
        Action{ActionType::Check}
    );

    EXPECT_TRUE(state.isTerminal());
    EXPECT_EQ(state.winner(), 1);
}