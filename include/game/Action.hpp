#pragma once

namespace poker {

enum class ActionType {
    Check,
    Bet,
    Fold,
    Call
};

class Action {
public:
    explicit Action(ActionType type);

    ActionType type() const;

private:
    ActionType type_;
};

} // namespace poker