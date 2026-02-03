// Graph type definitions

import type { Node, Edge } from 'reactflow';
import type { NodeData } from './node';

export interface GraphData {
  nodes: Node<NodeData>[];
  edges: Edge[];
  metadata?: {
    author?: string;
    version?: string;
    created?: string;
    modified?: string;
  };
}

export interface ValidationError {
  type: 'cycle' | 'type-mismatch' | 'missing-dependency';
  message: string;
  nodeIds: string[];
}

export interface CalculationResult {
  nodeId: string;
  value: number;
  timestamp: number;
}
