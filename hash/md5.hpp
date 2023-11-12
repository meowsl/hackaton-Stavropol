#pragma once
#include <string>
#include <vector>
#include <fstream>

class MD5 {
	typedef uint32_t(*Round_function)(uint32_t, uint32_t, uint32_t);
	static const uint32_t shift_table[4][4];
	//Sine table	
	static const uint32_t* sin_table();
	static const uint32_t* create_table();	
	static const Round_function round_functions[4];
	
	inline void change_endianness(char* val);
	void encode(char* message, size_t size);
	void pad_input(std::vector<char>& input);
	void compute_hash(char* message, size_t size);
	void process_block(char* block);
	uint32_t A = 0x67452301;
	uint32_t B = 0xEFCDAB89;
	uint32_t C = 0x98BADCFE;
	uint32_t D = 0x10325476;
public:
	MD5(std::string input);
	MD5(std::ifstream& file);
	MD5(const MD5& other);
	MD5& operator=(const MD5& other);
	MD5& operator=(MD5&& other);
	bool operator==(const MD5& other);
	bool operator!=(const MD5& other);
	std::string get_string_hash();
};






