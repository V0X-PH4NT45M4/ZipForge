#include <core-algorithms.hpp>
#include <cstdint>
#include <memory>

namespace huffman {

	Node::Node () : freq(0), ch(-1), left(-1), right(-1) {}
	Node::Node (int32_t fr, int32_t c) : freq(fr), ch(c), left(-1), right(-1) {}
	Node::Node (int32_t fr, ssize_t l, ssize_t r) : freq(fr), ch(-1), left(l), right(l) {}
	bool Node::operator<(Node& other) {
		if (freq == other.freq) return ch < other.ch; 
		return freq < other.freq; 
	}

	ssize_t generateHuffmanTree(std::array<Node, (NUM_SYMBOLS << 1) - 1> &nodes) {
		// the input is a sorted array and with 2 indices bestLeafNode, bestInternalNode 
		// bestLeafNode is running across to find candidates in the set of 0...NUM_SYMBOLS-1 
		// bestInternalNode is running across to find the current internal node 

		int bestLeafNode = 0; 
		int bestInternalNode = NUM_SYMBOLS; 
		int nextFillIndex = NUM_SYMBOLS; 
		
		while (nodes[bestLeafNode].freq == 0) bestLeafNode++; 
		int32_t sum = 0; 
		for (int i = bestLeafNode; i < NUM_SYMBOLS; i++) {
			sum += nodes[i].freq;
		}
	
		auto mergeNodes = [&] (ssize_t left, ssize_t right) -> void {
			nodes[nextFillIndex].freq = nodes[left].freq + nodes[right].freq; 
			nodes[nextFillIndex].left = left; 
			nodes[nextFillIndex].right = right; 
			nextFillIndex++; 
		};

		if (bestLeafNode + 1 == NUM_SYMBOLS) {
			// there is only one node
			nodes[bestLeafNode + 1].left = nodes[bestLeafNode + 1].right = bestLeafNode; 
			return bestLeafNode; 
		}

		mergeNodes(bestLeafNode, bestLeafNode + 1); bestLeafNode += 2;  

		while (nodes[nextFillIndex - 1].freq != sum) {
			// which two nodes are we going to merge? 
			// we will either merge the bestLeafNode and the next bestLeafNode 
			// or we merge the bestLeafNode and bestInternalNode 
			// or we merge the bestInternalNode and next bestInternalNode 
			
			if (bestLeafNode >= NUM_SYMBOLS) {
				// we only have internal nodes to compare 
				mergeNodes(bestInternalNode, bestInternalNode + 1); 
				bestInternalNode += 2; 
			} else if (nodes[bestInternalNode].freq > nodes[bestLeafNode].freq) {
				// bestLeafNode is the best! we compare with next bestLeafNode and bestInternalNode 
				if (bestLeafNode + 1 < NUM_SYMBOLS && nodes[bestLeafNode + 1].freq < nodes[bestInternalNode].freq) { 
					mergeNodes(bestLeafNode, bestLeafNode + 1); 
					bestLeafNode += 2; // next best available leaf node 
				} else {
					mergeNodes(bestLeafNode, bestInternalNode);
					bestLeafNode += 1; 
					bestInternalNode += 1; 					
				}
			} else {
				// bestInternalNode is the best!, we compare with next bestINternalNode and bestLeafNode 
				if (bestInternalNode + 1 < nextFillIndex && nodes[bestInternalNode + 1].freq < nodes[bestLeafNode].freq) {
					mergeNodes(bestInternalNode, bestInternalNode + 1); 
					bestInternalNode += 2; 
				} else {
					mergeNodes(bestLeafNode, bestInternalNode); 
					bestLeafNode += 1; 
					bestInternalNode += 1; 					
				}
			}
		}

		

		return nextFillIndex - 1; 
	}

	
	void enumerateHuffmanCodes (
			std::array<Node, (NUM_SYMBOLS << 1) - 1>&nodes, 
			Node& node, 
			std::array<Code, NUM_SYMBOLS>&codes, 
			int code, 
			ssize_t depth
	) {
		if (node.ch == -1) {
			// internal node, we need to recursively move down 
			enumerateHuffmanCodes(nodes, nodes[node.left], codes, code * 2, depth + 1);
			enumerateHuffmanCodes(nodes, nodes[node.right], codes, code * 2 + 1, depth + 1); 
		} else {
			codes[node.ch] = {.bitSize = depth, .integerRepr = code}; 
			//std::cout << static_cast<char>(node.ch) << " code: " << code << " bitSize: " << depth << ' ' << node.freq << '\n'; 
		}
	}
	
