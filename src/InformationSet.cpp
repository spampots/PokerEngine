#include "poker/InformationSet.hpp"

#include <stdexcept>
#include <utility>

namespace poker {

InformationSet::InformationSet(std::string key)
    : key_(std::move(key)) {}

InformationSet InformationSet::fromState(
    const GameState& state,
    int player
) {
    if (player < 0 || player >= GameState::NUM_PLAYERS) {
        throw std::out_of_range("Invalid player index");
    }

    if (!state.hasPrivateCard(player)) {
        throw std::runtime_error(
            "Cannot construct information set without private card"
        );
    }

    /*
     * Information set:
     *
     *     private card | public betting history
     *
     * Example:
     *
     *     K|
     *     Q|B
     *     J|Bc
     *
     * The opponent's private card is intentionally omitted.
     */
    std::string key =
        state.privateCard(player).toString();

    key += "|";
    key += state.historyString();

    return InformationSet(std::move(key));
}

const std::string& InformationSet::key() const noexcept {
    return key_;
}

bool InformationSet::operator==(
    const InformationSet& other
) const noexcept {
    return key_ == other.key_;
}

bool InformationSet::operator!=(
    const InformationSet& other
) const noexcept {
    return !(*this == other);
}

} // namespace poker