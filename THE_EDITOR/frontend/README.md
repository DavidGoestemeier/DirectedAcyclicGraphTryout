# DAG Editor Frontend

A visual DAG (Directed Acyclic Graph) editor built with React, TypeScript, and React Flow for creating and managing complex attribute systems in RPG games.

## 🎯 Features Implemented (Tier 1 MVP)

### Graph Editing
✅ Drag nodes from palette onto canvas
✅ Connect nodes by dragging edges  
✅ Delete nodes/edges (Delete key)
✅ Pan canvas, Zoom canvas
✅ Select nodes, Multi-select support

### Node Types (5 Types)
✅ StatNode, DerivedNode, ConditionalNode, CapNode, ModifierNode

### Properties & Validation
✅ Properties panel with real-time editing
✅ Cycle detection (DFS algorithm)
✅ Type checking support

### Persistence & History
✅ Auto-save to localStorage (30s)
✅ Export/Import JSON files
✅ Undo/Redo (Ctrl+Z/Y) with 100 action history

### Backend Integration
✅ WebSocket client with auto-reconnect
✅ Debounced graph updates (300ms)
✅ Connection status indicator

## 🚀 Quick Start

```bash
cd THE_EDITOR/frontend
npm install
npm run dev    # Development server at http://localhost:5173
npm run build  # Production build
```

## 📚 Documentation

See [SPEC_KIT_COMPENDIUM.md](../SPEC_KIT_COMPENDIUM.md) for full specification and roadmap.
