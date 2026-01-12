// File including helper functions
#pragma once


bool isValidPacket(int packetType) {
	if (packetType < 0 || packetType >5) return false;
	return true;
}