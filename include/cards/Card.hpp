#pragma once

namespace poker {

enum class Rank {
    Jack,   
    Queen,
    King
};

class Card {
public:
    explicit Card(Rank rank);

    Rank rank() const;

private:
    Rank rank_;
};

} // namespace poker