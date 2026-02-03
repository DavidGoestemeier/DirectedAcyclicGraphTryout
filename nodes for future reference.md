# Node Types for ARPG Systems

Reference document for additional node types used in ARPGs like Path of Exile and Diablo.

## Additional Node Types for ARPGs

### 1. **ConditionalNode** (Most Important)
Activates based on conditions (tags, thresholds, states)

```cpp
class ConditionalNode : public StatNode {
    std::function<bool()> m_condition;
    std::string m_trueParent;
    std::string m_falseParent;

    // Examples:
    // - "If Low Life (<35% HP): +50% More Damage"
    // - "While Dual Wielding: +15% Attack Speed"
    // - "Per Frenzy Charge: +4% Attack Speed"
};
```

### 2. **ThresholdNode**
Steps/breakpoints (common in PoE)

```cpp
class ThresholdNode : public StatNode {
    std::vector<std::pair<double, double>> m_thresholds;

    // Examples:
    // - Armor: Returns % reduction based on damage (diminishing)
    // - Accuracy: Hit chance based on accuracy vs evasion
    // - Resistances: Capped at 75%
};
```

### 3. **RangeNode** / **MinMaxNode**
Random ranges (damage, loot)

```cpp
class RangeNode : public StatNode {
    double m_min, m_max;

    // Examples:
    // - "Damage: 50-75" (random per hit)
    // - "Critical Multiplier: 150%-200%"
    // - "Item Quantity: +20-30%"
};
```

### 4. **PerXNode** / **ScalingNode**
Per-charge, per-stack, per-nearby-enemy

```cpp
class PerXNode : public StatNode {
    std::string m_sourceCountStat;
    double m_perAmount;

    // Examples:
    // - "+4% Attack Speed per Frenzy Charge"
    // - "+2% Damage per Power Charge"
    // - "+5% Movement Speed per nearby Enemy"
};
```

### 5. **CapNode** / **ClampNode**
Hard limits (resistance caps, maximum values)

```cpp
class CapNode : public StatNode {
    double m_min = -DBL_MAX;
    double m_max = DBL_MAX;

    // Examples:
    // - "Resistances: Capped at 75%"
    // - "Movement Speed: Max 200%"
    // - "Critical Chance: 0-95%"
};
```

### 6. **AverageNode** / **WeightedAverageNode**
For dual wielding, multiple damage types

```cpp
class AverageNode : public StatNode {
    std::vector<double> m_weights;

    // Examples:
    // - "Dual Wield: Average weapon damage"
    // - "Combined Physical + Elemental DPS"
};
```

