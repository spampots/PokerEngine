#include <gtest/gtest.h>

#include "poker/Card.hpp"

using namespace poker;

TEST(CardTest, RankAndSuitAreStoredCorrectly) {
    Card card{Rank::King, Suit::Spades};

    EXPECT_EQ(card.rank, Rank::King);
    EXPECT_EQ(card.suit, Suit::Spades);
}

TEST(CardTest, CardsCompareCorrectly) {
    Card kingSpades{Rank::King, Suit::Spades};
    Card kingSpadesCopy{Rank::King, Suit::Spades};
    Card queenSpades{Rank::Queen, Suit::Spades};

    EXPECT_EQ(kingSpades, kingSpadesCopy);
    EXPECT_NE(kingSpades, queenSpades);
}

TEST(CardTest, ToStringWorks) {
    EXPECT_EQ(
        Card{Rank::King, Suit::Spades}.toString(),
        "KS"
    );

    EXPECT_EQ(
        Card{Rank::Queen, Suit::Hearts}.toString(),
        "QH"
    );

    EXPECT_EQ(
        Card{Rank::Jack, Suit::Clubs}.toString(),
        "JC"
    );
}