#pragma once

#include "InformationSet.hpp"
#include "KuhnPoker.hpp"

#include <cstddef>
#include <string>
#include <unordered_map>
#include <vector>

namespace poker {

/*
 * A strategy for an information set.
 *
 * For Kuhn Poker there are at most two legal actions at a
 * decision point, but this representation remains generic.
 */
struct Strategy {
    std::vector<double> probabilities;

    Strategy() = default;

    explicit Strategy(std::size_t actionCount);

    /*
     * Returns a valid regret-matching strategy.
     */
    static Strategy fromRegrets(
        const std::vector<double>& regretSum
    );

    /*
     * Normalizes probabilities so that they sum to 1.
     */
    void normalize();

    double& operator[](std::size_t index);

    double operator[](std::size_t index) const;

    std::size_t size() const noexcept {
        return probabilities.size();
    }
};

/*
 * CFR data associated with one information set.
 *
 * regretSum:
 *     cumulative counterfactual regret
 *
 * strategySum:
 *     cumulative strategy used for average-strategy computation
 */
class CFRNode {
public:
    CFRNode() = default;

    explicit CFRNode(std::size_t actionCount);

    /*
     * Current strategy according to regret matching.
     */
    Strategy currentStrategy() const;

    /*
     * Add regret for an action.
     */
    void addRegret(
        std::size_t action,
        double regret
    );

    /*
     * Accumulate the current strategy for average-strategy
     * computation.
     */
    void accumulateStrategy(
        const Strategy& strategy,
        double reachProbability
    );

    /*
     * Average strategy over all training iterations.
     */
    Strategy averageStrategy() const;

    const std::vector<double>& regretSum() const noexcept;

    const std::vector<double>& strategySum() const noexcept;

private:
    std::vector<double> regretSum_;
    std::vector<double> strategySum_;
};

/*
 * Counterfactual Regret Minimization solver.
 *
 * This implementation performs vanilla CFR over Kuhn Poker.
 */
class CFRSolver {
public:
    explicit CFRSolver(
        const KuhnPoker& game
    );

    /*
     * Run CFR for the specified number of iterations.
     */
    void train(std::size_t iterations);

    /*
     * Run exactly one CFR iteration.
     */
    void trainIteration();

    /*
     * Retrieve the average strategy learned for an
     * information set.
     */
    Strategy averageStrategy(
        const InformationSet& informationSet
    ) const;

    /*
     * Retrieve the average strategy by its string key.
     */
    Strategy averageStrategy(
        const std::string& informationSet
    ) const;

    /*
     * Number of iterations completed.
     */
    std::size_t iterations() const noexcept;

    /*
     * Number of information sets discovered.
     */
    std::size_t informationSetCount() const noexcept;

    /*
     * Access the complete strategy table.
     */
    const std::unordered_map<
        std::string,
        CFRNode
    >& nodes() const noexcept;

    /*
     * Reset all learned regrets and strategies.
     */
    void reset();

private:
    /*
     * Core CFR recursion.
     *
     * reachProbability[0] and reachProbability[1] represent
     * the probability of reaching this state under each
     * player's strategy.
     */
    double cfr(
        const GameState& state,
        double reachProbability0,
        double reachProbability1
    );

    /*
     * Get or create the CFR node for an information set.
     */
    CFRNode& getNode(
        const InformationSet& informationSet,
        std::size_t actionCount
    );

    const CFRNode* findNode(
        const InformationSet& informationSet
    ) const;

private:
    const KuhnPoker& game_;

    std::unordered_map<
        std::string,
        CFRNode
    > nodes_;

    std::size_t iterations_ = 0;
};

} // namespace poker