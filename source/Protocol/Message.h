#pragma once

#include <vector>

namespace Protocol
{

// structure of a message:
// first line is header, contains length (int) and message type (byte)
// ex: "\x00\x00\x00\xef\x01"

// following data is arguments, which can be parsed by the parser for the message type (arbitrary data)
// ints should be transmitted in big-endian form
// must start with 4 byte length
// ex: "\x00\x00\x00\x01\xee"

enum class MessageType {
    INVALID = 0,
    HANDSHAKE,
    UPDATE,
    QUIT
};


struct Message {
    MessageType messageType;
    std::vector<std::vector<uint8_t>> arguments;
};

}