	// returns the wasted amount in last byte packed into output
	void writeBytes (std::array<Code, NUM_SYMBOLS>&codes, std::ifstream&input, std::ofstream&output) {
		uint8_t byte = 0; // we will pack the bits into this 
		ssize_t currentSize = CHAR_BIT; // how many bits are left  
		
       		char currentChar; 
		Code currentCode; 
		
		int totalBytesWritten = 0; 

		while (input.get(currentChar)) {
			// this is the current code 
			currentCode = codes[currentChar]; 
			
			// size of the current code will be 
			ssize_t size = currentCode.bitSize; 
			if (size > currentSize) {
				// calculate remainingsize which will be added to next byte, and 

				auto remainingSize = size - currentSize; 
				auto remainingSizeMask = (1 << remainingSize) - 1; 
				auto remainingBits = currentCode.integerRepr & remainingSizeMask;
			        auto bitsToBePacked = (currentCode.integerRepr - remainingBits) >> remainingSize; 

				byte |= bitsToBePacked; 
				output.put(byte);
				totalBytesWritten++; 	
				
				currentSize = CHAR_BIT - remainingSize; 
				byte = remainingBits << (currentSize);	
			} else {
				currentSize -= size; 
				auto shiftedCode = currentCode.integerRepr << currentSize; 
				byte |= shiftedCode;

				if (currentSize == 0) {
					output.put(byte);
				        totalBytesWritten++; 	
					byte = 0; 
				        currentSize = CHAR_BIT; 	
				}				
			}		
		}

		

		if (currentSize != CHAR_BIT) {output.put(byte); totalBytesWritten++;}
		std::cout << "total bytes written: " << totalBytesWritten << '\n';
	}

	bool compressor(std::string& input, std::string& output) {
		std::ifstream inputFile(input); 
		std::ofstream outputFile(output, std::ios::binary); 
		
		std::array<Node, NUM_SYMBOLS * 2 - 1> nodes; 
		std::array<Code, NUM_SYMBOLS> codes; 
		for (size_t i = 0; i < NUM_SYMBOLS; i++) nodes[i].ch = i;  
		
		char ch; 
		while (inputFile.get(ch)) {
			nodes[static_cast<ssize_t>(ch)].freq++;
		} 		
		std::sort(nodes.begin(), nodes.begin() + NUM_SYMBOLS); 
		auto rootLoc = generateHuffmanTree(nodes);

		if (rootLoc == NUM_SYMBOLS - 1) {
			codes[nodes[rootLoc].ch].bitSize = 1; 
			codes[nodes[rootLoc].ch].integerRepr = 1; 
		} else {
			enumerateHuffmanCodes(nodes, nodes[rootLoc], codes, 0, 0); 
		}


		// write the huffman tree (size then the set of nodes into )
		ssize_t start = 0; 
		while (nodes[start].freq == 0) start++; 
		auto begin = nodes.begin() + start; 
		auto end = nodes.begin() + rootLoc + 1;
		auto numBytes = std::distance(begin, end) * sizeof(Node); 
		

		int totalBits = 0; 


		// calculate the wasted bits by summing over size * frequency of all characters appearing 
		for (ssize_t i = start; i < NUM_SYMBOLS; i++) {
			totalBits += nodes[i].freq * codes[nodes[i].ch].bitSize;	
		}		
		
		// if for example we have 273 bits that would correspond 8 - 273 % 8 = 8 - 1 = 7 
		size_t wasted = CHAR_BIT - (totalBits) % CHAR_BIT; 
		outputFile.write(reinterpret_cast<const char*>(&wasted), sizeof(size_t)); 

		size_t size = numBytes;
    		outputFile.write(reinterpret_cast<const char*>(&size), sizeof(size_t));


		const char* startPtr = reinterpret_cast<const char*>(std::addressof(*begin));
		outputFile.write(startPtr, numBytes); 

		// outputFile.write(reinterpret_cast<char*>(nodes.data()), sizeof(Node) * nodes.size()); 	
		inputFile.close(); // reset the input file stream for streaming to output file
		inputFile.open(input); 

		// now we need to write to output file by packing character by character into 
		// we also get the number of bits wasted in the last byte packed into output
		writeBytes(codes, inputFile, outputFile); 
		

		return true; 
	}
}
