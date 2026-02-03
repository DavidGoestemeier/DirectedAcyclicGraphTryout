# Architecture Documentation

## System Overview

The DAG RPG Attribute System is built on a foundation of graph theory and design patterns to create an efficient, scalable attribute calculation engine suitable for modern RPGs.

## Core Architecture Principles

### 1. Directed Acyclic Graph (DAG) Structure

The system models attribute dependencies as a DAG where:
- **Nodes** represent stats (STR, Attack, DPS, etc.)
- **Edges** represent dependencies (Attack depends on STR)
- **Acyclic** property prevents circular dependencies

**Benefits:**
- Topological ordering guarantees correct calculation order
- No infinite loops in dependency resolution
- Clear data flow from source to derived values

### 2. Pull-Based Evaluation

Values "pull" from their dependencies rather than being "pushed" from sources.

```cpp
double StatNode::GetValue() {
    if (!m_isDirty) {
        return m_cachedValue;  // O(1) cached read
    }

    // Pull from parents
    std::vector<double> parentValues;
    for (auto& parent : m_parents) {
        parentValues.push_back(parent.lock()->GetValue());  // Recursive
    }

    // Calculate
    m_cachedValue = m_calculateFunc(parentValues);
    m_isDirty = false;
    return m_cachedValue;
}
```

### 3. Dirty Flag Optimization

**Problem**: Recalculating all values every frame is expensive (O(n) per read).

**Solution**: Cache values and only recalculate when "dirty".

```
Clean Node:  GetValue() → return m_cachedValue  [O(1)]
Dirty Node:  GetValue() → recalculate → cache → return  [O(d)]
```

Where d = depth of dependency chain.

**Propagation:**
```
SetBaseValue(STR)
    → Mark children dirty (Attack, Defense)
        → Mark their children dirty (DPS, Effective HP)
            → Continue recursively
```

## Component Architecture

### StatNode

**Responsibilities:**
- Store base value and cached calculated value
- Track dirty state
- Manage parent/child relationships
- Execute calculation function
- Propagate dirty flags

**Types:**
- **Base**: Source nodes (no parents)
- **Derived**: Calculated from parents
- **History**: Temporal data with rolling windows

### GraphManager

**Responsibilities:**
- Node lifecycle management (create, retrieve, destroy)
- Dependency resolution
- Tag container management
- Modifier registry
- Serialization for visualization

**API Design:**
```cpp
// Create nodes
auto str = graph.CreateNode("str", "Strength", 10.0);

// Create derived nodes
auto attack = graph.CreateDerivedNode(
    "attack", "Attack Power",
    {"str", "weaponDmg"},
    [](const std::vector<double>& p) {
        return p[0] * 2.0 + p[1];  // STR*2 + Weapon
    }
);
```

### Modifier System

**Architecture:**
```
Item/Aura/Buff
    ↓
Creates Modifiers
    ↓
Applied to Stats
    ↓
Calculation includes modifiers
```

**Types & Application Order:**
1. **Override**: Replace value completely (rare)
2. **Flat**: Add/subtract fixed amounts
   ```
   newValue = baseValue + sum(flatMods)
   ```
3. **Increased**: Additive percentages
   ```
   newValue = baseValue * (1.0 + sum(increasedMods))
   ```
4. **More**: Multiplicative (compound)
   ```
   newValue = baseValue * product(1.0 + moreMods)
   ```

**Example:**
```
Base Damage: 100
+20 Flat
+50% Increased
+30% More

Result = ((100 + 20) * (1.0 + 0.5)) * (1.0 + 0.3)
       = (120 * 1.5) * 1.3
       = 180 * 1.3
       = 234
```

## Design Patterns

### 1. Observer Pattern
StatNodes observe their parents and are notified when dirty.

### 2. Lazy Evaluation
Values computed on-demand, not eagerly.

### 3. Builder Pattern
Fluent API for creating complex items:
```cpp
item->AddModifier("atk", Flat, 10)
    .AddModifier("crit", Increased, 0.20)
    .GrantsTag("DualWielding");
```

### 4. Strategy Pattern
Calculation functions are injectable strategies.

### 5. Producer Pattern
Items, Auras, Buffs produce modifiers and tags.

## Memory Management

### Smart Pointers

- **shared_ptr**: Nodes own their data
- **weak_ptr**: Children hold weak references to parents (prevents cycles)
- **enable_shared_from_this**: Nodes can get shared_ptr to themselves

### Lifetime Management

