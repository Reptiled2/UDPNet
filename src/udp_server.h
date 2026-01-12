#pragma once
#include <functional>
#include <iostream>

#include <udpnet/udp_server.h>

using boost::asio::ip::udp;
using boost::asio::awaitable;
using boost::asio::co_spawn;

class UDPServer {
private:
	udp::socket p_serverSocket;
	boost::asio::io_context p_iocontext;
	std::unique_ptr<std::vector<Peer>> p_peers;
	uint32_t p_peerCount;
	std::unordered_map<Event, std::vector<std::function<void()>>> dispatchMap;

protected:
	size_t MAX_PAYLOAD_SIZE;

private:
	awaitable<void> send(std::shared_ptr<std::vector<char>> buffer, udp::endpoint remoteInfo);

	void handleMessage(std::unique_ptr<std::vector<char>> receivedBuffer, std::shared_ptr<udp::endpoint> remoteInfo);

	awaitable<void> receiveMessage();

public:
	UDPServer(const int& PORT, const size_t& MAX_PAYLOAD_SIZE = 2048);

	void start();

	awaitable<void> sendMessage(Packet& packet);

	void on(const Event& event, const std::function<void()>& function);
};