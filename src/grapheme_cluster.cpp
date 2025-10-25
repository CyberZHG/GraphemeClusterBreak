#include "grapheme_cluster.h"
#include "break_properties.h"

#include <format>

namespace grapheme_cluster {

    static constexpr auto CR = GraphemeClusterBreakProperty::CR;
    static constexpr auto LF = GraphemeClusterBreakProperty::LF;
    static constexpr auto Control = GraphemeClusterBreakProperty::Control;
    static constexpr auto L = GraphemeClusterBreakProperty::L;
    static constexpr auto V = GraphemeClusterBreakProperty::V;
    static constexpr auto T = GraphemeClusterBreakProperty::T;
    static constexpr auto LV = GraphemeClusterBreakProperty::LV;
    static constexpr auto LVT = GraphemeClusterBreakProperty::LVT;
    static constexpr auto Extend = GraphemeClusterBreakProperty::Extend;
    static constexpr auto ZWJ = GraphemeClusterBreakProperty::ZWJ;
    static constexpr auto SpacingMark = GraphemeClusterBreakProperty::SpacingMark;
    static constexpr auto Prepend = GraphemeClusterBreakProperty::Prepend;
    static constexpr auto Extended_Pictographic = GraphemeClusterBreakProperty::Extended_Pictographic;
    static constexpr auto Regional_Indicator = GraphemeClusterBreakProperty::Regional_Indicator;
    static constexpr auto Other = GraphemeClusterBreakProperty::Other;

    static constexpr auto InCB_Consonant = IndicConjunctBreakProperty::InCB_Consonant;
    static constexpr auto InCB_Linker = IndicConjunctBreakProperty::InCB_Linker;
    static constexpr auto InCB_Extend = IndicConjunctBreakProperty::InCB_Extend;
    static constexpr auto InCB_Other = IndicConjunctBreakProperty::InCB_Other;

    static std::vector<std::int32_t> utf8ToCodepoints(const std::string& utf8) {
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

    static std::string codepointsToUtf8(const std::vector<std::int32_t> &codepoints) {
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

    std::vector<std::string> segmentGraphemeClusters(const std::string& s) {
        if (s.empty()) {
            return {};
        }
        const auto codepoints = utf8ToCodepoints(s);
        const auto segments = segmentGraphemeClusters(codepoints);
        std::vector<std::string> result(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            result[i] = codepointsToUtf8(segments[i]);
        }
        return result;
    }

    std::vector<std::vector<std::int32_t>> segmentGraphemeClusters(const std::vector<std::int32_t>& codepoints) {
        if (codepoints.empty()) {
            return {};
        }
        const auto n = codepoints.size();
        if (n > std::numeric_limits<std::vector<std::int32_t>::difference_type>::max()) {
            throw std::runtime_error(std::format("Can not process large vector with size: {}", n));
        }
        // GB1: sot ÷ Any
        std::vector<std::int32_t>::difference_type lastBreak = 0;
        GraphemeClusterBreakProperty currentBreakProperty = findBreakProperty(codepoints[0]);
        std::vector<std::vector<std::int32_t>> result;
        for (size_t i = 0; i + 1 < n; ++i) {
            const GraphemeClusterBreakProperty nextBreakProperty = findBreakProperty(codepoints[i + 1]);
            bool breakCluster = false;
            if (currentBreakProperty == CR && nextBreakProperty == LF) {
                // GB3: CR × LF
            } else if (currentBreakProperty == Control || currentBreakProperty == CR || currentBreakProperty == LF) {
                // GB4: (Control | CR | LF)	÷
                breakCluster = true;
            } else if (nextBreakProperty == Control || nextBreakProperty == CR || nextBreakProperty == LF) {
                // GB5: ÷ (Control | CR | LF)
                breakCluster = true;
            }
            if (breakCluster) {
                const auto offset = static_cast<std::vector<std::int32_t>::difference_type>(i + 1);
                result.emplace_back(codepoints.begin() + lastBreak, codepoints.begin() + offset);
                lastBreak = offset;
            }
            currentBreakProperty = nextBreakProperty;
        }
        // GB2: Any ÷ eot
        result.emplace_back(codepoints.begin() + lastBreak, codepoints.end());
        return result;
    }

}
