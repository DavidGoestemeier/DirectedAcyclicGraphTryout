# 🎮 DAG RPG Attribute System

A **Live Visualized** RPG Attribute System demonstrating the power of **Directed Acyclic Graphs (DAG)** with **Dirty Flag Optimization**.

![Architecture](https://img.shields.io/badge/Architecture-DAG-blue)
![C++](https://img.shields.io/badge/C++-20-green)
![WebSocket](https://img.shields.io/badge/WebSocket-Real--time-orange)
![D3.js](https://img.shields.io/badge/D3.js-v7-red)

## 🎯 Purpose

This proof-of-concept demonstrates how modern game engines optimize stat/attribute calculations:

- **Lazy Evaluation**: Values are only recalculated when actually needed
- **Dirty Flag Pattern**: Changed nodes mark dependents as "dirty"
- **Pull-Based Architecture**: Derived stats pull values from dependencies on demand
- **Real-time Visualization**: See the optimization in action via WebSocket + D3.js

## 📊 The Stat Graph

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

- CMake 3.16+
- C++20 compatible compiler (MSVC 2019+, GCC 10+, Clang 12+)
- A modern web browser

### Build

```bash
# Create build directory
mkdir build && cd build

# Configure
cmake ..

# Build
cmake --build . --config Release

# Run
./bin/dag_rpg_system      # Linux/macOS
.\bin\Release\dag_rpg_system.exe  # Windows
```

### Run

1. Start the C++ server:
   ```bash
   ./dag_rpg_system
   ```

2. Open `index.html` in your browser (or from the `bin` folder)

3. Watch the graph! Use commands in the console.

## 🎮 Console Commands

| Command | Description | Example |
|---------|-------------|---------|
| `set <stat> <value>` | Set a base stat | `set str 50` |
| `get <stat>` | Get current value | `get totalAtk` |
| `auto [on\|off]` | Toggle auto-modulation | `auto on` |
| `speed <value>` | Set modulation speed | `speed 2.0` |
| `stats` | Show all stat values | `stats` |
| `dirty` | Show dirty flags | `dirty` |
| `recalc` | Force recalculation | `recalc` |
| `help` | Show help | `help` |
| `quit` | Exit program | `quit` |

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

| Color | Meaning |
|-------|---------|
| 🔘 Gray | Base Stat (source, no dependencies) |
| 🟢 Green | Clean (cached value is valid) |
| 🔴 Red (pulsing) | Dirty (needs recalculation) |

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

## 📚 Learning Resources

- [Game Programming Patterns - Dirty Flag](http://gameprogrammingpatterns.com/dirty-flag.html)
- [DAG-based Expression Evaluation](https://en.wikipedia.org/wiki/Directed_acyclic_graph)
- [D3.js Force Layout](https://d3js.org/d3-force)

## 📄 License

MIT License - Use freely for learning and projects!

---

*Built with ❤️ to demonstrate game engine architecture patterns*
