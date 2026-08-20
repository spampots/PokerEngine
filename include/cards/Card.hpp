#pragma once

namespace poker
{
    enum class Rank
    {
        Jack = 1,
        Queen = 2,
        King = 3
    };

    class Card
    {
    public:
        explicit Card(Rank rank);

        Rank rank() const;

    private:
        Rank rank_;
    };
} // namespace poker