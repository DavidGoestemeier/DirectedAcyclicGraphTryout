// ConditionalNode - Conditional logic node component

import { memo } from 'react';
import { Handle, Position } from 'reactflow';
import type { CustomNodeProps } from '../../types/node';
import { NodeType } from '../../types/node';

function ConditionalNode({ data, selected }: CustomNodeProps) {
  return (
    <div
      className={`conditional-node ${selected ? 'selected' : ''}`}
      style={{
        padding: '10px 15px',
        borderRadius: '8px',
        border: `2px solid ${selected ? '#FFD54F' : '#FFC107'}`,
        background: '#fff',
        minWidth: '180px',
        boxShadow: selected ? '0 4px 8px rgba(0,0,0,0.2)' : '0 2px 4px rgba(0,0,0,0.1)',
      }}
    >
      {/* Header */}
      <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px' }}>
        <span style={{ fontSize: '16px' }}>🔀</span>
        <span style={{ fontSize: '11px', color: '#666', textTransform: 'uppercase' }}>
          {NodeType.CONDITIONAL}
        </span>
      </div>

      {/* Content */}
      <div>
        <div style={{ fontWeight: 'bold', fontSize: '14px', marginBottom: '4px' }}>
          {data.label}
        </div>
        {data.condition && (
          <div style={{ fontSize: '11px', color: '#666', fontStyle: 'italic', marginTop: '4px' }}>
            If: {data.condition}
          </div>
        )}
        {data.calculatedValue !== undefined && (
          <div style={{ fontSize: '12px', color: '#666', marginTop: '4px' }}>
            Value: <span style={{ fontWeight: 'bold', color: '#FFC107' }}>{data.calculatedValue.toFixed(2)}</span>
          </div>
        )}
      </div>

      {/* Connection Handles */}
      <Handle
        type="target"
        position={Position.Left}
        style={{ background: '#555' }}
        id="in"
      />
      <Handle
        type="source"
        position={Position.Right}
        style={{ background: '#555' }}
        id="out"
      />
    </div>
  );
}

export default memo(ConditionalNode);
