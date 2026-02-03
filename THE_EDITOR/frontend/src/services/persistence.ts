// Graph persistence service for save/load functionality

import type { Node, Edge } from 'reactflow';
import type { NodeData } from '../types/node';
import type { GraphData } from '../types/graph';

const STORAGE_KEY = 'dag-editor-graph';
const AUTO_SAVE_INTERVAL = 30000; // 30 seconds

/**
 * Save graph to localStorage
 */
export function saveToLocalStorage(nodes: Node<NodeData>[], edges: Edge[]): void {
  try {
    const graphData: GraphData = {
      nodes,
      edges,
      metadata: {
        version: '1.0.0',
        modified: new Date().toISOString(),
      },
    };
    localStorage.setItem(STORAGE_KEY, JSON.stringify(graphData));
  } catch (error) {
    console.error('Failed to save to localStorage:', error);
  }
}

/**
 * Load graph from localStorage
 */
export function loadFromLocalStorage(): GraphData | null {
  try {
    const data = localStorage.getItem(STORAGE_KEY);
    if (!data) return null;
    return JSON.parse(data) as GraphData;
  } catch (error) {
    console.error('Failed to load from localStorage:', error);
    return null;
  }
}

/**
 * Export graph as JSON file (download)
 */
export function exportGraph(nodes: Node<NodeData>[], edges: Edge[], filename: string = 'graph.dagraph.json'): void {
  const graphData: GraphData = {
    nodes,
    edges,
    metadata: {
      version: '1.0.0',
      created: new Date().toISOString(),
      author: 'DAG Editor',
    },
  };
  
  const dataStr = JSON.stringify(graphData, null, 2);
  const dataUri = 'data:application/json;charset=utf-8,' + encodeURIComponent(dataStr);
  
  const exportFileDefaultName = filename;
  const linkElement = document.createElement('a');
  linkElement.setAttribute('href', dataUri);
  linkElement.setAttribute('download', exportFileDefaultName);
  linkElement.click();
}

/**
 * Import graph from JSON file
 */
export function importGraph(file: File): Promise<GraphData> {
  return new Promise((resolve, reject) => {
    const reader = new FileReader();
    
    reader.onload = (e) => {
      try {
        const data = JSON.parse(e.target?.result as string) as GraphData;
        resolve(data);
      } catch (error) {
        reject(new Error('Invalid JSON file'));
      }
    };
    
    reader.onerror = () => reject(new Error('Failed to read file'));
    reader.readAsText(file);
  });
}

/**
 * Set up auto-save interval
 */
export function setupAutoSave(
  getNodes: () => Node<NodeData>[],
  getEdges: () => Edge[]
): () => void {
  const intervalId = setInterval(() => {
    const nodes = getNodes();
    const edges = getEdges();
    if (nodes.length > 0 || edges.length > 0) {
      saveToLocalStorage(nodes, edges);
      console.log('[AutoSave] Graph saved to localStorage');
    }
  }, AUTO_SAVE_INTERVAL);
  
  // Return cleanup function
  return () => clearInterval(intervalId);
}

/**
 * Clear localStorage
 */
export function clearLocalStorage(): void {
  localStorage.removeItem(STORAGE_KEY);
}
