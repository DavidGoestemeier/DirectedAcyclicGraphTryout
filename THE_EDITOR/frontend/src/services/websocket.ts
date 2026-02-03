// WebSocket service for backend communication

import type { Node, Edge } from 'reactflow';
import type { NodeData } from '../types/node';

export type ConnectionStatus = 'connected' | 'disconnected' | 'connecting';
export type MessageHandler = (data: any) => void;

class WebSocketService {
  private ws: WebSocket | null = null;
  private url: string;
  private reconnectInterval: number = 5000;
  private reconnectTimer: ReturnType<typeof setTimeout> | null = null;
  private messageHandlers: Set<MessageHandler> = new Set();
  private statusChangeHandlers: Set<(status: ConnectionStatus) => void> = new Set();
  private status: ConnectionStatus = 'disconnected';
  private sendQueue: string[] = [];
  private debounceTimer: ReturnType<typeof setTimeout> | null = null;
  private debounceDelay: number = 300;

  constructor(url: string = 'ws://localhost:8765') {
    this.url = url;
  }

  connect(): void {
    if (this.ws?.readyState === WebSocket.OPEN) {
      return;
    }

    this.setStatus('connecting');
    
    try {
      this.ws = new WebSocket(this.url);

      this.ws.onopen = () => {
        console.log('[WebSocket] Connected to backend');
        this.setStatus('connected');
        this.flushSendQueue();
      };

      this.ws.onmessage = (event) => {
        try {
          const data = JSON.parse(event.data);
          this.notifyMessageHandlers(data);
        } catch (error) {
          console.error('[WebSocket] Failed to parse message:', error);
        }
      };

      this.ws.onerror = (error) => {
        console.error('[WebSocket] Error:', error);
      };

      this.ws.onclose = () => {
        console.log('[WebSocket] Disconnected');
        this.setStatus('disconnected');
        this.scheduleReconnect();
      };
    } catch (error) {
      console.error('[WebSocket] Connection failed:', error);
      this.setStatus('disconnected');
      this.scheduleReconnect();
    }
  }

  disconnect(): void {
    if (this.reconnectTimer) {
      clearTimeout(this.reconnectTimer);
      this.reconnectTimer = null;
    }
    
    if (this.ws) {
      this.ws.close();
      this.ws = null;
    }
    
    this.setStatus('disconnected');
  }

  /**
   * Send graph update to backend (debounced)
   */
  sendGraphUpdate(nodes: Node<NodeData>[], edges: Edge[]): void {
    // Clear existing debounce timer
    if (this.debounceTimer) {
      clearTimeout(this.debounceTimer);
    }

    // Set new debounce timer
    this.debounceTimer = setTimeout(() => {
      const message = JSON.stringify({
        type: 'graph_update',
        nodes: nodes.map((n) => ({
          id: n.id,
          label: n.data.label,
          type: n.data.type,
          baseValue: n.data.baseValue,
          isDirty: n.data.isDirty,
        })),
        edges: edges.map((e) => ({
          from: e.source,
          to: e.target,
        })),
        timestamp: Date.now(),
      });

      this.send(message);
    }, this.debounceDelay);
  }

  private send(message: string): void {
    if (this.ws?.readyState === WebSocket.OPEN) {
      this.ws.send(message);
    } else {
      // Queue message for when connection is restored
      this.sendQueue.push(message);
    }
  }

  private flushSendQueue(): void {
    while (this.sendQueue.length > 0) {
      const message = this.sendQueue.shift();
      if (message && this.ws?.readyState === WebSocket.OPEN) {
        this.ws.send(message);
      }
    }
  }

  private scheduleReconnect(): void {
    if (this.reconnectTimer) return;

    this.reconnectTimer = setTimeout(() => {
      console.log('[WebSocket] Attempting to reconnect...');
      this.reconnectTimer = null;
      this.connect();
    }, this.reconnectInterval);
  }

  private setStatus(status: ConnectionStatus): void {
    this.status = status;
    this.statusChangeHandlers.forEach((handler) => handler(status));
  }

  private notifyMessageHandlers(data: any): void {
    this.messageHandlers.forEach((handler) => handler(data));
  }

  getStatus(): ConnectionStatus {
    return this.status;
  }

  onMessage(handler: MessageHandler): () => void {
    this.messageHandlers.add(handler);
    return () => this.messageHandlers.delete(handler);
  }

  onStatusChange(handler: (status: ConnectionStatus) => void): () => void {
    this.statusChangeHandlers.add(handler);
    return () => this.statusChangeHandlers.delete(handler);
  }
}

// Export singleton instance
export const websocketService = new WebSocketService();
