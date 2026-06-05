#pragma once
#ifndef CG_CODE_H
#define CG_CODE_H

#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
namespace Code
{
	class Hex
	{
	public:
		static std::vector<unsigned char> de(const std::string& hex_str)
		{
			std::vector<unsigned char> result;
			if (hex_str.empty()) return {};
			if (hex_str.length() % 2 != 0) return {};
			result.reserve(hex_str.length() / 2);
			for (size_t i = 0; i < hex_str.length(); i += 2)
			{
				char c1 = static_cast<char>(std::tolower(static_cast<unsigned char>(hex_str[i])));
				char c2 = static_cast<char>(std::tolower(static_cast<unsigned char>(hex_str[i + 1])));
				if (!std::isxdigit(static_cast<unsigned char>(c1)) || !std::isxdigit(static_cast<unsigned char>(c2))) return {};
				int high = (c1 >= 'a') ? (c1 - 'a' + 10) : (c1 - '0');
				int low = (c2 >= 'a') ? (c2 - 'a' + 10) : (c2 - '0');
				result.push_back(static_cast<unsigned char>((high << 4) | low));
			}
			return result;
		}
		static std::string en(const unsigned char* data, const size_t size)
		{
			if (!data || size <= 0) return {};
			std::string result;
			result.reserve(size * 2 + 1);
			for (size_t i = 0; i < size; i++)
			{
				size_t high = (data[i] >> 4) & 0x0F;
				size_t low = data[i] & 0x0F;
				result.push_back(static_cast<char>(high + ((high > 9) ? 'a' - 10 : '0')));
				result.push_back(static_cast<char>(low + ((low > 9) ? 'a' - 10 : '0')));
			}
			return result;
		}
		static std::string en(const std::vector<unsigned char>& data)
		{
            if (data.empty()) return {};
			return en(data.data(), data.size());
		}
        static std::string en(const std::string& data)
        {
            if (data.empty()) return {};
            return en(reinterpret_cast<const unsigned char*>(data.data()), data.size());
        }
	};

	class Base64
	{
        static constexpr char kBase64Chars[] =
            "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
            "abcdefghijklmnopqrstuvwxyz"
            "0123456789+/";
        static constexpr int kBase64DecodeTable[] = {
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
            -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 62, -1, -1, -1, 63,
            52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -1, -1, -1, -1, -1, -1,
            -1,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
            15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -1, -1, -1, -1, -1,
            -1, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
            41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -1, -1, -1, -1, -1
        };
    public:
        static std::vector<unsigned char> de(const std::string& base64) {
            size_t len = base64.length();
            size_t padding = 0;
            if (len > 0 && base64[len - 1] == '=') padding++;
            if (len > 1 && base64[len - 2] == '=') padding++;
            size_t output_size = (len * 3) / 4 - padding;
            std::vector<unsigned char> result;
            result.reserve(output_size);
            size_t i = 0;
            while (i < len) {
                int sextets[4] = { 0 };
                int sextet_count = 0;
                while (i < len && sextet_count < 4) {
                    char c = base64[i++];
                    if (c <= 127 && kBase64DecodeTable[static_cast<unsigned char>(c)] != -1) {
                        sextets[sextet_count++] = kBase64DecodeTable[static_cast<unsigned char>(c)];
                    }
                }
                if (sextet_count == 0) continue;
                unsigned char triple[3] = { 0 };
                triple[0] = (sextets[0] << 2) | ((sextets[1] & 0x30) >> 4);
                triple[1] = ((sextets[1] & 0x0F) << 4) | ((sextets[2] & 0x3C) >> 2);
                triple[2] = ((sextets[2] & 0x03) << 6) | sextets[3];
                if (sextet_count >= 2) result.push_back(triple[0]);
                if (sextet_count >= 3) result.push_back(triple[1]);
                if (sextet_count >= 4) result.push_back(triple[2]);
            }
            return result;
        }
        static std::string en(const unsigned char* data, const size_t size) {
            if (data == nullptr || size == 0) return {};
            size_t output_len = ((size + 2) / 3) * 4;
            std::string result;
            result.reserve(output_len);

            size_t i = 0;
            while (i < size) {
                size_t start = i;
                unsigned char byte1 = data[i++];
                unsigned char byte2 = (i < size) ? data[i++] : 0;
                unsigned char byte3 = (i < size) ? data[i++] : 0;
                size_t bytes_read = i - start;

                int sextet1 = (byte1 & 0xFC) >> 2;
                int sextet2 = ((byte1 & 0x03) << 4) | ((byte2 & 0xF0) >> 4);
                int sextet3 = ((byte2 & 0x0F) << 2) | ((byte3 & 0xC0) >> 6);
                int sextet4 = byte3 & 0x3F;

                result.push_back(kBase64Chars[sextet1]);
                result.push_back(kBase64Chars[sextet2]);

                if (bytes_read == 1) {
                    result.push_back('=');
                    result.push_back('=');
                }
                else if (bytes_read == 2) {
                    result.push_back(kBase64Chars[sextet3]);
                    result.push_back('=');
                }
                else {
                    result.push_back(kBase64Chars[sextet3]);
                    result.push_back(kBase64Chars[sextet4]);
                }
            }

            return result;
        }
        static std::string en(const std::vector<unsigned char>& data) {
            if (data.empty()) return {};
            return en(data.data(), data.size());
        }
        static std::string en(const std::string& data) {
            if (data.empty()) return {};
            return en(reinterpret_cast<const unsigned char*>(data.data()), data.size());
        }
	};

