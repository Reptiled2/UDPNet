#pragma once

#include "udpnet/server/buffer.h"

enum class PacketType {
	CONNECT = 1,
	DISCONNECT = 2,
	MESSAGE = 3,
	KEEPALIVE = 4,
	ALREADY_CONNECTED = 5
};

enum class Event {
	CONNECT = 1,
	DISCONNECT = 2,
	MESSAGE = 3,
	KEEPALIVE = 4
};

struct Packet {
	std::vector<uint32_t> peerIds;
	PacketType type;
	BufferVector buffer;
};
