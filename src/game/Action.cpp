#include "../../include/game/Action.hpp"

namespace poker {

Action::Action(ActionType type)
    : type_(type) {}

ActionType Action::type() const {
    return type_;
}

} // namespace poker