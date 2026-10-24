#include "poker/CFR.hpp"
#include "poker/Evaluation.hpp"
#include "poker/InformationSet.hpp"
#include "poker/KuhnPoker.hpp"

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <random>
#include <string>

namespace {

void printStrategy(
    const poker::CFRSolver& solver,
    const std::string& key
) {
    const poker::Strategy strategy =
        solver.averageStrategy(key);

    if (strategy.size() == 0) {
        std::cout << "  " << key << ": unavailable\n";
        return;
    }

    std::cout << "  " << key << ": [";

    for (std::size_t i = 0; i < strategy.size(); ++i) {
        if (i > 0) {
            std::cout << ", ";
        }

        std::cout << std::fixed
                  << std::setprecision(4)
                  << strategy[i];
    }

    std::cout << "]\n";
}

} // namespace

int main(int argc, char* argv[]) {
    /*
     * Optional first argument: RNG seed, for reproducible runs.
     */
    const std::uint32_t seed =
        argc > 1
            ? static_cast<std::uint32_t>(std::stoul(argv[1]))
            : std::random_device{}();

    poker::KuhnPoker game(seed);
    poker::CFRSolver solver(game);

    constexpr std::size_t iterations = 100000;

    std::cout << "Kuhn Poker CFR Solver\n";
    std::cout << "=====================\n\n";

    std::cout
        << "Seed: "
        << seed
        << '\n';

    std::cout
        << "Training for "
        << iterations
        << " iterations...\n";

    solver.train(iterations);

    std::cout << "Training complete.\n\n";

    std::cout
        << "Iterations: "
        << solver.iterations()
        << '\n';

    std::cout
        << "Information sets: "
        << solver.informationSetCount()
        << "\n\n";

    std::cout
        << std::fixed
        << std::setprecision(6)
        << "Player 0 game value: "
        << poker::expectedValue(game, solver, 0)
        << "  (equilibrium: "
        << -1.0 / 18.0
        << ")\n";

    std::cout
        << "Exploitability:      "
        << poker::exploitability(game, solver)
        << "  (equilibrium: 0)\n\n";

    std::cout << "Learned average strategies:\n";

    /*
     * Initial decision information sets.
     *
     * The exact key includes the suit because Card::toString()
     * is generic, although suits are strategically irrelevant
     * in Kuhn Poker.
     */
    printStrategy(solver, "JC|");
    printStrategy(solver, "QC|");
    printStrategy(solver, "KC|");

    std::cout << "\nAll discovered information sets:\n";

    for (const auto& [key, node] : solver.nodes()) {
        const auto strategy =
            node.averageStrategy();

        std::cout << "  "
                  << key
                  << " -> [";

        for (std::size_t i = 0;
             i < strategy.size();
             ++i) {

            if (i > 0) {
                std::cout << ", ";
            }

            std::cout
                << std::fixed
                << std::setprecision(4)
                << strategy[i];
        }

        std::cout << "]\n";
    }

    return 0;
}