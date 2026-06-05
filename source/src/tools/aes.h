#pragma once
#ifndef CG_AES_H
#define CG_AES_H

#include <vector>
#include <stdexcept>
#include <cstring>
#include <iostream>
#include <iomanip>

class AES
{
	enum class MODE
	{
		ECB,
		CBC,
		CFB
	};

	static const int BLOCK_SIZE = 16;
	static const int NK_128 = 4;
	static const int NK_192 = 6;
	static const int NK_256 = 8;
	static const int NR_128 = 10;
	static const int NR_192 = 12;
	static const int NR_256 = 14;

	inline static const unsigned char RCON[11] = { 0x8d, 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1b, 0x36 };
	inline static const unsigned char SBOX[256] = {
		0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
		0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
		0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
		0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
		0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
		0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
		0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
		0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
		0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
		0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
		0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
		0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
		0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
		0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
		0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
		0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16
	};
	inline static const unsigned char INV_SBOX[256] = {
		0x52, 0x09, 0x6a, 0xd5, 0x30, 0x36, 0xa5, 0x38, 0xbf, 0x40, 0xa3, 0x9e, 0x81, 0xf3, 0xd7, 0xfb,
		0x7c, 0xe3, 0x39, 0x82, 0x9b, 0x2f, 0xff, 0x87, 0x34, 0x8e, 0x43, 0x44, 0xc4, 0xde, 0xe9, 0xcb,
		0x54, 0x7b, 0x94, 0x32, 0xa6, 0xc2, 0x23, 0x3d, 0xee, 0x4c, 0x95, 0x0b, 0x42, 0xfa, 0xc3, 0x4e,
		0x08, 0x2e, 0xa1, 0x66, 0x28, 0xd9, 0x24, 0xb2, 0x76, 0x5b, 0xa2, 0x49, 0x6d, 0x8b, 0xd1, 0x25,
		0x72, 0xf8, 0xf6, 0x64, 0x86, 0x68, 0x98, 0x16, 0xd4, 0xa4, 0x5c, 0xcc, 0x5d, 0x65, 0xb6, 0x92,
		0x6c, 0x70, 0x48, 0x50, 0xfd, 0xed, 0xb9, 0xda, 0x5e, 0x15, 0x46, 0x57, 0xa7, 0x8d, 0x9d, 0x84,
		0x90, 0xd8, 0xab, 0x00, 0x8c, 0xbc, 0xd3, 0x0a, 0xf7, 0xe4, 0x58, 0x05, 0xb8, 0xb3, 0x45, 0x06,
		0xd0, 0x2c, 0x1e, 0x8f, 0xca, 0x3f, 0x0f, 0x02, 0xc1, 0xaf, 0xbd, 0x03, 0x01, 0x13, 0x8a, 0x6b,
		0x3a, 0x91, 0x11, 0x41, 0x4f, 0x67, 0xdc, 0xea, 0x97, 0xf2, 0xcf, 0xce, 0xf0, 0xb4, 0xe6, 0x73,
		0x96, 0xac, 0x74, 0x22, 0xe7, 0xad, 0x35, 0x85, 0xe2, 0xf9, 0x37, 0xe8, 0x1c, 0x75, 0xdf, 0x6e,
		0x47, 0xf1, 0x1a, 0x71, 0x1d, 0x29, 0xc5, 0x89, 0x6f, 0xb7, 0x62, 0x0e, 0xaa, 0x18, 0xbe, 0x1b,
		0xfc, 0x56, 0x3e, 0x4b, 0xc6, 0xd2, 0x79, 0x20, 0x9a, 0xdb, 0xc0, 0xfe, 0x78, 0xcd, 0x5a, 0xf4,
		0x1f, 0xdd, 0xa8, 0x33, 0x88, 0x07, 0xc7, 0x31, 0xb1, 0x12, 0x10, 0x59, 0x27, 0x80, 0xec, 0x5f,
		0x60, 0x51, 0x7f, 0xa9, 0x19, 0xb5, 0x4a, 0x0d, 0x2d, 0xe5, 0x7a, 0x9f, 0x93, 0xc9, 0x9c, 0xef,
		0xa0, 0xe0, 0x3b, 0x4d, 0xae, 0x2a, 0xf5, 0xb0, 0xc8, 0xeb, 0xbb, 0x3c, 0x83, 0x53, 0x99, 0x61,
		0x17, 0x2b, 0x04, 0x7e, 0xba, 0x77, 0xd6, 0x26, 0xe1, 0x69, 0x14, 0x63, 0x55, 0x21, 0x0c, 0x7d
	};

