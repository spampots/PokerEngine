#include "poker/Evaluation.hpp"

#include "poker/InformationSet.hpp"

#include <algorithm>
#include <limits>
#include <map>
#include <string>
#include <vector>

namespace poker {

namespace {

/*
 * A state together with the probability of reaching it from
 * the chance node and the non-responding player's actions.
 */
struct WeightedState {
    GameState state;
    double weight;
};

/*
 * Average strategy at the state's information set, or uniform
 * if the solver has never visited it.
 */
Strategy profileStrategy(
    const CFRSolver& solver,
    const GameState& state,
    std::size_t actionCount
) {
    Strategy strategy =
        solver.averageStrategy(
            InformationSet::fromState(
                state,
                state.currentPlayer()
            )
        );

    if (strategy.size() != actionCount) {
        strategy = Strategy(actionCount);
        strategy.normalize();
    }

    return strategy;
}

/*
 * All 6 ordered deals, each with probability 1/6.
 */
std::vector<WeightedState> allDeals(
    const KuhnPoker& game
) {
    const auto cards = KuhnPoker::deck();

    std::vector<WeightedState> deals;

    for (std::size_t i = 0; i < cards.size(); ++i) {
        for (std::size_t j = 0; j < cards.size(); ++j) {
            if (i == j) {
                continue;
            }

            deals.push_back({
                game.initialState(cards[i], cards[j]),
                0.0
            });
        }
    }

    for (auto& deal : deals) {
        deal.weight =
            1.0 / static_cast<double>(deals.size());
    }

    return deals;
}

double expectedValueFromState(
    const KuhnPoker& game,
    const CFRSolver& solver,
    const GameState& state,
    int player
) {
    if (game.isTerminal(state)) {
        return static_cast<double>(
            game.utility(state, player)
        );
    }

    const std::vector<Action> actions =
        game.legalActions(state);

    const Strategy strategy =
        profileStrategy(solver, state, actions.size());

    double value = 0.0;

    for (std::size_t i = 0; i < actions.size(); ++i) {
        GameState next = state;

        game.applyAction(next, actions[i]);

        value +=
            strategy[i] *
            expectedValueFromState(
                game,
                solver,
                next,
                player
            );
    }

    return value;
}

/*
 * Best-response recursion over a set of states that share the
 * same public history (so they agree on whose turn it is, which
 * actions are legal and whether the game is over).
 *
 * Returns the reach-weighted sum of the responder's utility.
 */
double bestResponseFromStates(
    const KuhnPoker& game,
    const CFRSolver& solver,
    const std::vector<WeightedState>& states,
    int responder
) {
    const GameState& representative =
        states.front().state;

    if (game.isTerminal(representative)) {
        double value = 0.0;

        for (const auto& weighted : states) {
            value +=
                weighted.weight *
                game.utility(weighted.state, responder);
        }

        return value;
    }

    const std::vector<Action> actions =
        game.legalActions(representative);

    /*
     * Opponent to act: follow their average strategy, scaling
     * each state's weight by the probability of each action.
     */
    if (representative.currentPlayer() != responder) {
        double value = 0.0;

        for (std::size_t i = 0; i < actions.size(); ++i) {
            std::vector<WeightedState> children;

            for (const auto& weighted : states) {
                const Strategy strategy =
                    profileStrategy(
                        solver,
                        weighted.state,
                        actions.size()
                    );

                GameState next = weighted.state;

                game.applyAction(next, actions[i]);

                children.push_back({
                    next,
                    weighted.weight * strategy[i]
                });
            }

            value +=
                bestResponseFromStates(
                    game,
                    solver,
                    children,
                    responder
                );
        }

        return value;
    }

    /*
     * Responder to act: they cannot see the opponent's card,
     * so states are grouped by the responder's information set
     * and one action is chosen per group.
     */
    std::map<std::string, std::vector<WeightedState>> groups;

    for (const auto& weighted : states) {
        groups[
            InformationSet::fromState(
                weighted.state,
                responder
            ).key()
        ].push_back(weighted);
    }

    double value = 0.0;

    for (const auto& [key, group] : groups) {
        double best =
            -std::numeric_limits<double>::infinity();

        for (const Action& action : actions) {
            std::vector<WeightedState> children;

            for (const auto& weighted : group) {
                GameState next = weighted.state;

                game.applyAction(next, action);

                children.push_back({
                    next,
                    weighted.weight
                });
            }

            best = std::max(
                best,
                bestResponseFromStates(
                    game,
                    solver,
                    children,
                    responder
                )
            );
        }

        value += best;
    }

    return value;
}

} // namespace

double expectedValue(
    const KuhnPoker& game,
    const CFRSolver& solver,
    int player
) {
    double value = 0.0;

    for (const auto& deal : allDeals(game)) {
        value +=
            deal.weight *
            expectedValueFromState(
                game,
                solver,
                deal.state,
                player
            );
    }

    return value;
}

double bestResponseValue(
    const KuhnPoker& game,
    const CFRSolver& solver,
    int player
) {
    return bestResponseFromStates(
        game,
        solver,
        allDeals(game),
        player
    );
}

double exploitability(
    const KuhnPoker& game,
    const CFRSolver& solver
) {
    return (
        bestResponseValue(game, solver, 0) +
        bestResponseValue(game, solver, 1)
    ) / 2.0;
}

} // namespace poker