### 7. **ConversionNode**
Damage type conversion (PoE's complex conversions)

```cpp
class ConversionNode : public StatNode {
    std::map<std::string, double> m_conversionRatios;

    // Examples:
    // - "50% Physical → Fire"
    // - "100% Lightning → Chaos"
    // - "40% Cold → Lightning"
};
```

### 8. **AccumulatorNode** / **SumNode**
Total across all sources

```cpp
class AccumulatorNode : public StatNode {
    std::vector<std::string> m_sources;

    // Examples:
    // - "Total Elemental Damage" (Fire + Cold + Lightning)
    // - "All Resistances" (sum of all res types)
    // - "Total Increased Damage" (sum all modifiers)
};
```

### 9. **ChanceNode** / **ProbabilityNode**
Proc chances, critical strike

```cpp
class ChanceNode : public StatNode {
    bool RollChance() {
        return (rand() % 100) < m_cachedValue;
    }

    // Examples:
    // - "Critical Strike Chance: 35%"
    // - "Chance to Poison: 60%"
    // - "Block Chance: 25%"
};
```

### 10. **EffectiveNode**
Effective values (PoE's eHP, eDPS)

```cpp
class EffectiveNode : public StatNode {
    // Examples:
    // - "Effective HP" = Life / (1 - mitigation%)
    // - "Effective DPS" = DPS * hit_chance * (1 + crit_chance * crit_multi)
    // - "Effective Movement Speed" = base * (1 + increases) * slow_effects
};
```

### 11. **PenaltyNode** / **DiminishingNode**
Diminishing returns

```cpp
class DiminishingNode : public StatNode {
    double m_softCap;
    double m_diminishFactor;

    // Examples:
    // - "Armor: Diminishing returns above 30,000"
    // - "Evasion: 95% cap, harder to reach"
    // - "Energy Shield Recharge: Diminishing with investment"
};
```

### 12. **ToggleNode** / **BinaryNode**
On/off states (auras, stances)

```cpp
class ToggleNode : public StatNode {
    bool m_isActive = false;

    // Examples:
    // - "Aura Active: True/False"
    // - "Blood Rage: Enabled"
    // - "Shield Equipped: Yes/No"
};
```

## Path of Exile Specific Examples

### LuckyNode
Crit with lucky/unlucky mechanics

```cpp
class LuckyNode : public StatNode {
    // Rolls twice, takes best/worst
};
```

### PenetrationNode
Exposure/Penetration

```cpp
class PenetrationNode : public StatNode {
    // Reduces enemy resistance
};
```

### MoreMultiplierNode
More vs Increased (already in your Modifier system)

```cpp
class MoreMultiplierNode : public StatNode {
    // Multiplicative stacking
};
```

### ReservationNode
Reservation mechanics

```cpp
class ReservationNode : public StatNode {
    // Reduces max HP/mana pool
};
```

## Diablo Specific Examples

### PrimaryStatNode
Primary/Secondary stats

```cpp
class PrimaryStatNode : public StatNode {
    // STR, DEX, INT, VIT
};
```

### SnapshotNode
Snapshot mechanics

```cpp
class SnapshotNode : public StatNode {
    double m_snapshotValue;
    // Captures value at cast time
};
```

### AreaDamageNode
Area damage

```cpp
class AreaDamageNode : public StatNode {
    // Chance to proc AoE on hit
};
```

## Implementation Priority

1. **ConditionalNode** - Essential for most builds (conditional bonuses)
2. **CapNode** - Required for game balance (resistance caps, etc.)
3. **PerXNode** - Core scaling mechanic (charges, stacks)
4. **EffectiveNode** - Player understanding (effective HP, DPS)
5. **ConversionNode** - Build diversity (damage type conversions)
6. **ThresholdNode** - Complex mechanics (armor, accuracy)
7. **AccumulatorNode** - Combining multiple sources
8. **ChanceNode** - Proc mechanics
9. **RangeNode** - Random damage ranges
10. **DiminishingNode** - Balance mechanics
11. **AverageNode** - Dual wielding, hybrid damage
12. **ToggleNode** - Auras and active effects

## Integration with Current Architecture

All these node types extend the base `StatNode` class and maintain:
- DAG structure (acyclic dependencies)
- Dirty flag optimization
- Pull-based evaluation
- Smart pointer memory management

## Design Considerations

### When to Create New Node Types vs Using Calculation Functions

**Create New Node Type When:**
- Logic is reused across many instances
- Special data structures needed
- Custom dirty propagation required
- Type-specific optimizations possible

**Use Calculation Function When:**
- One-off or rare calculation
- Simple mathematical formula
- No special state management
- Easy to express as lambda

### Performance Impact

Most specialized nodes add minimal overhead:
- Same O(1) cached read performance
- Same dirty flag mechanism
- Additional memory: typically 8-16 bytes per instance
- Virtual function call overhead: negligible (<1ns)

## Examples in Real Games

### Path of Exile

```cpp
// Frenzy Charges
auto frenzyCharges = graph.CreateNode("frenzyCharges", "Frenzy Charges", 0.0);
auto atkSpeedPerFrenzy = graph.CreatePerXNode(
    "atkSpeedPerFrenzy",
    "Attack Speed per Frenzy",
    "frenzyCharges",
    4.0  // +4% per charge
);

// Conditional: Low Life
auto isLowLife = graph.CreateConditionalNode(
    "isLowLife",
    "Low Life State",
    [&]() { return (currentLife / maxLife) < 0.35; }
);

// Resistance Cap
auto fireRes = graph.CreateCapNode(
    "fireResistance",
    "Fire Resistance",
    {"baseFireRes", "increasedFireRes"},
    -60.0,  // min (negative res penalty)
    75.0    // max cap
);
```

### Diablo

```cpp
// Primary Stat Scaling
auto strength = graph.CreateNode("strength", "Strength", 100.0);
auto armorFromStr = graph.CreateDerivedNode(
    "armorFromStr",
    "Armor from Strength",
    {"strength"},
    [](const std::vector<double>& p) { return p[0] * 1.0; }  // 1 armor per STR
);

// Critical Hit Damage Range
auto critDamage = graph.CreateRangeNode(
    "critDamage",
    "Critical Hit Damage",
    150.0,  // min
    200.0   // max
);
```

## Future Extensions

### Temporal Nodes
- Damage over time (DoT)
- Buffs with duration
- Cooldowns and timers

### Stochastic Nodes
- Variance calculations
- Monte Carlo simulations
- Probability distributions

### Network Nodes
- Multi-player buffs
- Aura stacking
- Party-wide effects

### AI/ML Nodes
- Build optimization
- Gear recommendations
- Dynamic difficulty adjustment
