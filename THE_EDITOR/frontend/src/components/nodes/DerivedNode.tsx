// DerivedNode - Calculated node component

import { memo } from 'react';
import { Handle, Position } from 'reactflow';
import type { CustomNodeProps } from '../../types/node';
import { NodeType } from '../../types/node';

function DerivedNode({ data, selected }: CustomNodeProps) {
  return (
    <div
      className={`derived-node ${selected ? 'selected' : ''}`}
      style={{
        padding: '10px 15px',
        borderRadius: '8px',
        border: `2px solid ${selected ? '#81C784' : '#4CAF50'}`,
        background: '#fff',
        minWidth: '180px',
        boxShadow: selected ? '0 4px 8px rgba(0,0,0,0.2)' : '0 2px 4px rgba(0,0,0,0.1)',
      }}
    >
      {/* Header */}
      <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px' }}>
        <span style={{ fontSize: '16px' }}>📊</span>
        <span style={{ fontSize: '11px', color: '#666', textTransform: 'uppercase' }}>
          {NodeType.DERIVED}
        </span>
      </div>

      {/* Content */}
      <div>
        <div style={{ fontWeight: 'bold', fontSize: '14px', marginBottom: '4px' }}>
          {data.label}
        </div>
        {data.calculatedValue !== undefined && (
          <div style={{ fontSize: '12px', color: '#666' }}>
            Value: <span style={{ fontWeight: 'bold', color: '#4CAF50' }}>{data.calculatedValue.toFixed(2)}</span>
          </div>
        )}
        {data.isDirty && (
          <div style={{ fontSize: '11px', color: '#F44336', marginTop: '4px' }}>
            ⚠️ Needs recalculation
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

export default memo(DerivedNode);
