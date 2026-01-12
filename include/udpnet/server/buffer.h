#pragma once

#include <cstring>
#include <string>
#include <vector>

class BufferVector {
private:
	std::vector<char> buffer;

public:
	BufferVector() = default;
	BufferVector(std::vector<char>& vector) {
		buffer.insert(buffer.end(), vector.begin(), vector.end());
	}

	const std::vector<char> getBuffer() const { return buffer; }

	void addElement(const std::vector<char>& vector) {
		buffer.insert(buffer.end(), vector.begin(), vector.end());
	}

	void addElement(const char* element) {
		size_t length = std::strlen(element);
		buffer.insert(buffer.end(), element, element + length);
		buffer.push_back('\0');
	}

	void addElement(const std::string& element) {
		buffer.insert(buffer.end(), element.begin(), element.end());
		buffer.push_back('\0');
	}
};
