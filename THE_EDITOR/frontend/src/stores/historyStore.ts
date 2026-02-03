// Undo/Redo state management using Command pattern

import { create } from 'zustand';
import type { Node, Edge } from 'reactflow';
import type { NodeData } from '../types/node';

export interface Command {
  execute: () => void;
  undo: () => void;
  description: string;
}

interface HistoryStore {
  undoStack: Command[];
  redoStack: Command[];
  maxHistorySize: number;
  
  executeCommand: (command: Command) => void;
  undo: () => void;
  redo: () => void;
  canUndo: () => boolean;
  canRedo: () => boolean;
  clearHistory: () => void;
}

export const useHistoryStore = create<HistoryStore>((set, get) => ({
  undoStack: [],
  redoStack: [],
  maxHistorySize: 100,
  
  executeCommand: (command) => {
    command.execute();
    const { undoStack, maxHistorySize } = get();
    const newStack = [...undoStack, command];
    
    // Limit history size
    if (newStack.length > maxHistorySize) {
      newStack.shift();
    }
    
    set({ undoStack: newStack, redoStack: [] });
  },
  
  undo: () => {
    const { undoStack, redoStack } = get();
    if (undoStack.length === 0) return;
    
    const command = undoStack[undoStack.length - 1];
    command.undo();
    
    set({
      undoStack: undoStack.slice(0, -1),
      redoStack: [...redoStack, command],
    });
  },
  
  redo: () => {
    const { redoStack, undoStack } = get();
    if (redoStack.length === 0) return;
    
    const command = redoStack[redoStack.length - 1];
    command.execute();
    
    set({
      redoStack: redoStack.slice(0, -1),
      undoStack: [...undoStack, command],
    });
  },
  
  canUndo: () => get().undoStack.length > 0,
  canRedo: () => get().redoStack.length > 0,
  
  clearHistory: () => set({ undoStack: [], redoStack: [] }),
}));

// Helper function to create commands
export function createNodeCommand(
  description: string,
  nodes: Node<NodeData>[],
  edges: Edge[],
  setNodes: (nodes: Node<NodeData>[]) => void,
  setEdges: (edges: Edge[]) => void,
  previousNodes: Node<NodeData>[],
  previousEdges: Edge[]
): Command {
  return {
    execute: () => {
      setNodes(nodes);
      setEdges(edges);
    },
    undo: () => {
      setNodes(previousNodes);
      setEdges(previousEdges);
    },
    description,
  };
}
