// Toolbar - Top toolbar with actions and status

import { useGraphStore } from '../stores/graphStore';
import { useUIStore } from '../stores/uiStore';
import { useHistoryStore } from '../stores/historyStore';
import { exportGraph, importGraph } from '../services/persistence';

export default function Toolbar() {
  const { nodes, edges, setNodes, setEdges, resetGraph } = useGraphStore();
  const { connectionStatus } = useUIStore();
  const { undo, redo, canUndo, canRedo } = useHistoryStore();

  const handleExport = () => {
    exportGraph(nodes, edges);
  };

  const handleImport = () => {
    const input = document.createElement('input');
    input.type = 'file';
    input.accept = '.json';
    input.onchange = async (e) => {
      const file = (e.target as HTMLInputElement).files?.[0];
      if (file) {
        try {
          const data = await importGraph(file);
          setNodes(data.nodes);
          setEdges(data.edges);
        } catch (error) {
          alert('Failed to import graph: ' + (error as Error).message);
        }
      }
    };
    input.click();
  };

  const handleClear = () => {
    if (confirm('Are you sure you want to clear the entire graph?')) {
      resetGraph();
    }
  };

  const getStatusColor = () => {
    switch (connectionStatus) {
      case 'connected':
        return '#4CAF50';
      case 'connecting':
        return '#FFC107';
      case 'disconnected':
        return '#F44336';
      default:
        return '#999';
    }
  };

  return (
    <div
      style={{
        position: 'absolute',
        top: 0,
        left: 0,
        right: 0,
        height: '50px',
        background: '#fff',
        borderBottom: '1px solid #ddd',
        display: 'flex',
        alignItems: 'center',
        justifyContent: 'space-between',
        padding: '0 20px',
        zIndex: 20,
      }}
    >
      {/* Left side - Title */}
      <div style={{ fontSize: '18px', fontWeight: 'bold' }}>
        🎯 DAG Editor
      </div>

      {/* Center - Actions */}
      <div style={{ display: 'flex', gap: '8px' }}>
        <button
          onClick={undo}
          disabled={!canUndo()}
          style={{
            padding: '6px 12px',
            fontSize: '14px',
            border: '1px solid #ddd',
            borderRadius: '4px',
            background: canUndo() ? '#fff' : '#f5f5f5',
            cursor: canUndo() ? 'pointer' : 'not-allowed',
          }}
          title="Undo (Ctrl+Z)"
        >
          ↶ Undo
        </button>
        <button
          onClick={redo}
          disabled={!canRedo()}
          style={{
            padding: '6px 12px',
            fontSize: '14px',
            border: '1px solid #ddd',
            borderRadius: '4px',
            background: canRedo() ? '#fff' : '#f5f5f5',
            cursor: canRedo() ? 'pointer' : 'not-allowed',
          }}
          title="Redo (Ctrl+Y)"
        >
          ↷ Redo
        </button>
        <div style={{ width: '1px', background: '#ddd', margin: '0 4px' }} />
        <button
          onClick={handleExport}
          style={{
            padding: '6px 12px',
            fontSize: '14px',
            border: '1px solid #ddd',
            borderRadius: '4px',
            background: '#fff',
            cursor: 'pointer',
          }}
          title="Export graph to JSON"
        >
          💾 Export
        </button>
        <button
          onClick={handleImport}
          style={{
            padding: '6px 12px',
            fontSize: '14px',
            border: '1px solid #ddd',
            borderRadius: '4px',
            background: '#fff',
            cursor: 'pointer',
          }}
          title="Import graph from JSON"
        >
          📂 Import
        </button>
        <button
          onClick={handleClear}
          style={{
            padding: '6px 12px',
            fontSize: '14px',
            border: '1px solid #F44336',
            borderRadius: '4px',
            background: '#fff',
            color: '#F44336',
            cursor: 'pointer',
          }}
          title="Clear all nodes and edges"
        >
          🗑️ Clear
        </button>
      </div>

      {/* Right side - Status */}
      <div style={{ display: 'flex', alignItems: 'center', gap: '12px' }}>
        <div style={{ fontSize: '12px', color: '#666' }}>
          Nodes: <strong>{nodes.length}</strong>
          {' | '}
          Edges: <strong>{edges.length}</strong>
        </div>
        <div style={{ display: 'flex', alignItems: 'center', gap: '6px' }}>
          <div
            style={{
              width: '10px',
              height: '10px',
              borderRadius: '50%',
              background: getStatusColor(),
            }}
          />
          <span style={{ fontSize: '12px', textTransform: 'capitalize' }}>
            {connectionStatus}
          </span>
        </div>
      </div>
    </div>
  );
}
