#pragma once

#include <cstdint>
#include <string>

namespace poker {

enum class Suit : std::uint8_t {
    Clubs,
    Diamonds,
    Hearts,
    Spades
};

enum class Rank : std::uint8_t {
    Two = 2,
    Three,
    Four,
    Five,
    Six,
    Seven,
    Eight,
    Nine,
    Ten,
    Jack,
    Queen,
    King,
    Ace
};

struct Card {
    Rank rank;
    Suit suit;

    constexpr Card(Rank r, Suit s) noexcept
        : rank(r), suit(s) {}

    constexpr bool operator==(const Card& other) const noexcept {
        return rank == other.rank && suit == other.suit;
    }

    constexpr bool operator!=(const Card& other) const noexcept {
        return !(*this == other);
    }

    std::string toString() const;
};

} // namespace poker