    class MD5
    {
        static uint32_t F(uint32_t x, uint32_t y, uint32_t z) { return (x & y) | (~x & z); }
        static uint32_t G(uint32_t x, uint32_t y, uint32_t z) { return (x & z) | (y & ~z); }
        static uint32_t H(uint32_t x, uint32_t y, uint32_t z) { return x ^ y ^ z; }
        static uint32_t I(uint32_t x, uint32_t y, uint32_t z) { return y ^ (x | ~z); }
        static uint32_t rotate_left(uint32_t x, uint32_t n) { return (x << n) | (x >> (32 - n)); }
        static void FF(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) { a += F(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        static void GG(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) { a += G(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        static void HH(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) { a += H(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        static void II(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t x, uint32_t s, uint32_t ac) { a += I(b, c, d) + x + ac; a = rotate_left(a, s); a += b; }
        static std::vector<unsigned char> pad_message(const unsigned char* data, size_t size)
        {
            uint64_t original_bits = size * 8;
            std::vector<unsigned char> padded(data, data + size);
            padded.push_back(0x80);
            while ((padded.size() * 8) % 512 != 448) padded.push_back(0x00);
            for (int i = 0; i < 8; ++i) padded.push_back(static_cast<unsigned char>((original_bits >> (i * 8)) & 0xFF));
            return padded;
        }
    public:
        static std::vector<unsigned char> en(const unsigned char* data, size_t size)
        {
            uint32_t A = 0x67452301;
            uint32_t B = 0xefcdab89;
            uint32_t C = 0x98badcfe;
            uint32_t D = 0x10325476;

            std::vector<unsigned char> padded = pad_message(data, size);

            for (size_t i = 0; i < padded.size(); i += 64)
            {
                const unsigned char* block = &padded[i];
                uint32_t X[16];
                for (int j = 0; j < 16; ++j) X[j] = (block[j * 4]) | (block[j * 4 + 1] << 8) | (block[j * 4 + 2] << 16) | (block[j * 4 + 3] << 24);

                uint32_t a = A, b = B, c = C, d = D;

                FF(a, b, c, d, X[0], 7, 0xd76aa478);
                FF(d, a, b, c, X[1], 12, 0xe8c7b756);
                FF(c, d, a, b, X[2], 17, 0x242070db);
                FF(b, c, d, a, X[3], 22, 0xc1bdceee);
                FF(a, b, c, d, X[4], 7, 0xf57c0faf);
                FF(d, a, b, c, X[5], 12, 0x4787c62a);
                FF(c, d, a, b, X[6], 17, 0xa8304613);
                FF(b, c, d, a, X[7], 22, 0xfd469501);
                FF(a, b, c, d, X[8], 7, 0x698098d8);
                FF(d, a, b, c, X[9], 12, 0x8b44f7af);
                FF(c, d, a, b, X[10], 17, 0xffff5bb1);
                FF(b, c, d, a, X[11], 22, 0x895cd7be);
                FF(a, b, c, d, X[12], 7, 0x6b901122);
                FF(d, a, b, c, X[13], 12, 0xfd987193);
                FF(c, d, a, b, X[14], 17, 0xa679438e);
                FF(b, c, d, a, X[15], 22, 0x49b40821);

                GG(a, b, c, d, X[1], 5, 0xf61e2562);
                GG(d, a, b, c, X[6], 9, 0xc040b340);
                GG(c, d, a, b, X[11], 14, 0x265e5a51);
                GG(b, c, d, a, X[0], 20, 0xe9b6c7aa);
                GG(a, b, c, d, X[5], 5, 0xd62f105d);
                GG(d, a, b, c, X[10], 9, 0x02441453);
                GG(c, d, a, b, X[15], 14, 0xd8a1e681);
                GG(b, c, d, a, X[4], 20, 0xe7d3fbc8);
                GG(a, b, c, d, X[9], 5, 0x21e1cde6);
                GG(d, a, b, c, X[14], 9, 0xc33707d6);
                GG(c, d, a, b, X[3], 14, 0xf4d50d87);
                GG(b, c, d, a, X[8], 20, 0x455a14ed);
                GG(a, b, c, d, X[13], 5, 0xa9e3e905);
                GG(d, a, b, c, X[2], 9, 0xfcefa3f8);
                GG(c, d, a, b, X[7], 14, 0x676f02d9);
                GG(b, c, d, a, X[12], 20, 0x8d2a4c8a);

                HH(a, b, c, d, X[5], 4, 0xfffa3942);
                HH(d, a, b, c, X[8], 11, 0x8771f681);
                HH(c, d, a, b, X[11], 16, 0x6d9d6122);
                HH(b, c, d, a, X[14], 23, 0xfde5380c);
                HH(a, b, c, d, X[1], 4, 0xa4beea44);
                HH(d, a, b, c, X[4], 11, 0x4bdecfa9);
                HH(c, d, a, b, X[7], 16, 0xf6bb4b60);
                HH(b, c, d, a, X[10], 23, 0xbebfbc70);
                HH(a, b, c, d, X[13], 4, 0x289b7ec6);
                HH(d, a, b, c, X[0], 11, 0xeaa127fa);
                HH(c, d, a, b, X[3], 16, 0xd4ef3085);
                HH(b, c, d, a, X[6], 23, 0x04881d05);
                HH(a, b, c, d, X[9], 4, 0xd9d4d039);
                HH(d, a, b, c, X[12], 11, 0xe6db99e5);
                HH(c, d, a, b, X[15], 16, 0x1fa27cf8);
                HH(b, c, d, a, X[2], 23, 0xc4ac5665);

                II(a, b, c, d, X[0], 6, 0xf4292244);
                II(d, a, b, c, X[7], 10, 0x432aff97);
                II(c, d, a, b, X[14], 15, 0xab9423a7);
                II(b, c, d, a, X[5], 21, 0xfc93a039);
                II(a, b, c, d, X[12], 6, 0x655b59c3);
                II(d, a, b, c, X[3], 10, 0x8f0ccc92);
                II(c, d, a, b, X[10], 15, 0xffeff47d);
                II(b, c, d, a, X[1], 21, 0x85845dd1);
                II(a, b, c, d, X[8], 6, 0x6fa87e4f);
                II(d, a, b, c, X[15], 10, 0xfe2ce6e0);
                II(c, d, a, b, X[6], 15, 0xa3014314);
                II(b, c, d, a, X[13], 21, 0x4e0811a1);
                II(a, b, c, d, X[4], 6, 0xf7537e82);
                II(d, a, b, c, X[11], 10, 0xbd3af235);
                II(c, d, a, b, X[2], 15, 0x2ad7d2bb);
                II(b, c, d, a, X[9], 21, 0xeb86d391);

                A += a;
                B += b;
                C += c;
                D += d;
            }

            std::vector<unsigned char> digest(16);
            auto write_le = [](uint32_t val, unsigned char* out) {
                out[0] = static_cast<unsigned char>(val & 0xFF);
                out[1] = static_cast<unsigned char>((val >> 8) & 0xFF);
                out[2] = static_cast<unsigned char>((val >> 16) & 0xFF);
                out[3] = static_cast<unsigned char>((val >> 24) & 0xFF);
                };
            write_le(A, &digest[0]);
            write_le(B, &digest[4]);
            write_le(C, &digest[8]);
            write_le(D, &digest[12]);
            return digest;
        }
        static std::vector<unsigned char> en(const std::vector<unsigned char>& data)
        {
            if (data.empty()) return en(nullptr, 0);
            return en(data.data(), data.size());
        }
        static std::vector<unsigned char> en(const std::string& str)
        {
            return en(reinterpret_cast<const unsigned char*>(str.data()), str.size());
        }

        static std::string en_hex(const unsigned char* data, size_t size)
        {
            auto digest = en(data, size);
            std::ostringstream oss;
            for (unsigned char b : digest) oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
            return oss.str();
        }
        static std::string en_hex(const std::vector<unsigned char>& data)
        {
            return en_hex(data.data(), data.size());
        }
        static std::string en_hex(const std::string& str)
        {
            return en_hex(reinterpret_cast<const unsigned char*>(str.data()), str.size());
        }
    };

    class Sha256 {
        static constexpr uint32_t H0[] = {
            0x6a09e667, 0xbb67ae85, 0x3c6ef372, 0xa54ff53a,
            0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
        };
        static constexpr uint32_t K[] = {
            0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
            0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
            0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
            0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
            0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
            0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
            0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
            0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
        };

        static constexpr uint32_t rotr(uint32_t x, uint32_t n) { return (x >> n) | (x << (32 - n)); }
        static constexpr uint32_t shr(uint32_t x, uint32_t n) { return x >> n; }
        static constexpr uint32_t Ch(uint32_t e, uint32_t f, uint32_t g) { return (e & f) ^ (~e & g); }
        static constexpr uint32_t Maj(uint32_t a, uint32_t b, uint32_t c) { return (a & b) ^ (a & c) ^ (b & c); }
        static constexpr uint32_t Sigma0(uint32_t a) { return rotr(a, 2) ^ rotr(a, 13) ^ rotr(a, 22); }
        static constexpr uint32_t Sigma1(uint32_t e) { return rotr(e, 6) ^ rotr(e, 11) ^ rotr(e, 25); }
        static constexpr uint32_t sigma0(uint32_t x) { return rotr(x, 7) ^ rotr(x, 18) ^ shr(x, 3); }
        static constexpr uint32_t sigma1(uint32_t x) { return rotr(x, 17) ^ rotr(x, 19) ^ shr(x, 10); }

    public:
        static std::vector<unsigned char> en(const unsigned char* input, size_t size)
        {
            std::vector<unsigned char> padded(input, input + size);
            padded.push_back(0x80);

            while ((padded.size() % 64) != 56) padded.push_back(0x00);

            uint64_t bit_len = static_cast<uint64_t>(size) * 8;
            for (int i = 7; i >= 0; --i) padded.push_back(static_cast<unsigned char>((bit_len >> (i * 8)) & 0xFF));

            uint32_t state[8];
            std::memcpy(state, H0, sizeof(H0));

            for (size_t p = 0; p < padded.size(); p += 64)
            {
                const unsigned char* block = &padded[p];
                uint32_t W[64];

                for (int i = 0; i < 16; ++i) W[i] = (static_cast<uint32_t>(block[i * 4]) << 24) | (static_cast<uint32_t>(block[i * 4 + 1]) << 16) | (static_cast<uint32_t>(block[i * 4 + 2]) << 8) | (static_cast<uint32_t>(block[i * 4 + 3]));

                for (int t = 16; t < 64; ++t) W[t] = sigma1(W[t - 2]) + W[t - 7] + sigma0(W[t - 15]) + W[t - 16];

                uint32_t a = state[0];
                uint32_t b = state[1];
                uint32_t c = state[2];
                uint32_t d = state[3];
                uint32_t e = state[4];
                uint32_t f = state[5];
                uint32_t g = state[6];
                uint32_t h = state[7];

                for (int t = 0; t < 64; ++t)
                {
                    uint32_t T1 = h + Sigma1(e) + Ch(e, f, g) + K[t] + W[t];
                    uint32_t T2 = Sigma0(a) + Maj(a, b, c);
                    h = g;
                    g = f;
                    f = e;
                    e = d + T1;
                    d = c;
                    c = b;
                    b = a;
                    a = T1 + T2;
                }

                state[0] += a;
                state[1] += b;
                state[2] += c;
                state[3] += d;
                state[4] += e;
                state[5] += f;
                state[6] += g;
                state[7] += h;
            }

            std::vector<unsigned char> digest(32);
            for (int i = 0; i < 8; ++i)
            {
                digest[i * 4] = static_cast<unsigned char>((state[i] >> 24) & 0xFF);
                digest[i * 4 + 1] = static_cast<unsigned char>((state[i] >> 16) & 0xFF);
                digest[i * 4 + 2] = static_cast<unsigned char>((state[i] >> 8) & 0xFF);
                digest[i * 4 + 3] = static_cast<unsigned char>(state[i] & 0xFF);
            }
            return digest;
        }
        static std::vector<unsigned char> en(const std::vector<unsigned char>& input) { return en(input.data(), input.size()); }
        static std::vector<unsigned char> en(const std::string& input) { return en(reinterpret_cast<const unsigned char*>(input.data()), input.size()); }

        static std::string en_hex(const unsigned char* input, size_t size)
        {
            auto sha = en(input, size);
            std::ostringstream oss;
            for (unsigned char b : sha) oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(b);
            return oss.str();
        }
        static std::string en_hex(const std::vector<unsigned char>& input) { return en_hex(input.data(), input.size()); }
        static std::string en_hex(const std::string& input) { return en_hex(reinterpret_cast<const unsigned char*>(input.data()), input.size()); }
    };
}
#endif