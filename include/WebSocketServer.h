/**
 * WebSocketServer.h - Simple WebSocket Server Implementation
 * 
 * A lightweight, self-contained WebSocket server for broadcasting
 * graph updates to connected visualization clients.
 * 
 * Uses raw sockets with proper WebSocket handshake and framing.
 */

#pragma once

#include <string>
#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include <sstream>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <array>

#ifdef _WIN32
    #define WIN32_LEAN_AND_MEAN
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "ws2_32.lib")
    using SocketType = SOCKET;
    #define INVALID_SOCK INVALID_SOCKET
    #define CLOSE_SOCKET closesocket
#else
    #include <sys/socket.h>
    #include <netinet/in.h>
    #include <unistd.h>
    #include <arpa/inet.h>
    using SocketType = int;
    #define INVALID_SOCK -1
    #define CLOSE_SOCKET close
#endif

// ─────────────────────────────────────────────────────────────
// Base64 Encoding (for WebSocket handshake)
// ─────────────────────────────────────────────────────────────

namespace Base64 {
    static const std::string chars = 
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    inline std::string encode(const unsigned char* data, size_t len) {
        std::string result;
        int val = 0, valb = -6;
        for (size_t i = 0; i < len; i++) {
            val = (val << 8) + data[i];
            valb += 8;
            while (valb >= 0) {
                result.push_back(chars[(val >> valb) & 0x3F]);
                valb -= 6;
            }
        }
        if (valb > -6) result.push_back(chars[((val << 8) >> (valb + 8)) & 0x3F]);
        while (result.size() % 4) result.push_back('=');
        return result;
    }
}

// ─────────────────────────────────────────────────────────────
// Simple SHA-1 Implementation (for WebSocket handshake)
// ─────────────────────────────────────────────────────────────

namespace SHA1 {
    inline std::array<unsigned char, 20> hash(const std::string& input) {
        uint32_t h0 = 0x67452301;
        uint32_t h1 = 0xEFCDAB89;
        uint32_t h2 = 0x98BADCFE;
        uint32_t h3 = 0x10325476;
        uint32_t h4 = 0xC3D2E1F0;

        std::string msg = input;
        size_t origLen = msg.length();
        msg += static_cast<char>(0x80);
        while ((msg.length() % 64) != 56) msg += static_cast<char>(0x00);
        
        uint64_t bitLen = origLen * 8;
        for (int i = 7; i >= 0; i--) {
            msg += static_cast<char>((bitLen >> (i * 8)) & 0xFF);
        }

        auto leftRotate = [](uint32_t x, uint32_t n) { 
            return (x << n) | (x >> (32 - n)); 
        };

        for (size_t chunk = 0; chunk < msg.length(); chunk += 64) {
            uint32_t w[80];
            for (int i = 0; i < 16; i++) {
                w[i] = (static_cast<unsigned char>(msg[chunk + i*4]) << 24) |
                       (static_cast<unsigned char>(msg[chunk + i*4 + 1]) << 16) |
                       (static_cast<unsigned char>(msg[chunk + i*4 + 2]) << 8) |
                       (static_cast<unsigned char>(msg[chunk + i*4 + 3]));
            }
            for (int i = 16; i < 80; i++) {
                w[i] = leftRotate(w[i-3] ^ w[i-8] ^ w[i-14] ^ w[i-16], 1);
            }

            uint32_t a = h0, b = h1, c = h2, d = h3, e = h4;
            for (int i = 0; i < 80; i++) {
                uint32_t f, k;
                if (i < 20) { f = (b & c) | ((~b) & d); k = 0x5A827999; }
                else if (i < 40) { f = b ^ c ^ d; k = 0x6ED9EBA1; }
                else if (i < 60) { f = (b & c) | (b & d) | (c & d); k = 0x8F1BBCDC; }
                else { f = b ^ c ^ d; k = 0xCA62C1D6; }
                uint32_t temp = leftRotate(a, 5) + f + e + k + w[i];
                e = d; d = c; c = leftRotate(b, 30); b = a; a = temp;
            }
            h0 += a; h1 += b; h2 += c; h3 += d; h4 += e;
        }

        std::array<unsigned char, 20> result;
        for (int i = 0; i < 4; i++) {
            result[i] = (h0 >> (24 - i*8)) & 0xFF;
            result[i+4] = (h1 >> (24 - i*8)) & 0xFF;
            result[i+8] = (h2 >> (24 - i*8)) & 0xFF;
            result[i+12] = (h3 >> (24 - i*8)) & 0xFF;
            result[i+16] = (h4 >> (24 - i*8)) & 0xFF;
        }
        return result;
    }
}

