#include "pch.h"

#include <functional>
#include <iostream>

#include "utility/token_generator.h"
#include <udpnet/udp_server.h>
#include <utility/helpers.h>

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
	size_t MAX_PAYLOAD_SIZE = 2048;

private:
	awaitable<void> send(std::shared_ptr<std::vector<char>> buffer,
		udp::endpoint remoteInfo) {
		try {
			co_await p_serverSocket.async_send_to(boost::asio::buffer(*buffer), remoteInfo);
		}
		catch (std::exception& err) {
			std::cout << "Error: " << err.what() << "\n";
		}
	};

	void handleMessage(std::unique_ptr<std::vector<char>> receivedBuffer, std::shared_ptr<udp::endpoint> remoteInfo) {
		const int rawType = atoi(&receivedBuffer->data()[0]);
		if (!isValidPacket(rawType)) {
			std::cout << "Couldnt read message\n";
			return;
		}

		const PacketType type = static_cast<PacketType>(rawType);

		switch (type) {
		case PacketType::CONNECT: {
			bool found = false;

			const auto remoteAddr = remoteInfo->address().to_string();
			const auto remotePort = remoteInfo->port();

			for (const Peer& peer : *p_peers) {
				const auto peerAddr = peer.socket.address().to_string();
				const auto peerPort = peer.socket.port();


				if (peerAddr != remoteAddr && peerPort != remotePort) {
					continue;
				};

				found = true;
				break;
			};

			if (found) {
				break;
			};

			Peer newPeer;
			newPeer.peerId = p_peerCount;
			newPeer.socket = *remoteInfo;
			newPeer.sessionToken = generateToken(p_peers);

			Packet packet;
			packet.type = PacketType::CONNECT;
			packet.peerIds = { newPeer.peerId };

			BufferVector buffer;
			buffer.addElement(newPeer.sessionToken);

			packet.buffer = buffer;

			p_peers->push_back(newPeer);
			p_peerCount++;

			sendMessage(packet);
			break;
		}

		case PacketType::DISCONNECT: {
			std::cout << "To be added\n";
			break;
		}

		case PacketType::MESSAGE: {
			break;
		}
		};
	};

	awaitable<void> receiveMessage() {
		try {
			auto receivedBuffer = std::make_unique<std::vector<char>>();
			auto remoteInfo = std::make_shared<udp::endpoint>();

			receivedBuffer->reserve(MAX_PAYLOAD_SIZE + 8);

			co_await p_serverSocket.async_receive_from(boost::asio::buffer(receivedBuffer->data(), receivedBuffer->size()), *remoteInfo);
			if (receivedBuffer->size() > MAX_PAYLOAD_SIZE + 8) {

			}

			handleMessage(std::move(receivedBuffer), remoteInfo);
		}
		catch (std::exception& err) {
			std::cout << "Error on receiving message: " << err.what() << "\n";
		}

		receiveMessage();
	};

public:
	UDPServer(const int& PORT, const size_t& MAX_PAYLOAD_SIZE = 2048) : p_serverSocket(p_iocontext), p_peerCount(0), MAX_PAYLOAD_SIZE(MAX_PAYLOAD_SIZE) {
		p_serverSocket = udp::socket(p_iocontext);

		p_serverSocket.open(udp::v4());
		p_serverSocket.bind(
			udp::endpoint(boost::asio::ip::make_address("127.0.0.1"), PORT));

		co_spawn(p_iocontext, receiveMessage(), boost::asio::detached);
	};

	~UDPServer() {
		p_iocontext.stop();
		p_serverSocket.close();
	};

	void start() {
		std::cout << "Server started!\n";
		p_iocontext.run();
	};

	awaitable<void> sendMessage(Packet& packet) {
		if (packet.buffer.getBuffer().size() > MAX_PAYLOAD_SIZE) {
			std::cout << "Buffer size limit exceeded!\n";
			co_return;
		}

		std::vector<char> packetBuffer = packet.buffer.getBuffer();
		auto buffer = std::make_shared<std::vector<char>>();
		buffer->reserve(2048);
		buffer->push_back(static_cast<char>(packet.type));
		buffer->push_back('\0');
		buffer->insert(buffer->end(), packetBuffer.begin(), packetBuffer.end());

		// Send to every peer if peerids vector is empty
		if (packet.peerIds.empty()) {
			for (Peer& peer : *p_peers) {
				send(buffer, peer.socket);
			};

			co_return;
		}

		for (uint32_t i : packet.peerIds) {
			for (Peer& peer : *p_peers) {
				if (peer.peerId != i) {
					continue;
				};

				send(buffer, peer.socket);
				break;
			}
		}
	}
	void on(const Event& event, const std::function<void>& function);
};