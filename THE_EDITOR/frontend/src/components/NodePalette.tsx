// NodePalette - Left sidebar with draggable node types

import { NodeType } from '../types/node';

const NODE_TYPES = [
  { type: NodeType.STAT, icon: '🎯', color: '#2196F3', description: 'Base value' },
  { type: NodeType.DERIVED, icon: '📊', color: '#4CAF50', description: 'Calculated' },
  { type: NodeType.CONDITIONAL, icon: '🔀', color: '#FFC107', description: 'If/Then logic' },
  { type: NodeType.CAP, icon: '🔒', color: '#FF5722', description: 'Min/Max limits' },
  { type: NodeType.MODIFIER, icon: '⚡', color: '#9C27B0', description: 'Flat/Inc/More' },
];

interface NodePaletteProps {
  isOpen: boolean;
}

export default function NodePalette({ isOpen }: NodePaletteProps) {
  const onDragStart = (event: React.DragEvent, nodeType: NodeType) => {
    event.dataTransfer.setData('application/reactflow', nodeType);
    event.dataTransfer.effectAllowed = 'move';
  };

  if (!isOpen) return null;

  return (
    <div
      style={{
        position: 'absolute',
        left: 0,
        top: 0,
        bottom: 0,
        width: '240px',
        background: '#f5f5f5',
        borderRight: '1px solid #ddd',
        padding: '20px',
        overflowY: 'auto',
        zIndex: 10,
      }}
    >
      <h2 style={{ fontSize: '18px', fontWeight: 'bold', marginBottom: '16px' }}>
        Node Palette
      </h2>
      
      <div style={{ display: 'flex', flexDirection: 'column', gap: '12px' }}>
        {NODE_TYPES.map((nodeType) => (
          <div
            key={nodeType.type}
            draggable
            onDragStart={(e) => onDragStart(e, nodeType.type)}
            style={{
              padding: '12px',
              background: '#fff',
              border: `2px solid ${nodeType.color}`,
              borderRadius: '8px',
              cursor: 'grab',
              transition: 'all 0.2s',
            }}
            onMouseDown={(e) => {
              e.currentTarget.style.cursor = 'grabbing';
            }}
            onMouseUp={(e) => {
              e.currentTarget.style.cursor = 'grab';
            }}
          >
            <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '4px' }}>
              <span style={{ fontSize: '20px' }}>{nodeType.icon}</span>
              <span style={{ fontWeight: 'bold', fontSize: '14px' }}>{nodeType.type}</span>
            </div>
            <p style={{ fontSize: '12px', color: '#666', margin: 0 }}>
              {nodeType.description}
            </p>
          </div>
        ))}
      </div>

      <div style={{ marginTop: '24px', padding: '12px', background: '#fff', borderRadius: '8px', fontSize: '12px' }}>
        <strong>💡 Tip:</strong> Drag nodes onto the canvas to create them
      </div>
    </div>
  );
}
