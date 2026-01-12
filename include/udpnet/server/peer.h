#pragma once
#include <string>
#include <boost/asio.hpp>

// TODO: Convert to class
struct Peer {
	uint32_t peerId;
	std::string sessionToken;
	boost::asio::ip::udp::endpoint socket;

	std::time_t connectionTime;
};