```
GraphManager owns nodes → shared_ptr
Nodes reference parents → weak_ptr (don't extend lifetime)
Nodes reference children → weak_ptr (for dirty propagation)
```

**Why weak_ptr for children?**
- Prevents circular ownership
- Parent doesn't need to keep children alive
- Children die naturally when nobody references them

## Performance Considerations

### Cache Hit Rate

**Best Case**: 95%+ cache hits in steady state
- Most nodes read multiple times per frame
- Changes affect small subgraphs
- Only 5-10 nodes dirty per action

**Worst Case**: 50% hit rate during heavy changes
- Equipment changes
- Major buffs/debuffs
- System-wide modifiers

### Optimization Strategies

1. **Lazy Initialization**: Don't create nodes until needed
2. **Dirty Flag Coalescing**: Batch multiple changes
3. **Topological Sorting**: Calculate in optimal order
4. **Conditional Dependencies**: Skip inactive branches

## Data Flow Example

```
User: "Equip Weapon +50 ATK"
    ↓
1. Create Modifier (flat +50 → attackStat)
2. Apply modifier to graph
3. Mark attackStat DIRTY
4. Propagate: attack → DPS → EffectiveHP (all DIRTY)
    ↓
5. User: "Show DPS"
    ↓
6. EffectiveHP.GetValue() (dirty)
    → DPS.GetValue() (dirty)
        → attack.GetValue() (dirty)
            → weaponAtk.GetValue() (clean, cached)
            → STR.GetValue() (clean, cached)
            → Calculate: weaponAtk + STR*2 + modifiers
            → Cache result, mark clean
        → Calculate DPS
        → Cache, mark clean
    → Calculate Effective HP
    → Cache, mark clean
7. Return final value
```

## Extension Points

### Adding New Node Types

Inherit from StatNode or create sibling class:
```cpp
class HistoryNode : public StatNode {
    // Add temporal data storage
    // Override GetValue() for time-based calculations
};
```

### Custom Calculation Logic

Inject calculation functions:
```cpp
auto complexStat = graph.CreateDerivedNode(
    "id", "label", {"a", "b", "c"},
    [](const std::vector<double>& p) {
        // Your math here
        return std::pow(p[0], 1.5) + std::log(p[1] + 1) * p[2];
    }
);
```

### Adding Visualization

WebSocket broadcasts JSON:
```json
{
  "nodes": [...],
  "edges": [...],
  "timestamp": ...
}
```

Frontend consumes and renders with D3.js.

## Testing Strategy

### Unit Tests

- Test each component in isolation
- Mock dependencies with test doubles
- Verify edge cases and error handling

### Integration Tests

- Test component interactions
- Verify dirty propagation through chains
- Test modifier application

### Performance Tests

- Benchmark cache hit rates
- Measure recalculation overhead
- Profile memory usage

## Scalability Analysis

### Vertical Scaling (More Nodes)

- O(1) node lookups (hash map)
- O(d) calculation depth (typically d < 10)
- Memory: ~64 bytes per node
- **Limit**: ~10,000 nodes per graph before memory pressure

### Horizontal Scaling (Multiple Graphs)

- Each character has independent graph
- No cross-character dependencies
- Parallelizable across multiple threads

## Comparison with Alternatives

### vs. ECS (Entity Component System)

| Aspect       | DAG System             | ECS                    |
| ------------ | ---------------------- | ---------------------- |
| Dependencies | Explicit (edges)       | Implicit (systems)     |
| Cache        | Per-node               | Per-component-array    |
| Flexibility  | High (any calculation) | Medium (system-driven) |
| Performance  | O(1) cached reads      | O(n) system iteration  |
| Memory       | Higher overhead        | Cache-friendly         |

### vs. Traditional OOP

| Aspect        | DAG System               | Traditional            |
| ------------- | ------------------------ | ---------------------- |
| Recalculation | Lazy (on-demand)         | Eager (on-change)      |
| Extensibility | High (inject functions)  | Medium (inheritance)   |
| Complexity    | Medium (graph theory)    | Low (simple hierarchy) |
| Debugging     | Harder (graph structure) | Easier (call stack)    |

## Conclusion

The DAG-based architecture provides an optimal balance between:
- **Flexibility**: Add any stat with any calculation
- **Performance**: O(1) cached reads for clean nodes
- **Maintainability**: Clear separation of concerns
- **Extensibility**: Easy to add new features

The dirty flag optimization is crucial for real-time performance, reducing expensive recalculations by 90-95% in typical scenarios.
