#include <gtest/gtest.h>

#include "poker/CFR.hpp"

using namespace poker;

TEST(StrategyTest, UniformStrategyWhenNoPositiveRegret) {
    Strategy strategy =
        Strategy::fromRegrets({-1.0, -2.0});

    ASSERT_EQ(strategy.size(), 2);

    EXPECT_DOUBLE_EQ(
        strategy[0],
        0.5
    );

    EXPECT_DOUBLE_EQ(
        strategy[1],
        0.5
    );
}

TEST(StrategyTest, RegretMatchingUsesPositiveRegrets) {
    Strategy strategy =
        Strategy::fromRegrets({1.0, 3.0});

    ASSERT_EQ(strategy.size(), 2);

    EXPECT_DOUBLE_EQ(
        strategy[0],
        0.25
    );

    EXPECT_DOUBLE_EQ(
        strategy[1],
        0.75
    );
}

TEST(StrategyTest, NegativeRegretsAreIgnored) {
    Strategy strategy =
        Strategy::fromRegrets({-5.0, 2.0});

    EXPECT_DOUBLE_EQ(
        strategy[0],
        0.0
    );

    EXPECT_DOUBLE_EQ(
        strategy[1],
        1.0
    );
}

TEST(CFRNodeTest, InitialStrategyIsUniform) {
    CFRNode node(2);

    Strategy strategy =
        node.currentStrategy();

    ASSERT_EQ(strategy.size(), 2);

    EXPECT_DOUBLE_EQ(
        strategy[0],
        0.5
    );

    EXPECT_DOUBLE_EQ(
        strategy[1],
        0.5
    );
}

TEST(CFRNodeTest, RegretChangesStrategy) {
    CFRNode node(2);

    node.addRegret(0, 3.0);

    Strategy strategy =
        node.currentStrategy();

    EXPECT_DOUBLE_EQ(
        strategy[0],
        1.0
    );

    EXPECT_DOUBLE_EQ(
        strategy[1],
        0.0
    );
}

TEST(CFRNodeTest, AverageStrategyIsComputed) {
    CFRNode node(2);

    Strategy strategy(2);

    strategy[0] = 0.25;
    strategy[1] = 0.75;

    node.accumulateStrategy(
        strategy,
        1.0
    );

    Strategy average =
        node.averageStrategy();

    EXPECT_DOUBLE_EQ(
        average[0],
        0.25
    );

    EXPECT_DOUBLE_EQ(
        average[1],
        0.75
    );
}

TEST(CFRSolverTest, InitiallyHasNoInformationSets) {
    KuhnPoker game;
    CFRSolver solver(game);

    EXPECT_EQ(
        solver.informationSetCount(),
        0
    );

    EXPECT_EQ(
        solver.iterations(),
        0
    );
}

TEST(CFRSolverTest, TrainingDiscoversInformationSets) {
    KuhnPoker game;
    CFRSolver solver(game);

    solver.train(100);

    EXPECT_EQ(
        solver.iterations(),
        100
    );

    EXPECT_GT(
        solver.informationSetCount(),
        0
    );
}

TEST(CFRSolverTest, AverageStrategiesAreValidProbabilities) {
    KuhnPoker game;
    CFRSolver solver(game);

    solver.train(1000);

    for (const auto& [key, node] : solver.nodes()) {
        Strategy strategy =
            node.averageStrategy();

        if (strategy.size() == 0) {
            continue;
        }

        double sum = 0.0;

        for (double probability : strategy.probabilities) {
            EXPECT_GE(probability, 0.0);
            EXPECT_LE(probability, 1.0);

            sum += probability;
        }

        EXPECT_NEAR(
            sum,
            1.0,
            1e-9
        );
    }
}

TEST(CFRSolverTest, ResetClearsTrainingState) {
    KuhnPoker game;
    CFRSolver solver(game);

    solver.train(100);

    ASSERT_GT(
        solver.informationSetCount(),
        0
    );

    solver.reset();

    EXPECT_EQ(
        solver.iterations(),
        0
    );

    EXPECT_EQ(
        solver.informationSetCount(),
        0
    );
}