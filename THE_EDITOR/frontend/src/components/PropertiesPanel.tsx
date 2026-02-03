// PropertiesPanel - Right sidebar for editing node properties

import { useGraphStore } from '../stores/graphStore';
import { useUIStore } from '../stores/uiStore';
import { NodeType, ModifierType } from '../types/node';

interface PropertiesPanelProps {
  isOpen: boolean;
}

export default function PropertiesPanel({ isOpen }: PropertiesPanelProps) {
  const { nodes, updateNodeData } = useGraphStore();
  const { selectedNodeId } = useUIStore();

  if (!isOpen) return null;

  const selectedNode = nodes.find((n) => n.id === selectedNodeId);

  if (!selectedNode) {
    return (
      <div
        style={{
          position: 'absolute',
          right: 0,
          top: 0,
          bottom: 0,
          width: '300px',
          background: '#f5f5f5',
          borderLeft: '1px solid #ddd',
          padding: '20px',
          overflowY: 'auto',
          zIndex: 10,
        }}
      >
        <h2 style={{ fontSize: '18px', fontWeight: 'bold', marginBottom: '16px' }}>
          Properties
        </h2>
        <p style={{ color: '#666', fontSize: '14px' }}>
          Select a node to edit its properties
        </p>
      </div>
    );
  }

  const handleChange = (field: string, value: any) => {
    updateNodeData(selectedNode.id, { [field]: value });
  };

  return (
    <div
      style={{
        position: 'absolute',
        right: 0,
        top: 0,
        bottom: 0,
        width: '300px',
        background: '#f5f5f5',
        borderLeft: '1px solid #ddd',
        padding: '20px',
        overflowY: 'auto',
        zIndex: 10,
      }}
    >
      <h2 style={{ fontSize: '18px', fontWeight: 'bold', marginBottom: '16px' }}>
        Properties
      </h2>

      {/* Node ID */}
      <div style={{ marginBottom: '16px' }}>
        <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
          Node ID
        </label>
        <div style={{ fontSize: '14px', color: '#333' }}>{selectedNode.id}</div>
      </div>

      {/* Node Type */}
      <div style={{ marginBottom: '16px' }}>
        <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
          Type
        </label>
        <div style={{ fontSize: '14px', color: '#333' }}>{selectedNode.data.type}</div>
      </div>

      {/* Label */}
      <div style={{ marginBottom: '16px' }}>
        <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
          Label
        </label>
        <input
          type="text"
          value={selectedNode.data.label}
          onChange={(e) => handleChange('label', e.target.value)}
          style={{
            width: '100%',
            padding: '8px',
            fontSize: '14px',
            border: '1px solid #ddd',
            borderRadius: '4px',
          }}
        />
      </div>

      {/* Base Value (for StatNode) */}
      {selectedNode.data.type === NodeType.STAT && (
        <div style={{ marginBottom: '16px' }}>
          <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
            Base Value
          </label>
          <input
            type="number"
            value={selectedNode.data.baseValue ?? 0}
            onChange={(e) => handleChange('baseValue', parseFloat(e.target.value))}
            style={{
              width: '100%',
              padding: '8px',
              fontSize: '14px',
              border: '1px solid #ddd',
              borderRadius: '4px',
            }}
          />
        </div>
      )}

      {/* Modifier Type and Value (for ModifierNode) */}
      {selectedNode.data.type === NodeType.MODIFIER && (
        <>
          <div style={{ marginBottom: '16px' }}>
            <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
              Modifier Type
            </label>
            <select
              value={selectedNode.data.modifierType ?? ModifierType.FLAT}
              onChange={(e) => handleChange('modifierType', e.target.value)}
              style={{
                width: '100%',
                padding: '8px',
                fontSize: '14px',
                border: '1px solid #ddd',
                borderRadius: '4px',
              }}
            >
              <option value={ModifierType.FLAT}>Flat</option>
              <option value={ModifierType.INCREASED}>Increased</option>
              <option value={ModifierType.MORE}>More</option>
            </select>
          </div>
          <div style={{ marginBottom: '16px' }}>
            <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
              Modifier Value
            </label>
            <input
              type="number"
              value={selectedNode.data.modifierValue ?? 0}
              onChange={(e) => handleChange('modifierValue', parseFloat(e.target.value))}
              step="0.01"
              style={{
                width: '100%',
                padding: '8px',
                fontSize: '14px',
                border: '1px solid #ddd',
                borderRadius: '4px',
              }}
            />
          </div>
        </>
      )}

      {/* Min/Max Values (for CapNode) */}
      {selectedNode.data.type === NodeType.CAP && (
        <>
          <div style={{ marginBottom: '16px' }}>
            <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
              Min Value
            </label>
            <input
              type="number"
              value={selectedNode.data.minValue ?? 0}
              onChange={(e) => handleChange('minValue', parseFloat(e.target.value))}
              style={{
                width: '100%',
                padding: '8px',
                fontSize: '14px',
                border: '1px solid #ddd',
                borderRadius: '4px',
              }}
            />
          </div>
          <div style={{ marginBottom: '16px' }}>
            <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
              Max Value
            </label>
            <input
              type="number"
              value={selectedNode.data.maxValue ?? 100}
              onChange={(e) => handleChange('maxValue', parseFloat(e.target.value))}
              style={{
                width: '100%',
                padding: '8px',
                fontSize: '14px',
                border: '1px solid #ddd',
                borderRadius: '4px',
              }}
            />
          </div>
        </>
      )}

      {/* Condition (for ConditionalNode) */}
      {selectedNode.data.type === NodeType.CONDITIONAL && (
        <div style={{ marginBottom: '16px' }}>
          <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
            Condition
          </label>
          <input
            type="text"
            value={selectedNode.data.condition ?? ''}
            onChange={(e) => handleChange('condition', e.target.value)}
            placeholder="e.g., value > 50"
            style={{
              width: '100%',
              padding: '8px',
              fontSize: '14px',
              border: '1px solid #ddd',
              borderRadius: '4px',
            }}
          />
        </div>
      )}

      {/* Calculated Value (readonly) */}
      {selectedNode.data.calculatedValue !== undefined && (
        <div style={{ marginBottom: '16px' }}>
          <label style={{ display: 'block', fontSize: '12px', fontWeight: 'bold', marginBottom: '4px', color: '#666' }}>
            Calculated Value
          </label>
          <div
            style={{
              padding: '8px',
              fontSize: '14px',
              background: '#e0e0e0',
              border: '1px solid #ddd',
              borderRadius: '4px',
              fontWeight: 'bold',
              color: '#4CAF50',
            }}
          >
            {selectedNode.data.calculatedValue.toFixed(2)}
          </div>
        </div>
      )}
    </div>
  );
}
