#pragma once

#include "Message.h"
#include <string>
#include <ogc/lwp.h>
#include <network.h>

namespace Protocol
{

enum class ConnectionState {
    DISCONNECTED,
    CONNECTING,
    GAME
};

class Connection {
    private:
        u32 m_socket;
        lwp_t m_pollThread;
        ConnectionState m_connectionState;

        static void* poll_call(void*);
        void poll();

        bool handshake();
        bool sendMessage(const Message&);
        Message receiveMessage();
        bool completeRead(void*, int);
    public:
        Connection();
        bool connect(const std::string& host, uint16_t port);
};

}