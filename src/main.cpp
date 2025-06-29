#include <core-algorithms.hpp>

int main (int argc, char* argv[]) {
	std::string input = "../tests/test2/input.txt"; 
	std::string output = "../tests/test2/output.txt";
        std::string decode = "../tests/test2/decoded.txt"; 	
	huffman::compressor(input, output); 
	std::cout << "wow wello from wain\n"; 
	huffman::decompressor(output, decode); 
	
	std::cout << "hello from main\n"; 


	return 0; 	
}
