#include "../../include/cards/Card.hpp"

namespace poker
{

    Card::Card(Rank rank)
        : rank_(rank) {}

    Rank Card::rank() const
    {
        return rank_;
    }

} // namespace poker