// UI state management

import { create } from 'zustand';

interface UIStore {
  selectedNodeId: string | null;
  isPropertiesPanelOpen: boolean;
  isNodePaletteOpen: boolean;
  connectionStatus: 'connected' | 'disconnected' | 'connecting';
  validationErrors: string[];
  
  setSelectedNodeId: (nodeId: string | null) => void;
  togglePropertiesPanel: () => void;
  toggleNodePalette: () => void;
  setConnectionStatus: (status: 'connected' | 'disconnected' | 'connecting') => void;
  addValidationError: (error: string) => void;
  clearValidationErrors: () => void;
}

export const useUIStore = create<UIStore>((set) => ({
  selectedNodeId: null,
  isPropertiesPanelOpen: true,
  isNodePaletteOpen: true,
  connectionStatus: 'disconnected',
  validationErrors: [],
  
  setSelectedNodeId: (nodeId) => set({ selectedNodeId: nodeId }),
  togglePropertiesPanel: () => set((state) => ({ isPropertiesPanelOpen: !state.isPropertiesPanelOpen })),
  toggleNodePalette: () => set((state) => ({ isNodePaletteOpen: !state.isNodePaletteOpen })),
  setConnectionStatus: (status) => set({ connectionStatus: status }),
  addValidationError: (error) => set((state) => ({ validationErrors: [...state.validationErrors, error] })),
  clearValidationErrors: () => set({ validationErrors: [] }),
}));
