#include "pch.h"
#include <udpnet/udp_server.h>
#include "../udp_server.h"


void UDPServer::on(const Event& event, const std::function<void()>& function) {
	auto it = dispatchMap.find(event);
	if (it != dispatchMap.end()) {
		it->second.reserve(1);
		it->second.emplace_back(function);
		return;
	}

	std::vector<std::function<void()>> functionVector;
	functionVector.reserve(1);
	functionVector.emplace_back(function);

	dispatchMap.emplace(event, functionVector);
};