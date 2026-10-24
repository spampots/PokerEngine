#include <gtest/gtest.h>

#include "poker/CFR.hpp"
#include "poker/Evaluation.hpp"
#include "poker/KuhnPoker.hpp"

#include <memory>

using namespace poker;

namespace {

constexpr double EQUILIBRIUM_VALUE = -1.0 / 18.0;

} // namespace

// ============================================================================
// Seeding / determinism
// ============================================================================

TEST(SeedingTest, SameSeedDealsIdentically) {
    KuhnPoker gameA(42);
    KuhnPoker gameB(42);

    for (int i = 0; i < 100; ++i) {
        const GameState a = gameA.initialState();
        const GameState b = gameB.initialState();

        EXPECT_EQ(a.privateCard(0), b.privateCard(0));
        EXPECT_EQ(a.privateCard(1), b.privateCard(1));
    }
}

TEST(SeedingTest, SameSeedTrainsIdentically) {
    KuhnPoker gameA(42);
    KuhnPoker gameB(42);

    CFRSolver solverA(gameA);
    CFRSolver solverB(gameB);

    solverA.train(1000);
    solverB.train(1000);

    ASSERT_EQ(
        solverA.informationSetCount(),
        solverB.informationSetCount()
    );

    for (const auto& [key, node] : solverA.nodes()) {
        const auto it = solverB.nodes().find(key);

        ASSERT_NE(it, solverB.nodes().end()) << key;

        EXPECT_EQ(node.regretSum(), it->second.regretSum()) << key;
        EXPECT_EQ(node.strategySum(), it->second.strategySum()) << key;
    }
}

TEST(SeedingTest, FixedDealUsesGivenCards) {
    KuhnPoker game(0);

    const GameState state = game.initialState(
        Card(Rank::Queen, Suit::Clubs),
        Card(Rank::King, Suit::Clubs)
    );

    EXPECT_EQ(state.privateCard(0), Card(Rank::Queen, Suit::Clubs));
    EXPECT_EQ(state.privateCard(1), Card(Rank::King, Suit::Clubs));
    EXPECT_EQ(state.currentPlayer(), 0);
    EXPECT_EQ(state.pot(), 2);
}

TEST(SeedingTest, FixedDealRejectsDuplicateCards) {
    KuhnPoker game(0);

    EXPECT_THROW(
        game.initialState(
            Card(Rank::King, Suit::Clubs),
            Card(Rank::King, Suit::Clubs)
        ),
        std::invalid_argument
    );
}

// ============================================================================
// Evaluation invariants
// ============================================================================

TEST(EvaluationTest, ExpectedValuesAreZeroSum) {
    KuhnPoker game(1);
    CFRSolver solver(game);

    solver.train(500);

    EXPECT_NEAR(
        expectedValue(game, solver, 0) +
            expectedValue(game, solver, 1),
        0.0,
        1e-12
    );
}

TEST(EvaluationTest, BestResponseIsAtLeastExpectedValue) {
    KuhnPoker game(1);
    CFRSolver solver(game);

    solver.train(500);

    for (int player = 0; player < 2; ++player) {
        EXPECT_GE(
            bestResponseValue(game, solver, player),
            expectedValue(game, solver, player) - 1e-12
        );
    }

    EXPECT_GE(exploitability(game, solver), 0.0);
}

TEST(EvaluationTest, UniformStrategyHasKnownExploitability) {
    /*
     * With no training every information set is uniform random.
     * The known exploitability of that profile in Kuhn Poker is
     * 11/24 (NashConv 11/12), which checks the best-response
     * computation against an independent reference.
     */
    KuhnPoker game(1);
    CFRSolver solver(game);

    EXPECT_NEAR(exploitability(game, solver), 11.0 / 24.0, 1e-12);
}

// ============================================================================
// Convergence to the Kuhn Poker equilibrium
// ============================================================================

/*
 * Trains once and shares the solver across the convergence
 * tests.
 *
 * Kuhn Poker has a one-parameter family of equilibria. Player 0
 * bets a Jack with some probability alpha in [0, 1/3]; the rest
 * of the strategy is determined by alpha, and Player 1's
 * strategy is unique.
 */
class KuhnConvergenceTest : public ::testing::Test {
protected:
    static void SetUpTestSuite() {
        game_ = std::make_unique<KuhnPoker>(12345);
        solver_ = std::make_unique<CFRSolver>(*game_);

        solver_->train(100000);
    }

    static void TearDownTestSuite() {
        solver_.reset();
        game_.reset();
    }

    static double probability(
        const char* key,
        std::size_t action
    ) {
        const Strategy strategy =
            solver_->averageStrategy(key);

        EXPECT_EQ(strategy.size(), 2u) << key;

        return strategy.size() == 2 ? strategy[action] : -1.0;
    }

    static std::unique_ptr<KuhnPoker> game_;
    static std::unique_ptr<CFRSolver> solver_;
};

std::unique_ptr<KuhnPoker> KuhnConvergenceTest::game_;
std::unique_ptr<CFRSolver> KuhnConvergenceTest::solver_;

TEST_F(KuhnConvergenceTest, ExploitabilityIsNearZero) {
    EXPECT_LT(exploitability(*game_, *solver_), 0.01);
}

TEST_F(KuhnConvergenceTest, GameValueIsMinusOneEighteenth) {
    EXPECT_NEAR(
        expectedValue(*game_, *solver_, 0),
        EQUILIBRIUM_VALUE,
        0.005
    );
}

TEST_F(KuhnConvergenceTest, ExploitabilityDecreasesWithTraining) {
    KuhnPoker game(12345);
    CFRSolver shortRun(game);

    shortRun.train(1000);

    EXPECT_LT(
        exploitability(*game_, *solver_),
        exploitability(game, shortRun)
    );
}

TEST_F(KuhnConvergenceTest, FirstPlayerStrategyMatchesEquilibrium) {
    /*
     * Opening decision: [Check, Bet].
     */
    const double alpha = probability("JC|", 1);

    EXPECT_GE(alpha, 0.0);
    EXPECT_LE(alpha, 1.0 / 3.0 + 0.02);

    EXPECT_NEAR(probability("KC|", 1), 3.0 * alpha, 0.05);
    EXPECT_LT(probability("QC|", 1), 0.05);

    /*
     * Facing check-bet: [Call, Fold].
     */
    EXPECT_GT(probability("JC|C B1", 1), 0.95);
    EXPECT_NEAR(probability("QC|C B1", 0), alpha + 1.0 / 3.0, 0.05);
    EXPECT_GT(probability("KC|C B1", 0), 0.95);
}

TEST_F(KuhnConvergenceTest, SecondPlayerStrategyMatchesEquilibrium) {
    /*
     * Facing a bet: [Call, Fold].
     */
    EXPECT_GT(probability("JC|B1", 1), 0.95);
    EXPECT_NEAR(probability("QC|B1", 0), 1.0 / 3.0, 0.05);
    EXPECT_GT(probability("KC|B1", 0), 0.95);

    /*
     * Facing a check: [Check, Bet].
     */
    EXPECT_NEAR(probability("JC|C", 1), 1.0 / 3.0, 0.05);
    EXPECT_LT(probability("QC|C", 1), 0.05);
    EXPECT_GT(probability("KC|C", 1), 0.95);
}
