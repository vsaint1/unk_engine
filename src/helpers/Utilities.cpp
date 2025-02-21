#include "../public/helpers/Utilities.h"



std::string Decode_Base64(const std::string& encoded) {
    std::string decoded;
    std::vector<int> char_map(256, -1);

    for (int i = 0; i < 64; i++) {
        char_map[BASE64_CHARS[i]] = i;
    }

    int val = 0, bits = -8;
    for (unsigned char c : encoded) {
        if (char_map[c] == -1) {
            continue;
        }

        val = (val << 6) + char_map[c];
        bits += 6;

        if (bits >= 0) {
            decoded.push_back(static_cast<char>((val >> bits) & 0xFF));
            bits -= 8;
        }
    }

    return decoded;
}
