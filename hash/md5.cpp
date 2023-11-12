#include "md5.hpp"
#include <sstream>
#include <iomanip>
#include <cstring>

bool determine_endianness() {
	int32_t val = 1;
	return *reinterpret_cast<char*>(&val);
}

const bool LITTLE_ENDIAN = determine_endianness();

uint32_t F(uint32_t X, uint32_t Y, uint32_t Z) {
	return	(X & Y) | ((~X) & Z);
}

uint32_t G(uint32_t X, uint32_t Y, uint32_t Z) {
	return (X & Z) | (Y & (~Z));
}

uint32_t H(uint32_t X, uint32_t Y, uint32_t Z) {
	return X ^ Y ^ Z;
}

uint32_t I(uint32_t X, uint32_t Y, uint32_t Z) {
	return Y ^ (X | (~Z));
}

template<typename T>
inline void rotate_arguments_right(T& A, T& B, T& C, T& D) {
	std::swap(A, D);
	std::swap(D, C);
	std::swap(C, B);
}

const uint32_t MD5::shift_table[4][4] = {
	{7, 12, 17, 22},
	{5, 9,  14, 20},
	{4, 11, 16, 23},
	{6, 10, 15, 21} 
};

const MD5::Round_function MD5::round_functions[4] = { F,G,H,I };

MD5::MD5(std::string _input) {
	std::vector<char> input;
	
	for (char x : _input) {
		input.push_back(x);
	}

	pad_input(input);

	compute_hash(input.data(), input.size());
}

MD5::MD5(std::ifstream& file) {
	if (!file.is_open()) {
		throw std::runtime_error("Invalid file.");
	}

	file.seekg(0, std::ios::end);
	std::size_t input_size = file.tellg();	

	std::vector<char>input(input_size, 0);

	file.seekg(0, std::ios::beg);
	
	file.read(input.data(), input_size);
	
	pad_input(input);
	compute_hash(input.data(), input.size());
}

void MD5::pad_input(std::vector<char>& input) {

	uint64_t original_size = static_cast<uint64_t>(input.size());

	//Inserts padding
	input.push_back(0x80);
	do {
		input.push_back(0);
	} while (input.size() % 56);
	do {
		input.push_back(0);

	} while (input.size() % 64);

	
	uint64_t size = original_size * 8;
	void* size_section = reinterpret_cast<uint64_t*>(input.data() + input.size()) - 1;
	memcpy(size_section, &size, sizeof(uint64_t));
}

void MD5::compute_hash(char* message, size_t size) {

	if (!LITTLE_ENDIAN)
		encode(message, size);

	for (size_t i = 0; i < size; i+= 64, message += 64) {
		process_block(message);
	}

	if (LITTLE_ENDIAN) {
		change_endianness((char*)&A);
		change_endianness((char*)&B);
		change_endianness((char*)&C);
		change_endianness((char*)&D);
	}
}

void MD5::process_block(char* block) {
	uint32_t* block_as_word = reinterpret_cast<uint32_t*>(block);
		
	auto OLD_A = A;
	auto OLD_B = B;
	auto OLD_C = C;
	auto OLD_D = D;

	for (int i = 0; i < 64; ++i) {
		int round = i / 16;
		uint32_t s = shift_table[round][i % 4];
		int k;

		switch (round) {
		case 0:
			k = i;
			break;
		case 1:
			k = (1 + 5 * (i - round * 16)) % 16;
			break;
		case 2:
			k = (5 + 3 * (i - round * 16)) % 16;
			break;
		case 3:
			k = (7 * (i - round * 16)) % 16;
			break;
		}
		
		A = A + round_functions[round](B, C, D) + block_as_word[k] + sin_table()[i];
		A = (A << s) | (A >> (32 - s));
		A += B;
		
		rotate_arguments_right(A, B, C, D);
	}

	A += OLD_A;
	B += OLD_B;
	C += OLD_C;
	D += OLD_D;
}

void MD5::encode(char* message, size_t size) {
	for (size_t i = 0; i != size; i += 4) {
		change_endianness(message + 1);
	}
}

inline void MD5::change_endianness(char* val) {
	//Bitwise "trick" to swap values
	val[0] ^= (val[3] ^= (val[0] ^= val[3]));
	val[1] ^= (val[2] ^= (val[1] ^= val[2]));
}

const uint32_t* MD5::create_table() {
	static uint32_t values[64];

	for (int i = 0; i < 64; ++i) {
		values[i] = static_cast<uint32_t>(std::abs(std::sin(i + 1) * 4294967296LL));
	}

	return values;
}

const uint32_t* MD5::sin_table() {
	static const uint32_t* table = create_table();
	return table;
}

std::string MD5::get_string_hash() {
	std::stringstream ss;
	ss << std::hex << std::setw(8) << std::setfill('0') << A << B << C << D;
	std::string ret_val;
	ss >> ret_val;
	return ret_val;
}

MD5::MD5(const MD5& other) {
	*this = other;
}

MD5& MD5::operator=(const MD5& other) {
	this->A = other.A;
	this->B = other.B;
	this->C = other.C;
	this->D = other.D;
	return *this;
}

MD5& MD5::operator=(MD5&& other) {
	if (this != &other) {
		this->A = other.A;
		this->B = other.B;
		this->C = other.C;
		this->D = other.D;
	}
	return *this;
}

bool MD5::operator==(const MD5& other) {
	return this->A == other.A && this->B == other.B && this->C == other.C && this->D == other.D;
}

bool MD5::operator!=(const MD5& other) {
	return !(*this == other);
}

