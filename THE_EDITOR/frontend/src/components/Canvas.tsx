// Canvas - Main editor canvas with React Flow

import { useCallback, useRef, useEffect } from 'react';
import ReactFlow, {
  Background,
  Controls,
  MiniMap,
} from 'reactflow';
import type { Connection } from 'reactflow';
import 'reactflow/dist/style.css';

import { useGraphStore } from '../stores/graphStore';
import { useUIStore } from '../stores/uiStore';
import { NodeType } from '../types/node';
import type { NodeData } from '../types/node';
import { wouldCreateCycle } from '../services/validation';

// Import custom node components
import StatNode from './nodes/StatNode';
import DerivedNode from './nodes/DerivedNode';
import ModifierNode from './nodes/ModifierNode';
import ConditionalNode from './nodes/ConditionalNode';
import CapNode from './nodes/CapNode';

// Define node types mapping
const nodeTypes = {
  [NodeType.STAT]: StatNode,
  [NodeType.DERIVED]: DerivedNode,
  [NodeType.MODIFIER]: ModifierNode,
  [NodeType.CONDITIONAL]: ConditionalNode,
  [NodeType.CAP]: CapNode,
};

let nodeIdCounter = 1;

export default function Canvas() {
  const reactFlowWrapper = useRef<HTMLDivElement>(null);
  const {
    nodes,
    edges,
    onNodesChange,
    onEdgesChange,
    addNode,
    addEdge: addEdgeToStore,
    setNodes,
    setEdges,
  } = useGraphStore();
  const { setSelectedNodeId, addValidationError, clearValidationErrors } = useUIStore();

  // Handle node selection
  const onNodeClick = useCallback(
    (_event: React.MouseEvent, node: any) => {
      setSelectedNodeId(node.id);
    },
    [setSelectedNodeId]
  );

  // Handle pane click (deselect)
  const onPaneClick = useCallback(() => {
    setSelectedNodeId(null);
  }, [setSelectedNodeId]);

  // Handle connection creation
  const onConnect = useCallback(
    (connection: Connection) => {
      clearValidationErrors();

      // Validate: Check for cycles
      if (wouldCreateCycle(edges, { source: connection.source!, target: connection.target! })) {
        addValidationError('Cannot create connection: would create a cycle');
        alert('Cannot create connection: would create a cycle in the graph');
        return;
      }

      // Add edge
      const newEdge = {
        id: `e${connection.source}-${connection.target}`,
        source: connection.source!,
        target: connection.target!,
      };
      addEdgeToStore(newEdge);
    },
    [edges, addEdgeToStore, addValidationError, clearValidationErrors]
  );

  // Handle drag over
  const onDragOver = useCallback((event: React.DragEvent) => {
    event.preventDefault();
    event.dataTransfer.dropEffect = 'move';
  }, []);

  // Handle drop - create new node
  const onDrop = useCallback(
    (event: React.DragEvent) => {
      event.preventDefault();

      const reactFlowBounds = reactFlowWrapper.current?.getBoundingClientRect();
      const type = event.dataTransfer.getData('application/reactflow') as NodeType;

      if (!type || !reactFlowBounds) return;

      // Calculate position
      const position = {
        x: event.clientX - reactFlowBounds.left - 90, // Center the node
        y: event.clientY - reactFlowBounds.top - 50,
      };

      // Create node data based on type
      const nodeData: NodeData = {
        id: `node-${nodeIdCounter}`,
        label: `${type} ${nodeIdCounter}`,
        type,
      };

      // Set default values based on type
      if (type === NodeType.STAT) {
        nodeData.baseValue = 10;
      } else if (type === NodeType.MODIFIER) {
        nodeData.modifierType = 'Flat' as any;
        nodeData.modifierValue = 0;
      } else if (type === NodeType.CAP) {
        nodeData.minValue = 0;
        nodeData.maxValue = 100;
      } else if (type === NodeType.CONDITIONAL) {
        nodeData.condition = '';
      }

      const newNode = {
        id: nodeData.id,
        type,
        position,
        data: nodeData,
      };

      nodeIdCounter++;
      addNode(newNode);
    },
    [addNode]
  );

  // Handle keyboard shortcuts
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      // Delete key
      if (event.key === 'Delete') {
        const selectedNodes = nodes.filter((node) => node.selected);
        selectedNodes.forEach((node) => {
          // Remove node and connected edges
          setNodes(nodes.filter((n) => n.id !== node.id));
          setEdges(edges.filter((e) => e.source !== node.id && e.target !== node.id));
        });
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [nodes, edges, setNodes, setEdges]);

  return (
    <div
      ref={reactFlowWrapper}
      style={{ width: '100%', height: '100%' }}
    >
      <ReactFlow
        nodes={nodes}
        edges={edges}
        onNodesChange={onNodesChange}
        onEdgesChange={onEdgesChange}
        onConnect={onConnect}
        onNodeClick={onNodeClick}
        onPaneClick={onPaneClick}
        onDrop={onDrop}
        onDragOver={onDragOver}
        nodeTypes={nodeTypes}
        fitView
        attributionPosition="bottom-left"
      >
        <Background />
        <Controls />
        <MiniMap
          nodeColor={(node) => {
            switch (node.type) {
              case NodeType.STAT:
                return '#2196F3';
              case NodeType.DERIVED:
                return '#4CAF50';
              case NodeType.MODIFIER:
                return '#9C27B0';
              case NodeType.CONDITIONAL:
                return '#FFC107';
              case NodeType.CAP:
                return '#FF5722';
              default:
                return '#999';
            }
          }}
        />
      </ReactFlow>
    </div>
  );
}
