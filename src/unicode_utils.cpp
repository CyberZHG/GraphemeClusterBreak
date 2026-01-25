#include "unicode_utils.h"

namespace unicode_utils {

    std::vector<std::int32_t> utf8ToCodepoints(const std::string& utf8) {
        std::vector<std::int32_t> result;
        const auto *bytes = reinterpret_cast<const unsigned char*>(utf8.data());
        size_t i = 0;
        while (i < utf8.size()) {
            std::int32_t codepoint;
            if ((bytes[i] & 0x80) == 0) {
                // 1 byte: 0xxxxxxx
                codepoint = bytes[i];
                i += 1;
            } else if ((bytes[i] & 0xE0) == 0xC0) {
                // 2 bytes: 110xxxxx 10xxxxxx
                codepoint = (bytes[i] & 0x1F) << 6 | (bytes[i + 1] & 0x3F);
                i += 2;
            } else if ((bytes[i] & 0xF0) == 0xE0) {
                // 3 bytes: 1110xxxx 10xxxxxx 10xxxxxx
                codepoint = (bytes[i] & 0x0F) << 12 | (bytes[i + 1] & 0x3F) << 6 | (bytes[i + 2] & 0x3F);
                i += 3;
            } else {
                // 4 bytes: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
                codepoint = (bytes[i] & 0x07) << 18 | (bytes[i + 1] & 0x3F) << 12 | (bytes[i + 2] & 0x3F) << 6 | (bytes[i + 3] & 0x3F);
                i += 4;
            }
            result.push_back(codepoint);
        }
        return result;
    }

    std::string codepointsToUtf8(const std::vector<std::int32_t>& codepoints) {
        std::string result;
        for (const std::int32_t cp : codepoints) {
            if (cp < 0x80) {
                // 1 byte
                result.push_back(static_cast<char>(cp));
            } else if (cp < 0x800) {
                // 2 bytes
                result.push_back(static_cast<char>(0xC0 | (cp >> 6)));
                result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            } else if (cp < 0x10000) {
                // 3 bytes
                result.push_back(static_cast<char>(0xE0 | (cp >> 12)));
                result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            } else {
                // 4 bytes
                result.push_back(static_cast<char>(0xF0 | (cp >> 18)));
                result.push_back(static_cast<char>(0x80 | ((cp >> 12) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | ((cp >> 6) & 0x3F)));
                result.push_back(static_cast<char>(0x80 | (cp & 0x3F)));
            }
        }
        return result;
    }

}
