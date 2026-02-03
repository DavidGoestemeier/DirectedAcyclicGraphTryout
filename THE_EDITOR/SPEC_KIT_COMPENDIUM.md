# DAG Editor V3 - Spec Kit Information Compendium

## 📌 Scope & Rules

- **ALL CODE MUST BE BUILT IN** `THE_EDITOR/`
- Existing C++ project is **reference only** (no edits outside THE_EDITOR)

## 🎯 Project Vision

**ONE SENTENCE PITCH:**
A visual DAG editor where game designers can create complex attribute systems by dragging nodes, connecting them, and immediately seeing results—no code required.

---

## 📋 Constitution (Project Principles)

### Core Principles
1. **Plug & Play** - Items/effects are self-contained with tags and modifiers
2. **Visual First** - Everything editable through UI, zero code needed
3. **Real-time Feedback** - See calculated values instantly
4. **Undo Everything** - All actions reversible
5. **Portable** - Import/export graphs as JSON
6. **Type Safe** - Prevent invalid connections at design time
7. **Performance** - Handle 1000+ node graphs smoothly

---

## 🏗️ Technology Stack (Recommended)

### Frontend
- **Framework:** React 18 + TypeScript
- **Node Editor:** React Flow 11
- **State:** Zustand (lightweight, simple)
- **UI Library:** shadcn/ui
- **Build Tool:** Vite
- **Target:** **Web app first** (desktop wrapper later, optional)

### Backend
- **Core:** Existing C++ DAG system
- **Communication:** WebSocket (already implemented)
- **Serialization:** nlohmann/json

### Why This Stack?
- Fast iteration with React + Vite
- React Flow handles 90% of node editor complexity
- TypeScript prevents runtime errors
- Zustand is simpler than Redux
- Web-first keeps iteration fastest

---

## ✅ Critical Features Checklist

### 🔴 Tier 1 - MVP (Must Have First)

#### Graph Editing
- [ ] Drag nodes from palette onto canvas
- [ ] Connect nodes by dragging edges
- [ ] Delete nodes/edges (Delete key)
- [ ] Pan canvas (middle-click drag)
- [ ] Zoom canvas (mouse wheel)
- [ ] Select nodes (click)
- [ ] Multi-select (Ctrl+click, box select)

#### Node Types (All in first build)
- [ ] StatNode (base value)
- [ ] DerivedNode (calculated from parents)
- [ ] ConditionalNode (if/then logic)
- [ ] CapNode (min/max limits)
- [ ] ModifierNode (flat/increased/more)
- [ ] PerXNode (per charge/stack)
- [ ] ThresholdNode (breakpoints)
- [ ] RangeNode (min-max random)
- [ ] AccumulatorNode (sum multiple sources)
- [ ] ConversionNode (damage type conversion)
- [ ] ChanceNode (probability/proc)
- [ ] ToggleNode (on/off state)

#### Properties
- [ ] Properties panel (right sidebar)
- [ ] Edit node name inline
- [ ] Edit node values in panel
- [ ] See output value in real-time

#### Validation
- [ ] Prevent cycles when connecting
- [ ] Highlight invalid connection targets in red
- [ ] Type checking (number to number only initially)
- [ ] Show validation errors in UI

#### Persistence
- [ ] Save graph to JSON file
- [ ] Load graph from JSON file
- [ ] Auto-save (every 30s, localStorage)
- [ ] Export graph (download .json)
- [ ] Import graph (upload .json)

#### Undo/Redo
- [ ] Undo last action (Ctrl+Z)
- [ ] Redo last action (Ctrl+Y)
- [ ] Works for: add, delete, move, edit, connect
- [ ] Store last 100 actions
- [ ] Clear on save (optional)

#### Backend Integration (Immediate)
- [ ] Connect to C++ backend via WebSocket
- [ ] Send graph on change (debounced 300ms)
- [ ] Receive calculated values
- [ ] Display values on nodes
- [ ] Show connection status indicator

---

### 🟡 Tier 2 - Essential (Needed Soon)

#### Editing Features
- [ ] Copy/paste nodes (Ctrl+C/V)
- [ ] Duplicate nodes (Ctrl+D)
- [ ] Align nodes (horizontal/vertical)
- [ ] Snap to grid (optional toggle)
- [ ] Node search/filter (Ctrl+F)

#### Node Organization
- [ ] Node groups/frames (visual grouping)
- [ ] Collapse/expand groups
- [ ] Node colors by type
- [ ] Node icons
- [ ] Sticky notes/comments