	static void checkKeySize(const std::vector<unsigned char>& key)
	{
		if (key.size() != 16 && key.size() != 24 && key.size() != 32) throw std::invalid_argument("Invalid key size. Key must be 16, 24, or 32 bytes.");
	}
	static void checkDataSize(const std::vector<unsigned char>& data, MODE mode)
	{
		if (mode == MODE::ECB || mode == MODE::CBC)
		{
			if (data.size() % BLOCK_SIZE != 0) throw std::invalid_argument("Data length must be multiple of 16 bytes for ECB or CBC mode.");
		}
	}
	static int getKeyLengthType(size_t key_len)
	{
		if (key_len == 16) return AES_128;
		if (key_len == 24) return AES_192;
		if (key_len == 32) return AES_256;
		throw std::invalid_argument("Invalid key length");
	}
	static int getNr(int key_type)
	{
		switch (key_type)
		{
		case AES_128: return NR_128;
		case AES_192: return NR_192;
		case AES_256: return NR_256;
		default: throw std::invalid_argument("Invalid key type");
		}
	}
	static int getNk(int key_type)
	{
		switch (key_type) {
		case AES_128: return NK_128;
		case AES_192: return NK_192;
		case AES_256: return NK_256;
		default: throw std::invalid_argument("Invalid key type");
		}
	}
	static void subBytes(unsigned char state[4][4])
	{
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) state[i][j] = SBOX[state[i][j]];
	}
	static void invSubBytes(unsigned char state[4][4])
	{
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) state[i][j] = INV_SBOX[state[i][j]];
	}
	static void shiftRows(unsigned char state[4][4])
	{
		unsigned char temp;
		temp = state[1][0];
		state[1][0] = state[1][1];
		state[1][1] = state[1][2];
		state[1][2] = state[1][3];
		state[1][3] = temp;
		temp = state[2][0];
		state[2][0] = state[2][2];
		state[2][2] = temp;
		temp = state[2][1];
		state[2][1] = state[2][3];
		state[2][3] = temp;
		temp = state[3][3];
		state[3][3] = state[3][2];
		state[3][2] = state[3][1];
		state[3][1] = state[3][0];
		state[3][0] = temp;
	}
	static void invShiftRows(unsigned char state[4][4])
	{
		unsigned char temp;
		temp = state[1][3];
		state[1][3] = state[1][2];
		state[1][2] = state[1][1];
		state[1][1] = state[1][0];
		state[1][0] = temp;
		temp = state[2][0];
		state[2][0] = state[2][2];
		state[2][2] = temp;
		temp = state[2][1];
		state[2][1] = state[2][3];
		state[2][3] = temp;
		temp = state[3][0];
		state[3][0] = state[3][1];
		state[3][1] = state[3][2];
		state[3][2] = state[3][3];
		state[3][3] = temp;
	}
	static unsigned char gfMultiply(unsigned char a, unsigned char b)
	{
		unsigned char result = 0;
		unsigned char high_bit;
		for (int i = 0; i < 8; i++) {
			if (b & 1) result ^= a;
			high_bit = (a & 0x80);
			a <<= 1;
			if (high_bit) a ^= 0x1B;
			b >>= 1;
		}
		return result;
	}
	static void mixColumns(unsigned char state[4][4])
	{
		unsigned char temp[4];
		for (int i = 0; i < 4; i++) {
			temp[0] = gfMultiply(0x02, state[0][i]) ^ gfMultiply(0x03, state[1][i]) ^ state[2][i] ^ state[3][i];
			temp[1] = state[0][i] ^ gfMultiply(0x02, state[1][i]) ^ gfMultiply(0x03, state[2][i]) ^ state[3][i];
			temp[2] = state[0][i] ^ state[1][i] ^ gfMultiply(0x02, state[2][i]) ^ gfMultiply(0x03, state[3][i]);
			temp[3] = gfMultiply(0x03, state[0][i]) ^ state[1][i] ^ state[2][i] ^ gfMultiply(0x02, state[3][i]);
			state[0][i] = temp[0];
			state[1][i] = temp[1];
			state[2][i] = temp[2];
			state[3][i] = temp[3];
		}
	}
	static void invMixColumns(unsigned char state[4][4])
	{
		unsigned char temp[4];
		for (int i = 0; i < 4; i++) {
			temp[0] = gfMultiply(0x0e, state[0][i]) ^ gfMultiply(0x0b, state[1][i]) ^ gfMultiply(0x0d, state[2][i]) ^ gfMultiply(0x09, state[3][i]);
			temp[1] = gfMultiply(0x09, state[0][i]) ^ gfMultiply(0x0e, state[1][i]) ^ gfMultiply(0x0b, state[2][i]) ^ gfMultiply(0x0d, state[3][i]);
			temp[2] = gfMultiply(0x0d, state[0][i]) ^ gfMultiply(0x09, state[1][i]) ^ gfMultiply(0x0e, state[2][i]) ^ gfMultiply(0x0b, state[3][i]);
			temp[3] = gfMultiply(0x0b, state[0][i]) ^ gfMultiply(0x0d, state[1][i]) ^ gfMultiply(0x09, state[2][i]) ^ gfMultiply(0x0e, state[3][i]);
			state[0][i] = temp[0];
			state[1][i] = temp[1];
			state[2][i] = temp[2];
			state[3][i] = temp[3];
		}
	}
	static void addRoundKey(unsigned char state[4][4], const unsigned char* roundKey)
	{
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) state[j][i] ^= roundKey[i * 4 + j];
	}
	static void keyExpansion(const unsigned char* key, unsigned char* roundKeys, int key_type)
	{
		int nk = getNk(key_type);
		int nr = getNr(key_type);
		for (int i = 0; i < nk * 4; i++) roundKeys[i] = key[i];
		for (int i = nk; i < 4 * (nr + 1); i++) {
			unsigned char temp[4];
			for (int j = 0; j < 4; j++) temp[j] = roundKeys[(i - 1) * 4 + j];

			if (i % nk == 0) {
				unsigned char t = temp[0];
				temp[0] = temp[1];
				temp[1] = temp[2];
				temp[2] = temp[3];
				temp[3] = t;
				for (int j = 0; j < 4; j++) temp[j] = SBOX[temp[j]];
				if (i / nk <= 10) temp[0] ^= RCON[i / nk];
			}
			else if (nk > 6 && i % nk == 4) {
				for (int j = 0; j < 4; j++) temp[j] = SBOX[temp[j]];
			}
			for (int j = 0; j < 4; j++) roundKeys[i * 4 + j] = roundKeys[(i - nk) * 4 + j] ^ temp[j];
		}
	}
	static void encryptBlock(const unsigned char* in, unsigned char* out, const unsigned char* roundKeys, int nr)
	{
		unsigned char state[4][4];
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) state[j][i] = in[i * 4 + j];
		addRoundKey(state, roundKeys);
		for (int round = 1; round < nr; round++) {
			subBytes(state);
			shiftRows(state);
			mixColumns(state);
			addRoundKey(state, roundKeys + round * 16);
		}
		subBytes(state);
		shiftRows(state);
		addRoundKey(state, roundKeys + nr * 16);
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) out[i * 4 + j] = state[j][i];
	}
	static void decryptBlock(const unsigned char* in, unsigned char* out, const unsigned char* roundKeys, int nr)
	{
		unsigned char state[4][4];
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) state[j][i] = in[i * 4 + j];
		addRoundKey(state, roundKeys + nr * 16);
		for (int round = nr - 1; round > 0; round--) {
			invShiftRows(state);
			invSubBytes(state);
			addRoundKey(state, roundKeys + round * 16);
			invMixColumns(state);
		}
		invShiftRows(state);
		invSubBytes(state);
		addRoundKey(state, roundKeys);
		for (int i = 0; i < 4; i++) for (int j = 0; j < 4; j++) out[i * 4 + j] = state[j][i];
	}
	static void xorBlocks(const unsigned char* a, const unsigned char* b, unsigned char* result)
	{
		for (int i = 0; i < BLOCK_SIZE; i++) result[i] = a[i] ^ b[i];
	}

	static std::vector<unsigned char> pkcs7_pad(const std::vector<unsigned char>& data, size_t block_size = BLOCK_SIZE) {
		if (block_size == 0 || block_size > 255) throw std::invalid_argument("Invalid block size for PKCS#7 padding");
		size_t padding_len = block_size - (data.size() % block_size);
		std::vector<unsigned char> padded = data;
		padded.insert(padded.end(), padding_len, static_cast<unsigned char>(padding_len));
		return padded;
	}

	static std::vector<unsigned char> pkcs7_unpad(const std::vector<unsigned char>& padded_data, size_t block_size = BLOCK_SIZE) {
		if (padded_data.empty()) throw std::invalid_argument("Empty data cannot be unpadded");
		size_t padding_len = padded_data.back();
		if (padding_len == 0 || padding_len > block_size || padding_len > padded_data.size()) throw std::invalid_argument("Invalid PKCS#7 padding");
		for (size_t i = padded_data.size() - padding_len; i < padded_data.size(); i++) if (padded_data[i] != padding_len) throw std::invalid_argument("Invalid PKCS#7 padding");
		return std::vector<unsigned char>(padded_data.begin(), padded_data.end() - padding_len);
	}