// ─────────────────────────────────────────────────────────────
// WebSocket Server Class
// ─────────────────────────────────────────────────────────────

class WebSocketServer {
public:
    using MessageCallback = std::function<void(const std::string&)>;

private:
    int m_port;
    SocketType m_serverSocket = INVALID_SOCK;
    std::vector<SocketType> m_clients;
    std::mutex m_clientsMutex;
    std::thread m_acceptThread;
    std::atomic<bool> m_running{false};
    MessageCallback m_onMessage;

public:
    explicit WebSocketServer(int port = 8080) : m_port(port) {}
    
    ~WebSocketServer() {
        Stop();
    }

    /**
     * Set callback for received messages (optional)
     */
    void OnMessage(MessageCallback callback) {
        m_onMessage = std::move(callback);
    }

    /**
     * Start the WebSocket server
     */
    bool Start() {
#ifdef _WIN32
        WSADATA wsaData;
        if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
            std::cerr << "[WebSocket] WSAStartup failed\n";
            return false;
        }
#endif

        m_serverSocket = socket(AF_INET, SOCK_STREAM, 0);
        if (m_serverSocket == INVALID_SOCK) {
            std::cerr << "[WebSocket] Failed to create socket\n";
            return false;
        }

        // Allow address reuse
        int opt = 1;
#ifdef _WIN32
        setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));
#else
        setsockopt(m_serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
#endif

        sockaddr_in addr{};
        addr.sin_family = AF_INET;
        addr.sin_addr.s_addr = INADDR_ANY;
        addr.sin_port = htons(m_port);

        if (bind(m_serverSocket, (sockaddr*)&addr, sizeof(addr)) < 0) {
            std::cerr << "[WebSocket] Bind failed on port " << m_port << "\n";
            return false;
        }

        if (listen(m_serverSocket, 5) < 0) {
            std::cerr << "[WebSocket] Listen failed\n";
            return false;
        }

        m_running = true;
        m_acceptThread = std::thread(&WebSocketServer::AcceptLoop, this);
        
        std::cout << "[WebSocket] Server started on ws://localhost:" << m_port << "\n";
        return true;
    }

    /**
     * Stop the server and disconnect all clients
     */
    void Stop() {
        m_running = false;
        
        if (m_serverSocket != INVALID_SOCK) {
            CLOSE_SOCKET(m_serverSocket);
            m_serverSocket = INVALID_SOCK;
        }
        
        if (m_acceptThread.joinable()) {
            m_acceptThread.join();
        }

        std::lock_guard<std::mutex> lock(m_clientsMutex);
        for (auto client : m_clients) {
            CLOSE_SOCKET(client);
        }
        m_clients.clear();

#ifdef _WIN32
        WSACleanup();
#endif
    }

    /**
     * Broadcast a message to all connected clients
     */
    void Broadcast(const std::string& message) {
        std::vector<uint8_t> frame = CreateFrame(message);
        
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        std::vector<SocketType> deadClients;

        for (auto client : m_clients) {
            int sent = send(client, (char*)frame.data(), (int)frame.size(), 0);
            if (sent <= 0) {
                deadClients.push_back(client);
            }
        }

        // Remove dead clients
        for (auto dead : deadClients) {
            CLOSE_SOCKET(dead);
            m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), dead), m_clients.end());
        }
    }

    /**
     * Get number of connected clients
     */
    size_t GetClientCount() const {
        return m_clients.size();
    }

