// Main App component for DAG Editor

import { useEffect } from 'react';
import { ReactFlowProvider } from 'reactflow';
import Canvas from './components/Canvas';
import Toolbar from './components/Toolbar';
import NodePalette from './components/NodePalette';
import PropertiesPanel from './components/PropertiesPanel';
import { useUIStore } from './stores/uiStore';
import { useGraphStore } from './stores/graphStore';
import { useHistoryStore } from './stores/historyStore';
import { websocketService } from './services/websocket';
import { setupAutoSave, loadFromLocalStorage } from './services/persistence';
import 'reactflow/dist/style.css';

function App() {
  const { isNodePaletteOpen, isPropertiesPanelOpen, setConnectionStatus } = useUIStore();
  const { nodes, edges, setNodes, setEdges } = useGraphStore();
  const { undo, redo, canUndo, canRedo } = useHistoryStore();

  // Initialize app
  useEffect(() => {
    // Load saved graph from localStorage
    const savedGraph = loadFromLocalStorage();
    if (savedGraph) {
      setNodes(savedGraph.nodes);
      setEdges(savedGraph.edges);
      console.log('[App] Loaded graph from localStorage');
    }

    // Set up auto-save
    const cleanupAutoSave = setupAutoSave(
      () => nodes,
      () => edges
    );

    // Connect to WebSocket backend
    websocketService.connect();
    
    // Listen for connection status changes
    const unsubscribeStatus = websocketService.onStatusChange((status) => {
      setConnectionStatus(status);
    });

    // Listen for messages from backend
    const unsubscribeMessages = websocketService.onMessage((data) => {
      if (data.type === 'calculation_result') {
        // Update node values from backend calculations
        console.log('[App] Received calculation result:', data);
        // TODO: Update node calculated values
      }
    });

    // Cleanup on unmount
    return () => {
      cleanupAutoSave();
      unsubscribeStatus();
      unsubscribeMessages();
      websocketService.disconnect();
    };
  }, []);

  // Send graph updates to backend
  useEffect(() => {
    if (nodes.length > 0 || edges.length > 0) {
      websocketService.sendGraphUpdate(nodes, edges);
    }
  }, [nodes, edges]);

  // Keyboard shortcuts
  useEffect(() => {
    const handleKeyDown = (event: KeyboardEvent) => {
      // Ctrl+Z - Undo
      if (event.ctrlKey && event.key === 'z' && !event.shiftKey) {
        event.preventDefault();
        if (canUndo()) {
          undo();
        }
      }
      // Ctrl+Y or Ctrl+Shift+Z - Redo
      else if ((event.ctrlKey && event.key === 'y') || (event.ctrlKey && event.shiftKey && event.key === 'z')) {
        event.preventDefault();
        if (canRedo()) {
          redo();
        }
      }
    };

    window.addEventListener('keydown', handleKeyDown);
    return () => window.removeEventListener('keydown', handleKeyDown);
  }, [undo, redo, canUndo, canRedo]);

  return (
    <div style={{ width: '100vw', height: '100vh', overflow: 'hidden', position: 'relative' }}>
      <ReactFlowProvider>
        <Toolbar />
        <NodePalette isOpen={isNodePaletteOpen} />
        <PropertiesPanel isOpen={isPropertiesPanelOpen} />
        
        <div
          style={{
            position: 'absolute',
            top: '50px',
            left: isNodePaletteOpen ? '240px' : '0',
            right: isPropertiesPanelOpen ? '300px' : '0',
            bottom: 0,
            transition: 'left 0.3s, right 0.3s',
          }}
        >
          <Canvas />
        </div>
      </ReactFlowProvider>
    </div>
  );
}

export default App;
