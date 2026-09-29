#include "poker/Card.hpp"

#include <array>
#include <stdexcept>

namespace poker {

std::string Card::toString() const {
    static constexpr std::array<const char*, 13> rankNames{
        "2", "3", "4", "5", "6", "7", "8",
        "9", "T", "J", "Q", "K", "A"
    };

    static constexpr std::array<const char*, 4> suitNames{
        "C", "D", "H", "S"
    };

    const auto rankIndex =
        static_cast<int>(rank) - static_cast<int>(Rank::Two);

    const auto suitIndex =
        static_cast<int>(suit);

    if (rankIndex < 0 || rankIndex >= static_cast<int>(rankNames.size()) ||
        suitIndex < 0 || suitIndex >= static_cast<int>(suitNames.size())) {
        throw std::runtime_error("Invalid card");
    }

    return std::string(rankNames[rankIndex]) + suitNames[suitIndex];
}

} // namespace poker