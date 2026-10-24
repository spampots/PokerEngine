#pragma once

#include "CFR.hpp"
#include "KuhnPoker.hpp"

namespace poker {

/*
 * Exact evaluation of a learned Kuhn Poker strategy profile.
 *
 * Every chance outcome (all 6 deals) is enumerated, so these
 * values are exact rather than sampled. Information sets the
 * solver has never visited are treated as uniform random.
 */

/*
 * Expected utility for a player when both players follow the
 * solver's average strategy.
 *
 * At the Kuhn Poker equilibrium, Player 0's value is -1/18.
 */
double expectedValue(
    const KuhnPoker& game,
    const CFRSolver& solver,
    int player
);

/*
 * Expected utility for a player who plays a best response
 * against the opponent's average strategy.
 *
 * The best response is restricted to information sets: the
 * responder cannot see the opponent's card.
 */
double bestResponseValue(
    const KuhnPoker& game,
    const CFRSolver& solver,
    int player
);

/*
 * How much a best-responding opponent gains, on average over
 * both seats:
 *
 *     (bestResponseValue(0) + bestResponseValue(1)) / 2
 *
 * Always non-negative, and zero exactly at a Nash equilibrium.
 */
double exploitability(
    const KuhnPoker& game,
    const CFRSolver& solver
);

} // namespace poker
