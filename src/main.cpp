#include <core-algorithms.hpp>

int main (int argc, char* argv[]) {
	std::string input = "../tests/test1/input.txt"; 
	std::string output = "../tests/test1/output.txt";
        std::string decode = "../tests/test1/decoded.txt"; 	
	huffman::compressor(input, output); 
	std::cout << "wow wello from wain\n"; 
	huffman::decompressor(output, decode); 
	
	std::cout << "hello from main\n"; 


	return 0; 	
}