#### Navigation
- [ ] Minimap (bottom-right corner)
- [ ] Fit to screen (Ctrl+0)
- [ ] Zoom to selection
- [ ] Breadcrumb navigation

#### Templates
- [ ] Save node subgraph as template
- [ ] Template library panel
- [ ] Drag template onto canvas
- [ ] Built-in templates (common patterns)

#### Advanced Nodes
- [ ] Included in Tier 1 (see above)

#### Item System Integration
- [ ] Create Item (gives tags + modifiers)
- [ ] Item panel (list all items)
- [ ] Equip/unequip items
- [ ] See modifier list on nodes
- [ ] Item templates

---

### 🟢 Tier 3 - Polish (Nice to Have)

#### UX Polish
- [ ] Keyboard shortcuts help (press ?)
- [ ] Tooltips on hover
- [ ] Welcome tutorial
- [ ] Example graphs to load
- [ ] Dark/light theme

#### Performance
- [ ] Virtualized rendering (only visible nodes)
- [ ] Level-of-detail (simplified view when zoomed out)
- [ ] Lazy loading for large graphs
- [ ] Performance metrics overlay

#### Advanced Features
- [ ] Auto-layout algorithms (dagre, elk)
- [ ] Expression editor (for calculations)
- [ ] Debugging mode (step through calculation)
- [ ] Profiling view (show calculation time per node)
- [ ] Version history (git-like)

#### Export/Import
- [ ] Export to PNG/SVG (visualization)
- [ ] Export to code (C++ header)
- [ ] Import from other formats (GraphML, etc.)
- [ ] Batch operations on graphs

#### Collaboration (Future)
- [ ] Share graph via URL
- [ ] Collaborative editing (multiplayer)
- [ ] Comments/annotations
- [ ] Change tracking

---

## 🎨 Design System

### Node Visual Design
```
┌─────────────────────┐
│ 🎯 StatNode         │  ← Icon + Type Name
├─────────────────────┤
│ Strength            │  ← Display Name
│ Base: 10            │  ← Editable Value
│ Final: 25           │  ← Calculated (if derived)
├─────────────────────┤
│ ◀ In   Out ▶       │  ← Connection Handles
└─────────────────────┘
```

### Color Coding
- **Blue:** Base nodes (StatNode)
- **Green:** Derived nodes (calculations)
- **Yellow:** Conditional logic
- **Purple:** Modifiers
- **Red:** Invalid/error state
- **Gray:** Disabled/inactive

### Theme
- **Dark theme only (Blender-style)**

### Connection Rules
- **Solid line:** Active connection
- **Dashed line:** Conditional/inactive
- **Red line:** Invalid (during drag)
- **Animated:** Currently calculating

---

## 🔌 Plug & Play Item System

### Design Goal
Create items that are **self-contained** and **composable**.

### Item Definition (JSON)
```json
{
  "id": "legendary_sword_001",
  "name": "Legendary Sword of Fury",
  "type": "weapon",
  "tags": ["Weapon", "Sword", "TwoHanded", "Legendary"],
  "modifiers": [
    {
      "target": "physicalDamage",
      "type": "Flat",
      "value": 50
    },
    {
      "target": "attackSpeed",
      "type": "Increased",
      "value": 0.20
    },
    {
      "target": "criticalChance",
      "type": "Flat",
      "value": 0.05
    }
  ],
  "conditionalModifiers": [
    {
      "condition": "hasTag('LowLife')",
      "target": "damage",
      "type": "More",
      "value": 0.50
    }
  ]
}
```

### Usage in Editor
1. Drag "Item Slot" node onto canvas
2. Assign item from dropdown
3. Item automatically:
   - Applies all modifiers to connected stats
   - Grants tags to character
   - Shows preview of effects
4. Equip/unequip with single click
5. See before/after values immediately

---

## 📦 Import/Export System

### Export Formats

#### 1. Full Graph (.dagraph.json)
- Complete graph with positions, types, connections
- Include metadata (author, version, created date)
- Human-readable JSON
- Compressed option for large graphs

#### 2. Graph Template (.template.json)
- Subgraph (selected nodes only)
- Reusable patterns
- Parameterized values

#### 3. Item Definition (.item.json)
- Single item with modifiers
- Shareable across projects

#### 4. Visualization (.svg/.png)
- Static image of graph
- For documentation/sharing

