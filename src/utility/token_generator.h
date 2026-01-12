#pragma once

#include <random>

#include <udpnet/server/peer.h>

typedef std::vector<char> char_array;

char_array chars() {
	return { 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N',
					'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', 'a', 'b',
					'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p',
					'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z', '1', '2', '3', '4',
					'5', '6', '7', '8', '9', '!', '^', '+', '/', '=', '?', '_', '*', '$' };
};

std::string generateToken(const std::unique_ptr<std::vector<Peer>>& peers) {
	while (true) {
		std::string token;
		token.resize(32);

		char_array randomChars = chars();

		std::random_device randomDevice;
		std::mt19937 mt(randomDevice());
		std::uniform_int_distribution<int> random(0, randomChars.size() - 1);

		for (int i = 0; i < 32; i++) {
			token[i] = randomChars[random(mt)];
		};

		bool conflict = false;
		for (const Peer& peer : *peers) {
			if (peer.sessionToken == token) {
				conflict = true;
				break;
			}
		}

		if (!conflict) {
			return token;
		};
	}
};
