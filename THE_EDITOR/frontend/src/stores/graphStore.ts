// Main graph state management using Zustand

import { create } from 'zustand';
import type { Node, Edge, NodeChange, EdgeChange } from 'reactflow';
import { applyNodeChanges, applyEdgeChanges } from 'reactflow';
import type { NodeData } from '../types/node';

interface GraphStore {
  nodes: Node<NodeData>[];
  edges: Edge[];
  selectedNodes: string[];
  
  setNodes: (nodes: Node<NodeData>[]) => void;
  setEdges: (edges: Edge[]) => void;
  
  onNodesChange: (changes: NodeChange[]) => void;
  onEdgesChange: (changes: EdgeChange[]) => void;
  
  addNode: (node: Node<NodeData>) => void;
  deleteNode: (nodeId: string) => void;
  updateNodeData: (nodeId: string, data: Partial<NodeData>) => void;
  
  addEdge: (edge: Edge) => void;
  deleteEdge: (edgeId: string) => void;
  
  setSelectedNodes: (nodeIds: string[]) => void;
  clearSelection: () => void;
  
  resetGraph: () => void;
}

export const useGraphStore = create<GraphStore>((set, get) => ({
  nodes: [],
  edges: [],
  selectedNodes: [],
  
  setNodes: (nodes) => set({ nodes }),
  setEdges: (edges) => set({ edges }),
  
  onNodesChange: (changes) => {
    set({
      nodes: applyNodeChanges(changes, get().nodes),
    });
  },
  
  onEdgesChange: (changes) => {
    set({
      edges: applyEdgeChanges(changes, get().edges),
    });
  },
  
  addNode: (node) => {
    set({ nodes: [...get().nodes, node] });
  },
  
  deleteNode: (nodeId) => {
    set({
      nodes: get().nodes.filter((n) => n.id !== nodeId),
      edges: get().edges.filter((e) => e.source !== nodeId && e.target !== nodeId),
    });
  },
  
  updateNodeData: (nodeId, data) => {
    set({
      nodes: get().nodes.map((node) =>
        node.id === nodeId
          ? { ...node, data: { ...node.data, ...data } }
          : node
      ),
    });
  },
  
  addEdge: (edge) => {
    set({ edges: [...get().edges, edge] });
  },
  
  deleteEdge: (edgeId) => {
    set({ edges: get().edges.filter((e) => e.id !== edgeId) });
  },
  
  setSelectedNodes: (nodeIds) => set({ selectedNodes: nodeIds }),
  clearSelection: () => set({ selectedNodes: [] }),
  
  resetGraph: () => set({ nodes: [], edges: [], selectedNodes: [] }),
}));