private:
    /**
     * Accept loop running in separate thread
     */
    void AcceptLoop() {
        while (m_running) {
            sockaddr_in clientAddr{};
            socklen_t clientLen = sizeof(clientAddr);
            
            SocketType clientSocket = accept(m_serverSocket, (sockaddr*)&clientAddr, &clientLen);
            if (clientSocket == INVALID_SOCK) {
                continue;
            }

            // Handle WebSocket handshake
            if (DoHandshake(clientSocket)) {
                std::lock_guard<std::mutex> lock(m_clientsMutex);
                m_clients.push_back(clientSocket);
                std::cout << "[WebSocket] Client connected. Total: " << m_clients.size() << "\n";
                
                // Start client read thread
                std::thread(&WebSocketServer::ClientReadLoop, this, clientSocket).detach();
            } else {
                CLOSE_SOCKET(clientSocket);
            }
        }
    }

    /**
     * Read loop for a single client
     */
    void ClientReadLoop(SocketType client) {
        char buffer[4096];
        while (m_running) {
            int received = recv(client, buffer, sizeof(buffer), 0);
            if (received <= 0) {
                break;
            }

            // Parse WebSocket frame
            if (received >= 2) {
                uint8_t opcode = buffer[0] & 0x0F;
                if (opcode == 0x08) { // Close frame
                    break;
                }
                if (opcode == 0x01 && m_onMessage) { // Text frame
                    std::string msg = ParseFrame((uint8_t*)buffer, received);
                    if (!msg.empty()) {
                        m_onMessage(msg);
                    }
                }
            }
        }

        // Remove client
        std::lock_guard<std::mutex> lock(m_clientsMutex);
        m_clients.erase(std::remove(m_clients.begin(), m_clients.end(), client), m_clients.end());
        CLOSE_SOCKET(client);
        std::cout << "[WebSocket] Client disconnected. Total: " << m_clients.size() << "\n";
    }

    /**
     * Perform WebSocket handshake
     */
    bool DoHandshake(SocketType client) {
        char buffer[4096];
        int received = recv(client, buffer, sizeof(buffer) - 1, 0);
        if (received <= 0) return false;
        buffer[received] = '\0';

        std::string request(buffer);
        
        // Find Sec-WebSocket-Key
        std::string keyHeader = "Sec-WebSocket-Key: ";
        size_t keyPos = request.find(keyHeader);
        if (keyPos == std::string::npos) return false;
        
        size_t keyEnd = request.find("\r\n", keyPos);
        std::string key = request.substr(keyPos + keyHeader.length(), 
                                          keyEnd - keyPos - keyHeader.length());

        // Generate accept key
        std::string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
        auto hash = SHA1::hash(key + magic);
        std::string acceptKey = Base64::encode(hash.data(), hash.size());

        // Send handshake response
        std::ostringstream response;
        response << "HTTP/1.1 101 Switching Protocols\r\n"
                 << "Upgrade: websocket\r\n"
                 << "Connection: Upgrade\r\n"
                 << "Sec-WebSocket-Accept: " << acceptKey << "\r\n"
                 << "\r\n";

        std::string respStr = response.str();
        return send(client, respStr.c_str(), (int)respStr.length(), 0) > 0;
    }

    /**
     * Create a WebSocket text frame
     */
    std::vector<uint8_t> CreateFrame(const std::string& message) {
        std::vector<uint8_t> frame;
        size_t len = message.length();

        // Opcode for text frame
        frame.push_back(0x81);

        // Length encoding
        if (len <= 125) {
            frame.push_back(static_cast<uint8_t>(len));
        } else if (len <= 65535) {
            frame.push_back(126);
            frame.push_back((len >> 8) & 0xFF);
            frame.push_back(len & 0xFF);
        } else {
            frame.push_back(127);
            for (int i = 7; i >= 0; i--) {
                frame.push_back((len >> (i * 8)) & 0xFF);
            }
        }

        // Payload
        frame.insert(frame.end(), message.begin(), message.end());
        return frame;
    }

    /**
     * Parse a WebSocket frame and extract the message
     */
    std::string ParseFrame(uint8_t* data, int len) {
        if (len < 2) return "";

        bool masked = data[1] & 0x80;
        uint64_t payloadLen = data[1] & 0x7F;
        int offset = 2;

        if (payloadLen == 126) {
            if (len < 4) return "";
            payloadLen = (data[2] << 8) | data[3];
            offset = 4;
        } else if (payloadLen == 127) {
            if (len < 10) return "";
            payloadLen = 0;
            for (int i = 0; i < 8; i++) {
                payloadLen = (payloadLen << 8) | data[2 + i];
            }
            offset = 10;
        }

        uint8_t mask[4] = {0};
        if (masked) {
            if (len < offset + 4) return "";
            std::memcpy(mask, data + offset, 4);
            offset += 4;
        }

        if (len < offset + (int)payloadLen) return "";

        std::string result;
        result.reserve(payloadLen);
        for (uint64_t i = 0; i < payloadLen; i++) {
            char c = data[offset + i];
            if (masked) c ^= mask[i % 4];
            result += c;
        }

        return result;
    }
};
