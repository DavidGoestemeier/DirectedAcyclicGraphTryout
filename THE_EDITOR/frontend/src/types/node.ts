// Node type definitions for the DAG editor

export const NodeType = {
  STAT: 'StatNode',
  DERIVED: 'DerivedNode',
  CONDITIONAL: 'ConditionalNode',
  CAP: 'CapNode',
  MODIFIER: 'ModifierNode',
} as const;

export type NodeType = typeof NodeType[keyof typeof NodeType];

export const ModifierType = {
  FLAT: 'Flat',
  INCREASED: 'Increased',
  MORE: 'More',
} as const;

export type ModifierType = typeof ModifierType[keyof typeof ModifierType];

export interface NodeData {
  id: string;
  label: string;
  type: NodeType;
  baseValue?: number;
  calculatedValue?: number;
  modifierType?: ModifierType;
  modifierValue?: number;
  minValue?: number;
  maxValue?: number;
  condition?: string;
  isDirty?: boolean;
}

export interface CustomNodeProps {
  id: string;
  data: NodeData;
  selected: boolean;
}

