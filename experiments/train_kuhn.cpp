#include "poker/CFR.hpp"
#include "poker/InformationSet.hpp"
#include "poker/KuhnPoker.hpp"

#include <iomanip>
#include <iostream>
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

int main() {
    poker::KuhnPoker game;
    poker::CFRSolver solver(game);

    constexpr std::size_t iterations = 100000;

    std::cout << "Kuhn Poker CFR Solver\n";
    std::cout << "=====================\n\n";

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