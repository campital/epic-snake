#include <memory>
#include "Connection.h"
#include "Message.h"

namespace Protocol
{

void* Connection::poll_call(void* instance)
{
    reinterpret_cast<Connection*>(instance)->poll();
    return nullptr;
}

void Connection::poll()
{
    m_connectionState = ConnectionState::CONNECTING;
    handshake();
}

bool Connection::handshake()
{
    Message handshake;
    handshake.messageType = MessageType::HANDSHAKE;
    static const std::string id = "EP1CSNA@K3";
    handshake.arguments.push_back(std::vector<unsigned char>(id.begin(), id.end()));
    return sendMessage(handshake);
}

bool Connection::sendMessage(const Message& message)
{

    //net_write(m_socket, 
}

Message Connection::receiveMessage()
{
    Message message {};
    message.messageType = MessageType::INVALID;
    int32_t len;
    if(!completeRead(&len, sizeof(len)) || len < 1)
        return message;
    auto messageBuf = std::make_unique<uint8_t[]>(len);
    if(!completeRead(messageBuf.get(), len))
        return message;
    
    
}

bool Connection::completeRead(void* buf, int n)
{
    int need = n;
    int nread;
    while((nread = net_read(m_socket, buf, need)) > 0) {
        need -= nread;
        if(need == 0)
            return true;
        buf = static_cast<uint8_t*>(buf) + nread;
    }
    return false;
}

Connection::Connection() : m_connectionState(ConnectionState::DISCONNECTED)
{}

bool Connection::connect(const std::string& host, uint16_t port)
{
    if(net_init() != 0)
        return false;
    m_socket = net_socket(AF_INET, SOCK_STREAM, 0);
    if(m_socket < 0)
        return false;

    sockaddr_in server;
    if(isdigit(host[host.size() - 1])) {
        server.sin_addr.s_addr = inet_addr(host.c_str());
        if(server.sin_addr.s_addr == 0xffffffff)
            return false;
    } else {
        hostent* ip = net_gethostbyname(host.c_str());
        if(ip == NULL)
            return false;
        // u8, u16, u32 are used internally, but not in args
        server.sin_addr.s_addr = *reinterpret_cast<u32*>(ip->h_addr_list[0]);
    }
    server.sin_port = htons(port);
    server.sin_family = AF_INET;
    server.sin_len = 4;
    if(net_connect(m_socket, reinterpret_cast<sockaddr*>(&server), sizeof(server)) < 0)
        return false;
    if(LWP_CreateThread(&m_pollThread, poll_call, this, NULL, 0, 1) < 0)
        return false;
    return true;
}

}