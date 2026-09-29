#include "poker/CFR.hpp"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <stdexcept>

namespace poker {

namespace {

constexpr double EPSILON = 1e-12;

} // namespace

// ============================================================================
// Strategy
// ============================================================================

Strategy::Strategy(std::size_t actionCount)
    : probabilities(actionCount, 0.0) {}

Strategy Strategy::fromRegrets(
    const std::vector<double>& regretSum
) {
    Strategy result(regretSum.size());

    double positiveRegretSum = 0.0;

    for (double regret : regretSum) {
        positiveRegretSum += std::max(0.0, regret);
    }

    /*
     * Regret matching:
     *
     *     sigma(a) =
     *         R+(a) / sum_b R+(b)
     *
     * If all regrets are non-positive, use a uniform strategy.
     */
    if (positiveRegretSum <= EPSILON) {
        const double probability =
            1.0 / static_cast<double>(result.size());

        std::fill(
            result.probabilities.begin(),
            result.probabilities.end(),
            probability
        );

        return result;
    }

    for (std::size_t i = 0; i < regretSum.size(); ++i) {
        result[i] =
            std::max(0.0, regretSum[i]) /
            positiveRegretSum;
    }

    return result;
}

void Strategy::normalize() {
    if (probabilities.empty()) {
        return;
    }

    double sum = std::accumulate(
        probabilities.begin(),
        probabilities.end(),
        0.0
    );

    if (sum <= EPSILON) {
        const double uniform =
            1.0 / static_cast<double>(probabilities.size());

        std::fill(
            probabilities.begin(),
            probabilities.end(),
            uniform
        );

        return;
    }

    for (double& probability : probabilities) {
        probability /= sum;
    }
}

double& Strategy::operator[](std::size_t index) {
    return probabilities.at(index);
}

double Strategy::operator[](std::size_t index) const {
    return probabilities.at(index);
}

// ============================================================================
// CFRNode
// ============================================================================

CFRNode::CFRNode(std::size_t actionCount)
    : regretSum_(actionCount, 0.0),
      strategySum_(actionCount, 0.0) {}

Strategy CFRNode::currentStrategy() const {
    return Strategy::fromRegrets(regretSum_);
}

void CFRNode::addRegret(
    std::size_t action,
    double regret
) {
    if (action >= regretSum_.size()) {
        throw std::out_of_range(
            "CFR action index out of range"
        );
    }

    regretSum_[action] += regret;
}

void CFRNode::accumulateStrategy(
    const Strategy& strategy,
    double reachProbability
) {
    if (strategy.size() != strategySum_.size()) {
        throw std::invalid_argument(
            "Strategy size does not match CFR node"
        );
    }

    for (std::size_t i = 0; i < strategy.size(); ++i) {
        strategySum_[i] +=
            reachProbability * strategy[i];
    }
}

Strategy CFRNode::averageStrategy() const {
    Strategy result(strategySum_.size());

    if (strategySum_.empty()) {
        return result;
    }

    const double total =
        std::accumulate(
            strategySum_.begin(),
            strategySum_.end(),
            0.0
        );

    if (total <= EPSILON) {
        const double uniform =
            1.0 / static_cast<double>(strategySum_.size());

        std::fill(
            result.probabilities.begin(),
            result.probabilities.end(),
            uniform
        );

        return result;
    }

    for (std::size_t i = 0; i < strategySum_.size(); ++i) {
        result[i] =
            strategySum_[i] / total;
    }

    return result;
}

const std::vector<double>& CFRNode::regretSum() const noexcept {
    return regretSum_;
}

const std::vector<double>& CFRNode::strategySum() const noexcept {
    return strategySum_;
}

// ============================================================================
// CFRSolver
// ============================================================================

CFRSolver::CFRSolver(
    const KuhnPoker& game
)
    : game_(game) {}

void CFRSolver::train(
    std::size_t iterations
) {
    for (std::size_t i = 0; i < iterations; ++i) {
        trainIteration();
    }
}

void CFRSolver::trainIteration() {
    /*
     * Vanilla CFR alternates the traversing player.
     *
     * For two-player zero-sum games, each iteration traverses
     * the game tree from one player's perspective.
     */
    const GameState initialState =
        game_.initialState();

    cfr(
        initialState,
        1.0,
        1.0
    );

    ++iterations_;
}

Strategy CFRSolver::averageStrategy(
    const InformationSet& informationSet
) const {
    return averageStrategy(
        informationSet.key()
    );
}

Strategy CFRSolver::averageStrategy(
    const std::string& informationSet
) const {
    const auto it =
        nodes_.find(informationSet);

    if (it == nodes_.end()) {
        return {};
    }

    return it->second.averageStrategy();
}

std::size_t CFRSolver::iterations() const noexcept {
    return iterations_;
}

std::size_t CFRSolver::informationSetCount() const noexcept {
    return nodes_.size();
}

const std::unordered_map<
    std::string,
    CFRNode
>& CFRSolver::nodes() const noexcept {
    return nodes_;
}

void CFRSolver::reset() {
    nodes_.clear();
    iterations_ = 0;
}

double CFRSolver::cfr(
    const GameState& state,
    double reachProbability0,
    double reachProbability1
) {
    if (game_.isTerminal(state)) {
        /*
         * CFR utility is always from Player 0's perspective
         * in this implementation.
         */
        return static_cast<double>(
            game_.utility(state, 0)
        );
    }

    const int player =
        state.currentPlayer();

    const std::vector<Action> actions =
        game_.legalActions(state);

    if (actions.empty()) {
        throw std::runtime_error(
            "Non-terminal state has no legal actions"
        );
    }

    const InformationSet informationSet =
        InformationSet::fromState(
            state,
            player
        );

    CFRNode& node =
        getNode(
            informationSet,
            actions.size()
        );

    const Strategy strategy =
        node.currentStrategy();

    /*
     * Save utilities for every available action.
     */
    std::vector<double> actionUtilities(
        actions.size(),
        0.0
    );

    double nodeUtility = 0.0;

    for (std::size_t actionIndex = 0;
         actionIndex < actions.size();
         ++actionIndex) {

        GameState nextState = state;

        game_.applyAction(
            nextState,
            actions[actionIndex]
        );

        double nextReach0 =
            reachProbability0;

        double nextReach1 =
            reachProbability1;

        if (player == 0) {
            nextReach0 *= strategy[actionIndex];
        } else {
            nextReach1 *= strategy[actionIndex];
        }

        actionUtilities[actionIndex] =
            cfr(
                nextState,
                nextReach0,
                nextReach1
            );

        nodeUtility +=
            strategy[actionIndex] *
            actionUtilities[actionIndex];
    }

    /*
     * Counterfactual regret update.
     *
     * For Player 0:
     *
     *     regret(a) =
     *         utility(a) - utility(strategy)
     *
     * weighted by the opponent's reach probability.
     *
     * For Player 1, utility is represented from Player 0's
     * perspective, so the sign is reversed.
     */
    const double opponentReach =
        player == 0
            ? reachProbability1
            : reachProbability0;

    for (std::size_t actionIndex = 0;
         actionIndex < actions.size();
         ++actionIndex) {

        double regret =
            actionUtilities[actionIndex] -
            nodeUtility;

        if (player == 1) {
            regret = -regret;
        }

        node.addRegret(
            actionIndex,
            opponentReach * regret
        );
    }

    /*
     * Average strategy is weighted by the player's own reach
     * probability.
     */
    const double ownReach =
        player == 0
            ? reachProbability0
            : reachProbability1;

    node.accumulateStrategy(
        strategy,
        ownReach
    );

    return nodeUtility;
}

CFRNode& CFRSolver::getNode(
    const InformationSet& informationSet,
    std::size_t actionCount
) {
    const auto [it, inserted] =
        nodes_.try_emplace(
            informationSet.key(),
            CFRNode(actionCount)
        );

    if (!inserted &&
        it->second.currentStrategy().size() != actionCount) {
        throw std::runtime_error(
            "Information set encountered with inconsistent action count"
        );
    }

    return it->second;
}

const CFRNode* CFRSolver::findNode(
    const InformationSet& informationSet
) const {
    const auto it =
        nodes_.find(informationSet.key());

    if (it == nodes_.end()) {
        return nullptr;
    }

    return &it->second;
}

} // namespace poker