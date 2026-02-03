// CapNode - Min/Max cap node component

import { memo } from 'react';
import { Handle, Position } from 'reactflow';
import type { CustomNodeProps } from '../../types/node';
import { NodeType } from '../../types/node';

function CapNode({ data, selected }: CustomNodeProps) {
  return (
    <div
      className={`cap-node ${selected ? 'selected' : ''}`}
      style={{
        padding: '10px 15px',
        borderRadius: '8px',
        border: `2px solid ${selected ? '#FF7043' : '#FF5722'}`,
        background: '#fff',
        minWidth: '180px',
        boxShadow: selected ? '0 4px 8px rgba(0,0,0,0.2)' : '0 2px 4px rgba(0,0,0,0.1)',
      }}
    >
      {/* Header */}
      <div style={{ display: 'flex', alignItems: 'center', gap: '8px', marginBottom: '8px' }}>
        <span style={{ fontSize: '16px' }}>🔒</span>
        <span style={{ fontSize: '11px', color: '#666', textTransform: 'uppercase' }}>
          {NodeType.CAP}
        </span>
      </div>

      {/* Content */}
      <div>
        <div style={{ fontWeight: 'bold', fontSize: '14px', marginBottom: '4px' }}>
          {data.label}
        </div>
        <div style={{ fontSize: '12px', color: '#666' }}>
          Min: <span style={{ fontWeight: 'bold' }}>{data.minValue ?? 0}</span>
          {' | '}
          Max: <span style={{ fontWeight: 'bold' }}>{data.maxValue ?? 100}</span>
        </div>
        {data.calculatedValue !== undefined && (
          <div style={{ fontSize: '12px', color: '#666', marginTop: '4px' }}>
            Value: <span style={{ fontWeight: 'bold', color: '#FF5722' }}>{data.calculatedValue.toFixed(2)}</span>
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

export default memo(CapNode);
