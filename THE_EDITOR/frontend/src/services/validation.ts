// Graph validation utilities including cycle detection

import type { Edge } from 'reactflow';

/**
 * Detects if adding an edge would create a cycle in the graph
 * Uses DFS to check if there's already a path from target to source
 */
export function wouldCreateCycle(
  edges: Edge[],
  newEdge: { source: string; target: string }
): boolean {
  // Build adjacency list
  const adjacencyList = new Map<string, string[]>();
  
  // Add existing edges
  edges.forEach((edge) => {
    if (!adjacencyList.has(edge.source)) {
      adjacencyList.set(edge.source, []);
    }
    adjacencyList.get(edge.source)!.push(edge.target);
  });
  
  // Add the new edge
  if (!adjacencyList.has(newEdge.source)) {
    adjacencyList.set(newEdge.source, []);
  }
  adjacencyList.get(newEdge.source)!.push(newEdge.target);
  
  // Check if there's a path from newEdge.target back to newEdge.source
  const visited = new Set<string>();
  
  function dfs(current: string, target: string): boolean {
    if (current === target) return true;
    if (visited.has(current)) return false;
    
    visited.add(current);
    const neighbors = adjacencyList.get(current) || [];
    
    for (const neighbor of neighbors) {
      if (dfs(neighbor, target)) return true;
    }
    
    return false;
  }
  
  // Check if there's a path from target to source (which would create a cycle)
  return dfs(newEdge.target, newEdge.source);
}

/**
 * Validates connection based on type compatibility
 * For MVP, all number-to-number connections are valid
 */
export function isValidConnection(
  _sourceNodeType: string,
  _targetNodeType: string
): boolean {
  // For MVP, allow all connections between nodes
  // Can be extended later with specific type rules
  return true;
}

/**
 * Find all nodes that would be affected if a node is deleted
 */
export function findDependentNodes(nodeId: string, edges: Edge[]): string[] {
  const dependents = new Set<string>();
  
  function findChildren(id: string) {
    edges.forEach((edge) => {
      if (edge.source === id && !dependents.has(edge.target)) {
        dependents.add(edge.target);
        findChildren(edge.target);
      }
    });
  }
  
  findChildren(nodeId);
  return Array.from(dependents);
}

/**
 * Topological sort to determine calculation order
 */
export function topologicalSort(
  nodeIds: string[],
  edges: Edge[]
): string[] | null {
  const adjacencyList = new Map<string, string[]>();
  const inDegree = new Map<string, number>();
  
  // Initialize
  nodeIds.forEach((id) => {
    adjacencyList.set(id, []);
    inDegree.set(id, 0);
  });
  
  // Build graph
  edges.forEach((edge) => {
    if (adjacencyList.has(edge.source) && adjacencyList.has(edge.target)) {
      adjacencyList.get(edge.source)!.push(edge.target);
      inDegree.set(edge.target, (inDegree.get(edge.target) || 0) + 1);
    }
  });
  
  // Kahn's algorithm
  const queue: string[] = [];
  const result: string[] = [];
  
  // Find nodes with no incoming edges
  inDegree.forEach((degree, id) => {
    if (degree === 0) queue.push(id);
  });
  
  while (queue.length > 0) {
    const current = queue.shift()!;
    result.push(current);
    
    adjacencyList.get(current)!.forEach((neighbor) => {
      const newDegree = (inDegree.get(neighbor) || 0) - 1;
      inDegree.set(neighbor, newDegree);
      if (newDegree === 0) {
        queue.push(neighbor);
      }
    });
  }
  
  // If result doesn't contain all nodes, there's a cycle
  return result.length === nodeIds.length ? result : null;
}