public:
	enum
	{
		AES_128,
		AES_192,
		AES_256
	};

	static std::vector<unsigned char> make_iv(size_t len = 16) {
		std::vector<unsigned char> iv;
		iv.resize(len);
		srand(time(nullptr) + clock());
		for (size_t i = 0; i < len; i++) iv[i] = static_cast<unsigned char>(rand() % 256);
		return iv;
	}

	static std::vector<unsigned char> en_ecb(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key) {
		checkKeySize(key);
		checkDataSize(data, MODE::ECB);

		int key_type = getKeyLengthType(key.size());
		int nr = getNr(key_type);

		std::vector<unsigned char> roundKeys(16 * (nr + 1));
		keyExpansion(key.data(), roundKeys.data(), key_type);

		std::vector<unsigned char> result(data.size());
		for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) encryptBlock(data.data() + i, result.data() + i, roundKeys.data(), nr);

		return result;
	}
	static std::vector<unsigned char> en_ecb(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len) {
		return en_ecb(std::vector<unsigned char>(data, data + data_len), std::vector<unsigned char>(key, key + key_len));
	}
	static std::vector<unsigned char> en_ecb_pkcs7(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key) {
		auto padded = pkcs7_pad(data);
		return en_ecb(padded, key);
	}
	static std::vector<unsigned char> de_ecb(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key) {
		checkKeySize(key);
		checkDataSize(data, MODE::ECB);

		int key_type = getKeyLengthType(key.size());
		int nr = getNr(key_type);

		std::vector<unsigned char> roundKeys(16 * (nr + 1));
		keyExpansion(key.data(), roundKeys.data(), key_type);

		std::vector<unsigned char> result(data.size());
		for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) decryptBlock(data.data() + i, result.data() + i, roundKeys.data(), nr);

		return result;
	}
	static std::vector<unsigned char> de_ecb(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len) {
		return de_ecb(std::vector<unsigned char>(data, data + data_len), std::vector<unsigned char>(key, key + key_len));
	}
	static std::vector<unsigned char> de_ecb_pkcs7(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key) {
		auto decrypted = de_ecb(ciphertext, key);
		return pkcs7_unpad(decrypted);
	}

	static std::vector<unsigned char> en_cbc(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv = {}) {
		checkKeySize(key);
		checkDataSize(data, MODE::CBC);

		int key_type = getKeyLengthType(key.size());
		int nr = getNr(key_type);

		std::vector<unsigned char> roundKeys(16 * (nr + 1));
		keyExpansion(key.data(), roundKeys.data(), key_type);

		std::vector<unsigned char> iv_vec(iv);
		if (iv_vec.empty()) iv_vec.resize(BLOCK_SIZE, 0);
		else if (iv_vec.size() < BLOCK_SIZE) iv_vec.resize(BLOCK_SIZE, 0);

		std::vector<unsigned char> result(data.size());
		unsigned char previous[BLOCK_SIZE];
		unsigned char current[BLOCK_SIZE];

		memcpy(previous, iv_vec.data(), BLOCK_SIZE);

		for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) {
			xorBlocks(data.data() + i, previous, current);
			encryptBlock(current, result.data() + i, roundKeys.data(), nr);
			memcpy(previous, result.data() + i, BLOCK_SIZE);
		}

		return result;
	}
	static std::vector<unsigned char> en_cbc(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len, const unsigned char* iv = nullptr, size_t iv_len = 0) {
		return en_cbc(std::vector<unsigned char>(data, data + data_len), std::vector<unsigned char>(key, key + key_len), iv ? std::vector<unsigned char>(iv, iv + iv_len) : std::vector<unsigned char>());
	}
	static std::vector<unsigned char> en_cbc_pkcs7(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv = {}) {
		auto padded = pkcs7_pad(data);
		return en_cbc(padded, key, iv);
	}
	static std::vector<unsigned char> de_cbc(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv = {}) {
		checkKeySize(key);
		checkDataSize(data, MODE::CBC);

		int key_type = getKeyLengthType(key.size());
		int nr = getNr(key_type);

		std::vector<unsigned char> roundKeys(16 * (nr + 1));
		keyExpansion(key.data(), roundKeys.data(), key_type);

		std::vector<unsigned char> iv_vec(iv);
		if (iv_vec.empty()) iv_vec.resize(BLOCK_SIZE, 0);
		else if (iv_vec.size() < BLOCK_SIZE) iv_vec.resize(BLOCK_SIZE, 0);

		std::vector<unsigned char> result(data.size());
		unsigned char previous[BLOCK_SIZE];
		unsigned char current[BLOCK_SIZE];

		memcpy(previous, iv_vec.data(), BLOCK_SIZE);

		for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) {
			decryptBlock(data.data() + i, current, roundKeys.data(), nr);
			xorBlocks(current, previous, result.data() + i);
			memcpy(previous, data.data() + i, BLOCK_SIZE);
		}

		return result;
	}
	static std::vector<unsigned char> de_cbc(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len, const unsigned char* iv = nullptr, size_t iv_len = 0) {
		return de_cbc(std::vector<unsigned char>(data, data + data_len), std::vector<unsigned char>(key, key + key_len), iv ? std::vector<unsigned char>(iv, iv + iv_len) : std::vector<unsigned char>());
	}
	static std::vector<unsigned char> de_cbc_pkcs7(const std::vector<unsigned char>& ciphertext, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv = {}) {
		auto decrypted = de_cbc(ciphertext, key, iv);
		return pkcs7_unpad(decrypted);
	}

	static std::vector<unsigned char> en_cfb(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv = {}) {
		checkKeySize(key);

		int key_type = getKeyLengthType(key.size());
		int nr = getNr(key_type);

		std::vector<unsigned char> roundKeys(16 * (nr + 1));
		keyExpansion(key.data(), roundKeys.data(), key_type);

		std::vector<unsigned char> iv_vec(iv);
		if (iv_vec.empty()) iv_vec.resize(BLOCK_SIZE, 0);
		else if (iv_vec.size() < BLOCK_SIZE) iv_vec.resize(BLOCK_SIZE, 0);

		std::vector<unsigned char> result(data.size());
		unsigned char feedback[BLOCK_SIZE];

		memcpy(feedback, iv_vec.data(), BLOCK_SIZE);

		for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) {
			unsigned char encrypted[BLOCK_SIZE];
			encryptBlock(feedback, encrypted, roundKeys.data(), nr);

			size_t remaining = data.size() - i;
			size_t block_size = (remaining < BLOCK_SIZE) ? remaining : BLOCK_SIZE;

			for (size_t j = 0; j < block_size; j++) {
				result[i + j] = data[i + j] ^ encrypted[j];
				feedback[j] = result[i + j];
			}

			for (size_t j = block_size; j < BLOCK_SIZE; j++) feedback[j] = 0;
		}

		return result;
	}
	static std::vector<unsigned char> en_cfb(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len, const unsigned char* iv = nullptr, size_t iv_len = 0) {
		return en_cfb(std::vector<unsigned char>(data, data + data_len), std::vector<unsigned char>(key, key + key_len), iv ? std::vector<unsigned char>(iv, iv + iv_len) : std::vector<unsigned char>());
	}
	static std::vector<unsigned char> de_cfb(const std::vector<unsigned char>& data, const std::vector<unsigned char>& key, const std::vector<unsigned char>& iv = {}) {
		checkKeySize(key);

		int key_type = getKeyLengthType(key.size());
		int nr = getNr(key_type);

		std::vector<unsigned char> roundKeys(16 * (nr + 1));
		keyExpansion(key.data(), roundKeys.data(), key_type);

		std::vector<unsigned char> iv_vec(iv);
		if (iv_vec.empty()) iv_vec.resize(BLOCK_SIZE, 0);
		else if (iv_vec.size() < BLOCK_SIZE) iv_vec.resize(BLOCK_SIZE, 0);

		std::vector<unsigned char> result(data.size());
		unsigned char feedback[BLOCK_SIZE];

		memcpy(feedback, iv_vec.data(), BLOCK_SIZE);

		for (size_t i = 0; i < data.size(); i += BLOCK_SIZE) {
			unsigned char encrypted[BLOCK_SIZE];
			encryptBlock(feedback, encrypted, roundKeys.data(), nr);

			size_t remaining = data.size() - i;
			size_t block_size = (remaining < BLOCK_SIZE) ? remaining : BLOCK_SIZE;

			for (size_t j = 0; j < block_size; j++) {
				result[i + j] = data[i + j] ^ encrypted[j];
				feedback[j] = data[i + j];
			}

			for (size_t j = block_size; j < BLOCK_SIZE; j++) feedback[j] = 0;
		}

		return result;
	}
	static std::vector<unsigned char> de_cfb(const unsigned char* data, size_t data_len, const unsigned char* key, size_t key_len, const unsigned char* iv = nullptr, size_t iv_len = 0) {
		return de_cfb(std::vector<unsigned char>(data, data + data_len), std::vector<unsigned char>(key, key + key_len), iv ? std::vector<unsigned char>(iv, iv + iv_len) : std::vector<unsigned char>());
	}
};
#endif