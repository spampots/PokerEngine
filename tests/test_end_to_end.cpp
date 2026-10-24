#include <gtest/gtest.h>

#include "poker/CFR.hpp"
#include "poker/InformationSet.hpp"
#include "poker/KuhnPoker.hpp"

using namespace poker;

TEST(EndToEndTest, KuhnPokerCanBeTrained) {
    KuhnPoker game;
    CFRSolver solver(game);

    solver.train(10000);

    EXPECT_EQ(
        solver.iterations(),
        10000
    );

    EXPECT_GT(
        solver.informationSetCount(),
        0
    );
}

TEST(EndToEndTest, LearnedStrategiesAreNormalized) {
    KuhnPoker game;
    CFRSolver solver(game);

    solver.train(10000);

    for (const auto& [key, node] : solver.nodes()) {
        const Strategy strategy =
            node.averageStrategy();

        ASSERT_GT(strategy.size(), 0);

        double probabilitySum = 0.0;

        for (double probability : strategy.probabilities) {
            EXPECT_GE(probability, 0.0);
            EXPECT_LE(probability, 1.0);

            probabilitySum += probability;
        }

        EXPECT_NEAR(
            probabilitySum,
            1.0,
            1e-9
        );
    }
}

TEST(EndToEndTest, InformationSetsHideOpponentCard) {
    GameState stateA;

    stateA.setPrivateCard(
        0,
        Card{Rank::King, Suit::Clubs}
    );

    stateA.setPrivateCard(
        1,
        Card{Rank::Jack, Suit::Clubs}
    );

    GameState stateB;

    stateB.setPrivateCard(
        0,
        Card{Rank::King, Suit::Clubs}
    );

    stateB.setPrivateCard(
        1,
        Card{Rank::Queen, Suit::Clubs}
    );

    InformationSet infoA =
        InformationSet::fromState(stateA, 0);

    InformationSet infoB =
        InformationSet::fromState(stateB, 0);

    /*
     * Player 0 knows that they hold K, but cannot distinguish
     * whether Player 1 holds J or Q.
     */
    EXPECT_EQ(
        infoA.key(),
        infoB.key()
    );
}

TEST(EndToEndTest, DifferentPrivateCardsProduceDifferentInformationSets) {
    GameState stateA;

    stateA.setPrivateCard(
        0,
        Card{Rank::King, Suit::Clubs}
    );

    stateA.setPrivateCard(
        1,
        Card{Rank::Jack, Suit::Clubs}
    );

    GameState stateB;

    stateB.setPrivateCard(
        0,
        Card{Rank::Queen, Suit::Clubs}
    );

    stateB.setPrivateCard(
        1,
        Card{Rank::Jack, Suit::Clubs}
    );

    InformationSet infoA =
        InformationSet::fromState(stateA, 0);

    InformationSet infoB =
        InformationSet::fromState(stateB, 0);

    EXPECT_NE(
        infoA.key(),
        infoB.key()
    );
}