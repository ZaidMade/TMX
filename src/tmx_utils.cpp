#include "tmx_utils.h"

/**
 * Get the offset of given character in the base64 set.
 *
 * @param p_c Character to find the offset of.
 * @returns [char] The offset of the character.
 */
static char base64_value(char p_c) {
	if (p_c >= 'A' && p_c <= 'Z') return p_c - 'A';
	else if (p_c >= 'a' && p_c <= 'z') return p_c - 'a' + 26;
	else if (p_c >= '0' && p_c <= '9') return p_c - '0' + 52;
	else if (p_c == '+') return 62;
	else if (p_c == '/') return 63;
	else if (p_c == '=') return 0;
	return -1;
}

namespace tmx {
	std::string base64_decode(const char* p_raw, const size_t p_len) {
        // Read 4 base64 chars, write 3 bytes.
        // 1st byte = (1st char << 2) + (2nd char >> 4)

		const unsigned int rlen = (p_len/4)*3;
		char* buff = new char[rlen];

		unsigned int in = 0;
		char v;

		for (unsigned int i = 0; i < p_len; i += 4) {
			for(unsigned int c = 0; c < 4; c++){
				v = base64_value(p_raw[i + c]);

				in = in << 6;
				in += v;
			}
			for (unsigned int j = 0; j<3; j++)
				memcpy(buff + (i / 4) * 3 + j, ((char*)&in) + 2 - j, 1);
		}
		return std::string(buff);
	}
}
