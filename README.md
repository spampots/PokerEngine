# Poker Engine

A modular C++ poker engine implementing Kuhn Poker and Counterfactual Regret Minimization (CFR) for learning approximate equilibrium strategies in an imperfect-information game.

## Overview

This project implements a small poker engine designed around explicit game-state and information-set abstractions.

The engine currently supports:

- Card and action representations
- Player state and betting contributions
- Pot tracking
- Rule-based game transitions
- Kuhn Poker
- Hidden-information modeling through information sets
- Counterfactual Regret Minimization
- Average-strategy computation
- Exact game-value and exploitability evaluation
- Seedable, reproducible training
- Unit and integration testing

The architecture separates generic game state from game-specific rules and the CFR learning algorithm, making the system extensible to other imperfect-information poker variants.

## Project Structure

```text
poker-engine/
├── CMakeLists.txt
├── README.md
│
├── include/
│   └── poker/
│       ├── Card.hpp
│       ├── CFR.hpp
│       ├── Evaluation.hpp
│       ├── GameState.hpp
│       ├── InformationSet.hpp
│       └── KuhnPoker.hpp
│
├── src/
│   ├── Card.cpp
│   ├── CFR.cpp
│   ├── Evaluation.cpp
│   ├── GameState.cpp
│   ├── InformationSet.cpp
│   └── KuhnPoker.cpp
│
├── tests/
│   ├── test_cards.cpp
│   ├── test_game_state.cpp
│   ├── test_kuhn.cpp
│   ├── test_cfr.cpp
│   ├── test_end_to_end.cpp
│   └── test_evaluation.cpp
│
└── experiments/
    └── train_kuhn.cpp