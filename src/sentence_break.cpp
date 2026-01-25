#include "sentence_break.h"
#include "sentence_break_properties.h"
#include "unicode_utils.h"

#include <format>

namespace sentence_break {

    static constexpr auto CR = SentenceBreakProperty::CR;
    static constexpr auto LF = SentenceBreakProperty::LF;
    static constexpr auto Sep = SentenceBreakProperty::Sep;
    static constexpr auto Extend = SentenceBreakProperty::Extend;
    static constexpr auto Format = SentenceBreakProperty::Format;
    static constexpr auto STerm = SentenceBreakProperty::STerm;
    static constexpr auto ATerm = SentenceBreakProperty::ATerm;
    static constexpr auto Numeric = SentenceBreakProperty::Numeric;
    static constexpr auto Upper = SentenceBreakProperty::Upper;
    static constexpr auto Sp = SentenceBreakProperty::Sp;
    static constexpr auto Lower = SentenceBreakProperty::Lower;
    static constexpr auto OLetter = SentenceBreakProperty::OLetter;
    static constexpr auto SContinue = SentenceBreakProperty::SContinue;
    static constexpr auto Close = SentenceBreakProperty::Close;
    static constexpr auto Other = SentenceBreakProperty::Other;

    // Helper: ParaSep = Sep | CR | LF
    static bool isParaSep(const SentenceBreakProperty prop) {
        return prop == Sep || prop == CR || prop == LF;
    }

    // Helper: SATerm = STerm | ATerm
    static bool SATerm(const SentenceBreakProperty prop) {
        return prop == STerm || prop == ATerm;
    }

    std::vector<std::string> segmentSentences(const std::string& s) {
        if (s.empty()) {
            return {};
        }
        const auto codepoints = unicode_utils::utf8ToCodepoints(s);
        const auto segments = segmentSentences(codepoints);
        std::vector<std::string> result(segments.size());
        for (size_t i = 0; i < segments.size(); ++i) {
            result[i] = unicode_utils::codepointsToUtf8(segments[i]);
        }
        return result;
    }

    std::vector<std::vector<std::int32_t>> segmentSentences(const std::vector<std::int32_t>& codepoints) {
        if (codepoints.empty()) {
            return {};
        }
        const auto n = codepoints.size();
        if (n > std::numeric_limits<std::vector<std::int32_t>::difference_type>::max()) {
            throw std::runtime_error(std::format("Can not process large vector with size: {}", n));
        }
        // TODO
        return {};
    }

}
