#pragma once
#include <cstddef> 
#include <cstdint> 
#include <fstream> 
#include <iostream> 
#include <ios> 
#include <string> 
#include <algorithm> 
#include <array> 
#include <climits> 
#include <sys/types.h>
#include <vector> 
#define NUM_SYMBOLS 256

namespace huffman {
	struct Node {
		int32_t freq; 
		int32_t ch; 
		ssize_t left; 
		ssize_t right; 

		Node (); 
		Node (int32_t f, int32_t c); 
		Node (int32_t f, ssize_t l, ssize_t r); 
		bool operator<(Node& other); 
	}; 

	struct Code {
		ssize_t bitSize; 
		int32_t integerRepr; 
	};

	ssize_t generateHuffmanTree(std::array<Node, (NUM_SYMBOLS << 1) - 1>&nodes);
	void enumerateHuffmanCodes (std::array<Node, (NUM_SYMBOLS << 1) - 1>&NODES, std::array<Code, NUM_SYMBOLS>&codes, char code, ssize_t depth);
	void writeBytes (std::array<Code, NUM_SYMBOLS>&CODES, std::ifstream&input, std::ofstream&output); 	
	bool compressor (std::string&input, std::string&output); 
	void decompressor (std::string&input, std::string&output); 

}