### Import Sources
- Local file (.json)
- URL (fetch from web)
- Clipboard (paste JSON)
- Merge into existing graph (don't replace)

---

## 🎬 Initial Prompts for Spec Kit

### Prompt 1: /specify

```
Build a visual DAG node editor for an RPG attribute system using React + TypeScript + React Flow.

WHAT TO BUILD:
- Canvas where users drag nodes from a palette and connect them with edges
- 5 initial node types: StatNode, DerivedNode, ConditionalNode, CapNode, ModifierNode
- Properties panel to edit node values
- Real-time calculation via existing C++ backend over WebSocket
- Validation to prevent cycles
- Full undo/redo (100 action history)
- Save/load graphs as JSON
- Import/export system

PLUG & PLAY DESIGN:
- Items are self-contained JSON with tags and modifiers
- Drag item onto graph → automatically applies modifiers
- No code required to create items or stats
- Visual feedback for all changes

MUST HAVES:
- TypeScript for type safety
- Zustand for state management
- React Flow 11 for node editor
- Vite for build tool
- shadcn/ui for UI components
- WebSocket connection to existing C++ backend
- Save to localStorage (auto-save every 30s)
- Export/import as .json files
- Undo/redo with Command pattern

REFERENCE:
- See THE_EDITOR/notes.md for full architecture
- Existing C++ backend already handles calculations
- WebSocket server already implemented
```

### Prompt 2: /clarify (Expected Questions)

**Q:** What should happen when user drags invalid connection?
**A:** Show red highlight on invalid target nodes, prevent drop, show tooltip with reason

**Q:** How to handle large graphs (1000+ nodes)?
**A:** Use React Flow's virtualization, only render visible nodes, start with 100-node limit for MVP

**Q:** Where to store graph data?
**A:** Primary: localStorage (auto-save), Secondary: JSON file export/import

**Q:** How to sync with C++ backend?
**A:** Debounce changes (300ms), send full graph as JSON via WebSocket, receive calculated values back

**Q:** What happens if backend is offline?
**A:** Show "Disconnected" indicator, allow editing, queue calculations, retry on reconnect

**Q:** How detailed should undo/redo be?
**A:** Every distinct user action (add node, delete edge, change value, move node = separate undo)

**Q:** Can users create custom node types in the editor?
**A:** No in MVP. Later: visual node type designer with JSON schema output

### Prompt 3: /plan

**Phase 1: Foundation (Week 1)**
- Set up Vite + React + TypeScript project in THE_EDITOR/frontend/
- Install dependencies: React Flow, Zustand, shadcn/ui
- Create basic canvas with pan/zoom
- Implement node palette with 1 draggable node type (StatNode)
- Basic drop-on-canvas functionality

**Phase 2: Core Editing (Week 2)**
- Add all 5 node types with visual styling
- Implement connection drawing with validation
- Prevent cycles with DFS check
- Properties panel (right sidebar) for editing
- Multi-select and delete

**Phase 3: Persistence (Week 3)**
- Undo/redo with Command pattern
- Save/load to localStorage
- Export to JSON file (download)
- Import from JSON file (upload)
- Auto-save every 30s

**Phase 4: Backend Integration (Week 4)**
- WebSocket client connection
- Send graph on change (debounced)
- Receive and display calculated values
- Connection status indicator
- Error handling

**Phase 5: Polish (Week 5)**
- Keyboard shortcuts (Ctrl+Z, Ctrl+C/V, Delete)
- Minimap
- Fit to screen
- Node search
- Welcome tutorial

---

## 🧪 Success Criteria

### Technical
- [ ] Load 500-node graph in <2 seconds
- [ ] 60fps interaction (no jank when dragging)
- [ ] Save/load roundtrip preserves graph exactly
- [ ] Undo/redo works 100% reliably
- [ ] Backend sync latency <500ms

### User Experience
- [ ] Create first node in <30 seconds (no tutorial)
- [ ] Build simple graph (5 nodes) in <2 minutes
- [ ] Invalid connection attempt shows clear error
- [ ] All critical features keyboard accessible
- [ ] Zero data loss (auto-save works)

### Plug & Play
- [ ] Add item to graph in 1 click
- [ ] See modifier effects immediately
- [ ] Export item for reuse in <10 seconds
- [ ] Import template in <5 seconds

---

## 🚀 Quick Start Commands

```bash
# Initialize project
cd THE_EDITOR
npm create vite@latest frontend -- --template react-ts
cd frontend
npm install

# Install dependencies
npm install reactflow zustand
npm install -D @types/node

# Install shadcn/ui
npx shadcn-ui@latest init

# Start dev server
npm run dev

# Build for production
npm run build
```

---

## 📁 Project Structure

```
THE_EDITOR/
├── frontend/
│   ├── src/
│   │   ├── App.tsx                    # Main app component
│   │   ├── components/
│   │   │   ├── Canvas.tsx             # Main editor canvas
│   │   │   ├── NodePalette.tsx        # Left sidebar - node types
│   │   │   ├── PropertiesPanel.tsx    # Right sidebar - edit props
│   │   │   ├── Toolbar.tsx            # Top toolbar
│   │   │   ├── Minimap.tsx            # Bottom-right minimap
│   │   │   └── nodes/
│   │   │       ├── StatNode.tsx
│   │   │       ├── DerivedNode.tsx
│   │   │       ├── ConditionalNode.tsx
│   │   │       ├── CapNode.tsx
│   │   │       └── ModifierNode.tsx
│   │   ├── stores/
│   │   │   ├── graphStore.ts          # Main graph state (Zustand)
│   │   │   ├── historyStore.ts        # Undo/redo state
│   │   │   └── uiStore.ts             # UI state (selected, etc.)
│   │   ├── services/
│   │   │   ├── websocket.ts           # Backend connection
│   │   │   ├── persistence.ts         # Save/load
│   │   │   └── validation.ts          # Cycle detection, etc.
│   │   ├── types/
│   │   │   ├── graph.ts               # Graph types
│   │   │   └── node.ts                # Node types
│   │   └── utils/
│   │       ├── commands.ts            # Command pattern for undo
│   │       └── layout.ts              # Auto-layout helpers
│   ├── package.json
│   └── vite.config.ts
├── schemas/                            # JSON schemas
│   ├── graph.schema.json
│   ├── item.schema.json
│   └── node.schema.json
├── examples/                           # Example graphs
│   ├── basic-combat.json
│   ├── dual-wield.json
│   └── conditional-damage.json
├── notes.md                            # This file
└── SPEC_KIT_COMPENDIUM.md             # You are here
```

---

## 🎯 Key Design Decisions

### 1. Why React Flow?
- Handles 90% of node editor complexity
- Built-in pan/zoom, connection drawing
- Customizable node components
- Good performance (<5000 nodes)
- Active community

### 2. Why Zustand over Redux?
- Simpler API (less boilerplate)
- Better TypeScript support
- Easier to test
- Smaller bundle size
- Sufficient for this use case

### 3. Why TypeScript?
- Catch errors at compile time
- Better IDE support
- Self-documenting code
- Easier refactoring
- Type-safe backend contract

### 4. Why Vite over CRA?
- 10x faster dev server
- Faster builds
- Better HMR
- Modern defaults
- Smaller bundle

### 5. Why WebSocket over REST?
- Real-time bidirectional
- Backend already uses it
- Efficient for frequent updates
- Can push server-side changes

---

## 🔥 Critical Path (Must Not Fail)

### Week 1 Milestone
- User can drag StatNode onto canvas
- User can see node on screen
- User can pan/zoom canvas

**If this fails:** React Flow setup is wrong, abort and debug

### Week 2 Milestone
- User can connect two nodes
- User can see connection line
- User cannot create cycle (error shown)

**If this fails:** Graph data structure is wrong, revisit architecture

### Week 3 Milestone
- User can save graph
- User can load saved graph
- Undo/redo works for add/delete

**If this fails:** Serialization or command pattern broken, critical to fix

### Week 4 Milestone
- Frontend connects to C++ backend
- Graph sent to backend on change
- Calculated values shown on nodes

**If this fails:** WebSocket contract mismatch, need protocol spec

---

## 📝 Next Immediate Actions

1. **Run spec kit /specify** with Prompt 1 above
2. **Answer /clarify** questions using answers provided
3. **Execute /plan** following Phase 1 tasks
4. **Create initial project structure** in THE_EDITOR/frontend/
5. **Install dependencies** and verify build works
6. **Implement first node type** (StatNode) with drag-drop
7. **Commit to git** after each working feature

---

## ✨ Vision for Final Product

**Imagine:**
A game designer opens the editor, drags a "Strength" node, connects it to "Attack" with a multiplier, drops a "Legendary Sword" item which adds +50 attack and +20% crit, sees the final damage number update in real-time, exports the graph, and hands it to a programmer who drops the .json file into the game and it just works.

**That's the goal.** 🎯
