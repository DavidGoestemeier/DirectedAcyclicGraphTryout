# � DAG RPG Attribute System - Academic Study Project

[![C++20](https://img.shields.io/badge/C++-20-blue.svg)](https://isocpp.org/)
[![CMake](https://img.shields.io/badge/CMake-3.16+-green.svg)](https://cmake.org/)
[![Tests](https://img.shields.io/badge/Tests-Google%20Test-red.svg)](https://github.com/google/googletest)
[![License](https://img.shields.io/badge/License-MIT-yellow.svg)](LICENSE)

> **Academic Research Project**: Advanced study in game engine architecture, data structures, and performance optimization patterns using Directed Acyclic Graphs (DAG) and dirty flag optimization.

## 📖 Project Overview

This is a **long-term academic study project** exploring the mathematical and architectural foundations of modern RPG attribute systems. The project investigates how game engines efficiently manage complex dependency graphs while maintaining real-time performance.

### 🎯 Learning Objectives

- **Data Structures**: Deep dive into DAG implementation and traversal algorithms
- **Design Patterns**: Dirty flag, observer, pull-based evaluation, builder pattern
- **Performance Optimization**: Lazy evaluation, caching strategies, and computational complexity
- **Software Engineering**: Test-driven development, code coverage, CI/CD practices
- **Real-time Systems**: WebSocket communication and live visualization
- **Modern C++**: C++20 features, smart pointers, lambdas, and functional programming

### 🔬 Research Focus Areas

1. **Graph Theory Applications**: DAG properties, topological sorting, cycle detection
2. **Computational Efficiency**: O(1) cached reads vs O(n) recomputation analysis
3. **Temporal Mechanics**: History nodes with rolling windows and decay functions
4. **Conditional Logic**: Tag-based systems and dynamic modifier application
5. **Architecture Patterns**: Comparison with ECS, traditional OOP, and data-oriented design

## 🏗️ System Architecture

```
┌─────────┐     ┌─────────┐     ┌─────────┐     ┌─────────┐
│   STR   │     │   DEX   │     │   INT   │     │   VIT   │
│ (Base)  │     │ (Base)  │     │ (Base)  │     │ (Base)  │
└────┬────┘     └────┬────┘     └────┬────┘     └────┬────┘
     │               │               │               │
     │    ┌──────────┼───────────────┤               │
     │    │          │               │               │
     ▼    ▼          ▼               ▼               ▼
┌─────────────┐  ┌─────────┐   ┌─────────┐     ┌─────────┐
│ Total ATK   │  │ Defense │   │ Crit %  │     │  Max HP │
│ Weapon+STR×2│  │ DEX+VIT │   │DEX+INT  │     │VIT×10+..│
└──────┬──────┘  └────┬────┘   └────┬────┘     └────┬────┘
       │              │             │               │
       ▼              │             │               │
┌─────────────┐       │             │               │
│    DPS      │◄──────┴─────────────┘               │
│ ATK×CritMod │                                     │
└─────────────┘                                     │
       │                                            │
       ▼                                            ▼
┌─────────────────────────────────────────────────────────┐
│                    Effective HP                         │
│              MaxHP × (1 + Defense/200)                  │
└─────────────────────────────────────────────────────────┘
```

## 🔴 The Dirty Flag in Action

When you change **STR**:
1. STR marks all dependents as **DIRTY** (red in visualizer)
2. Dependent nodes: Total ATK → DPS → Effective HP
3. When you **read** a dirty node, it recalculates
4. After recalculation, node turns **CLEAN** (green)

**This is the optimization!** - Only dirty nodes recalculate, clean nodes return cached values instantly.

## 🚀 Quick Start

### Prerequisites

- **CMake** 3.16 or higher
- **C++20** compatible compiler:
  - MSVC 2019+ (Windows)
  - GCC 10+ (Linux)
  - Clang 12+ (macOS)
- **Git** for version control
- Modern web browser (for visualization)

### 🔧 Development Setup

```bash
# Clone the repository
git clone <your-repo-url>
cd DAG_PROJECT_TRYOUT_1

# Configure CMake with testing enabled
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug -DENABLE_TESTING=ON -DENABLE_COVERAGE=ON

# Build the project
cmake --build build --config Debug

# Run tests
cd build
ctest --output-on-failure

# Run the main application
./bin/dag_rpg_system      # Linux/macOS
.\bin\Debug\dag_rpg_system.exe  # Windows
```

### 📊 Code Coverage

```bash
# Generate coverage report (Linux/GCC)
cmake --build build --target coverage

# View HTML report
open build/coverage/index.html  # macOS
xdg-open build/coverage/index.html  # Linux

# Windows: Use OpenCppCoverage or Visual Studio Enterprise
```

### 🧪 Running Tests

```bash
# Run all tests
ctest --test-dir build --output-on-failure

# Run tests in parallel
ctest --test-dir build -j8

# Run specific test
./build/bin/Debug/dag_rpg_tests --gtest_filter=StatNodeTest.*

# Run with verbose output
./build/bin/Debug/dag_rpg_tests --gtest_verbose
```

## 🛠️ VS Code Integration

This project is fully configured for VS Code with:

- **IntelliSense**: Full C++ code completion and error detection
- **Build Tasks**: CMake configure, build, test, and coverage (Ctrl+Shift+B)
- **Debugging**: Launch configurations for main app and tests (F5)
- **Testing**: Integrated test explorer
- **Coverage**: Coverage gutters showing line coverage
- **Formatting**: Auto-format on save with clang-format

### Recommended Extensions

Install the recommended extensions by opening the command palette (Ctrl+Shift+P) and selecting:
```
Extensions: Show Recommended Extensions
```

Key extensions:
- C/C++ Extension Pack (Microsoft)
- CMake Tools
- Test Explorer
- Coverage Gutters
- GitLens
- Better Comments

## 📚 Project Structure

```
DAG_PROJECT_TRYOUT_1/
├── include/              # Header files
│   ├── StatNode.h        # Core DAG node with dirty flag
│   ├── GraphManagerV2.h  # Graph management and operations
│   ├── Modifier.h        # Modifier system (Flat, Increased, More)
│   ├── Producer.h        # Items, Auras, and modifier sources
│   ├── HistoryNode.h     # Temporal nodes with rolling windows
│   ├── GameplayTag.h     # Tag-based condition system
│   └── WebSocketServer.h # Real-time visualization server
├── src/
│   └── main.cpp          # Application entry point and examples
├── tests/                # Unit tests (Google Test)
│   ├── test_StatNode.cpp
│   ├── test_Modifier.cpp
│   ├── test_Producer.cpp
│   ├── test_GraphManager.cpp
│   ├── test_HistoryNode.cpp
│   └── test_GameplayTag.cpp
├── build/                # Build output (gitignored)
├── .vscode/              # VS Code configuration
│   ├── settings.json     # Editor and C++ settings
│   ├── tasks.json        # Build tasks
│   ├── launch.json       # Debug configurations
│   └── extensions.json   # Recommended extensions
├── .clang-format         # Code formatting rules
├── .editorconfig         # Editor configuration
├── CMakeLists.txt        # Main build configuration
├── index.html            # D3.js visualization
└── README.md            # This file
```

## 🧬 Core Components

### StatNode
The fundamental building block of the graph system. Implements lazy evaluation with dirty flag optimization.

**Features:**
- Base nodes (source values)
- Derived nodes (calculated from dependencies)
- History nodes (temporal data with rolling windows)
- Conditional dependencies
- Custom calculation functions

### Modifier System
Path of Exile-inspired modifier architecture with multiple application types.

**Modifier Types:**
- **Flat**: Adds/subtracts fixed value (applied first)
- **Increased**: Additive percentage (stacks additively)
- **More**: Multiplicative (compounds separately)
- **Override**: Replaces value completely

### GraphManager
Central orchestrator managing node lifecycle, dependencies, and state.

**Responsibilities:**
- Node creation and retrieval
- Dependency resolution
- Tag management
- Modifier application
- Serialization for visualization

### Producer Pattern
Abstraction for game objects that generate modifiers (items, auras, buffs).

**Examples:**
- Equipment (weapons, armor)
- Active auras
- Temporary buffs
- Combat events

## 📊 The Stat Graph

| Command              | Description            | Example        |
| -------------------- | ---------------------- | -------------- |
| `set <stat> <value>` | Set a base stat        | `set str 50`   |
| `get <stat>`         | Get current value      | `get totalAtk` |
| `auto [on\|off]`     | Toggle auto-modulation | `auto on`      |
| `speed <value>`      | Set modulation speed   | `speed 2.0`    |
| `stats`              | Show all stat values   | `stats`        |
| `dirty`              | Show dirty flags       | `dirty`        |
| `recalc`             | Force recalculation    | `recalc`       |
| `help`               | Show help              | `help`         |
| `quit`               | Exit program           | `quit`         |

## 🌊 Auto-Modulation Mode

Type `auto on` to see the graph "breathe"!

- STR, DEX, and VIT oscillate on sine waves
- Watch dirty flags flicker through the dependency chain
- Perfect for demonstrating the optimization visually

## 🏗️ Architecture

### C++ Backend

```
include/
├── StatNode.h       # Core DAG node with dirty flag
├── GraphManager.h   # Node registry and JSON serialization
└── WebSocketServer.h # Self-contained WS server

src/
└── main.cpp         # Application entry, graph setup, main loop
```

### Frontend

```
index.html           # D3.js force-directed graph visualizer
```

### Key Design Patterns

1. **Pull Pattern**: `GetValue()` lazily evaluates only when dirty
2. **Push Invalidation**: `SetBaseValue()` propagates dirty flags to children
3. **Weak References**: Children hold weak_ptr to parents (prevents cycles)
4. **Observer Pattern**: WebSocket broadcasts state changes

## 📡 WebSocket Protocol

The server broadcasts JSON on each update:

```json
{
  "nodes": [
    { "id": "str", "label": "STR", "value": 50.0, "isDirty": false, "type": "base" },
    { "id": "totalAtk", "label": "Total ATK", "value": 200.0, "isDirty": true, "type": "derived" }
  ],
  "edges": [
    { "from": "str", "to": "totalAtk" },
    { "from": "weaponAtk", "to": "totalAtk" }
  ],
  "timestamp": 1706745600000
}
```

## 🎨 Visualization Legend

| Color           | Meaning                             |
| --------------- | ----------------------------------- |
| 🔘 Gray          | Base Stat (source, no dependencies) |
| 🟢 Green         | Clean (cached value is valid)       |
| 🔴 Red (pulsing) | Dirty (needs recalculation)         |

## 🔧 Extending the System

### Add a New Stat

```cpp
// In BuildCharacterGraph():

// 1. Create base stat
auto Luck = graph.CreateBaseStat("luck", "LUCK", 10);

// 2. Create derived stat depending on it
auto DropRate = graph.CreateDerivedStat(
    "dropRate", "Drop Rate %",
    {Luck, DEX},
    [](const std::vector<double>& parents) {
        return parents[0] * 0.5 + parents[1] * 0.1;
    }
);
```

### Custom Calculation

```cpp
auto ComplexStat = graph.CreateDerivedStat(
    "complex", "Complex Calc",
    {A, B, C},
    [](const std::vector<double>& p) {
        // p[0] = A, p[1] = B, p[2] = C
        return std::pow(p[0], 1.5) + std::log(p[1] + 1) * p[2];
    }
);
```

## 📚 Academic References & Learning Resources

### Books
- **Game Programming Patterns** by Robert Nystrom
  - Chapter: Dirty Flag Pattern
- **Game Engine Architecture** by Jason Gregory
  - Sections on attribute systems and dependencies
- **Introduction to Algorithms** (CLRS)
  - Graph algorithms and DAG properties

### Papers & Articles
- [Dirty Flag Pattern](http://gameprogrammingpatterns.com/dirty-flag.html)
- [Path of Exile Attribute System GDC Talk](https://www.gdcvault.com/)
- [Entity Component System Architecture](https://en.wikipedia.org/wiki/Entity_component_system)

### Online Resources
- [D3.js Force-Directed Graphs](https://d3js.org/d3-force)
- [WebSocket Protocol](https://developer.mozilla.org/en-US/docs/Web/API/WebSocket)
- [C++20 Features](https://en.cppreference.com/w/cpp/20)

## 🤝 Contributing

This is an academic study project. Contributions, suggestions, and feedback are welcome!

### How to Contribute
1. Fork the repository
2. Create a feature branch (`git checkout -b feature/amazing-feature`)
3. Ensure all tests pass (`ctest --test-dir build`)
4. Run code formatter (`cmake --build build --target format`)
5. Commit your changes (`git commit -m 'Add amazing feature'`)
6. Push to the branch (`git push origin feature/amazing-feature`)
7. Open a Pull Request

### Code Style
- Follow the `.clang-format` configuration
- Use meaningful variable and function names
- Add unit tests for new features
- Document complex algorithms
- Keep functions focused and small

## 🎯 Future Research Directions

### Planned Enhancements
- [ ] Cycle detection in graph construction
- [ ] Multithreaded calculation for independent subgraphs
- [ ] Serialization and persistence
- [ ] More complex temporal mechanics
- [ ] Performance profiling and benchmarking suite
- [ ] Comparison with ECS architecture
- [ ] GPU-accelerated calculation for massive graphs
- [ ] Network synchronization for multiplayer

### Open Questions
1. What is the optimal cache strategy for very large graphs (1000+ nodes)?
2. How to balance memory vs computation for history nodes?
3. Can we predict which nodes will be dirty based on gameplay patterns?
4. What is the impact of conditional dependencies on cache hit rates?

## 📊 Project Metrics

### Development Status
- **Phase**: Active Development
- **Version**: 1.0.0-alpha
- **Test Coverage Target**: 100%
- **Code Quality**: Continuous improvement

### Repository Stats
- **Language**: C++20
- **Build System**: CMake
- **Testing**: Google Test
- **Documentation**: Markdown + Doxygen (planned)

## 📄 License

MIT License - Free for academic and commercial use

Copyright (c) 2026 [Your Name]

See LICENSE file for details.

## 🙏 Acknowledgments

- Robert Nystrom for Game Programming Patterns
- Grinding Gear Games (Path of Exile) for inspiration
- D3.js community for visualization tools
- Google Test team for testing framework

---

## 📧 Contact & Discussion

For questions, suggestions, or academic collaboration:

- **Issues**: Use GitHub Issues for bugs and feature requests
- **Discussions**: GitHub Discussions for questions and ideas
- **Email**: [Your Email]

---

*This project is developed as part of ongoing research in game engine architecture and performance optimization. All feedback and collaboration opportunities are welcome!*
