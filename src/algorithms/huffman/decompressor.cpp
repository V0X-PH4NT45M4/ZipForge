#include <cstdint>
#include <string> 
#include <cstddef> 
#include <fstream> 
#include <core-algorithms.hpp>

namespace huffman {

	void decompressor (std::string&input, std::string&output) {
		// we want to decompress so first we need to set up the file streams 
		std::ifstream inputFile(input, std::ios::binary); 
		std::ofstream outputFile(output);
		// get the number of wasted bits and then the number of bytes the huffman tree spans 
		// then extract the huffman tree, and lets just print out the tree and reuse enumerateHuffmanCodes to get 
		// a testing of whether we get the correct tree 
		size_t wasted, numBytes; 
	        inputFile.read(reinterpret_cast<char*>(&wasted), sizeof(size_t)); 
		inputFile.read(reinterpret_cast<char*>(&numBytes), sizeof(size_t)); 
		std::vector<Node> huffmanTree(numBytes / sizeof(Node));
		inputFile.read(reinterpret_cast<char*>(huffmanTree.data()), numBytes); 

	        //	for (auto&node: huffmanTree) {
	        //		std::cout << node.ch << ' ' << node.freq << ' ' << node.left << ' ' << node.right << '\n'; 
	        //	}

		int numNodes = huffmanTree.size(); 
		int leafNodes = (numNodes + 1) / 2; // numNodes = 2 * leafNodes - 1 for a full binary tree (huffman tree is full)
		int offset = huffmanTree[leafNodes].left; // we have to subtract the offset to get the child's index in this vector

		char ch; 
		ssize_t state = huffmanTree.size() - 1; 
		int counter = 0; 
		while (inputFile.get(ch)) {
			counter++; 
			// we will read this and use the state machine that is the huffmanTree
			uint8_t mask = (1 << (CHAR_BIT - 1));
			for (int i = 0; i < CHAR_BIT - wasted * (inputFile.peek() == EOF); i++) {
				// 0 is left and 1 is right
				bool left = ((static_cast<uint8_t>(ch) & mask) == 0); 
				
				if (left) {
					state = huffmanTree[state].left - offset; 
				} else {
					state = huffmanTree[state].right - offset; 
				}

				if (huffmanTree[state].left == -1) {
					outputFile.put(char(huffmanTree[state].ch)); 
					state = huffmanTree.size() - 1; 
				}

				mask >>= 1; // right shift so that we read from right to left 
			}
		}
		std::cout << "\nread bytes: " << counter << '\n'; 	
	}
}
