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
    static bool isSATerm(const SentenceBreakProperty prop) {
        return prop == STerm || prop == ATerm;
    }

    // Helper: Check if property should be ignored for SB5
    static bool isIgnoredForSB5(const SentenceBreakProperty prop) {
        return prop == Extend || prop == Format;
    }

    // SB11: SATerm Close* Sp* ParaSep? ÷
    enum class SentenceState {
        Initial,
        ATerm,
        STerm,
        ASp,
        SSp,
        ParaSep,
    };

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

        // Precompute properties (with sentinel at end)
        std::vector<SentenceBreakProperty> props(n + 1);
        for (size_t i = 0; i < n; ++i) {
            props[i] = findSentenceBreakProperty(codepoints[i]);
        }
        props[n] = Other;

        // Precompute next effective indices (for SB5 optimization)
        std::vector<size_t> nextEffectiveIndices(n + 1);
        size_t lastEffectiveIndex = n;
        for (int i = static_cast<int>(n - 1); i >= 0; --i) {
            nextEffectiveIndices[i] = lastEffectiveIndex;
            if (!isIgnoredForSB5(props[i])) {
                lastEffectiveIndex = i;
            }
        }
        nextEffectiveIndices[n] = n;

        // SB8: ATerm Close* Sp* × ( ¬(OLetter | Upper | Lower | ParaSep | SATerm) )* Lower
        std::vector<bool> hasLowerAfterCache(n + 1);
        hasLowerAfterCache[n] = false;
        for (int i = static_cast<int>(n - 1); i >= 0; --i) {
            if (const auto p = props[i]; p == Lower) {
                hasLowerAfterCache[i] = true;
            } else if (p == OLetter || p == Upper || isParaSep(p) || isSATerm(p)) {
                hasLowerAfterCache[i] = false;
            } else {
                hasLowerAfterCache[i] = hasLowerAfterCache[i + 1];
            }
        }

        // SB1: sot ÷ Any
        std::vector<std::int32_t>::difference_type lastBreak = 0;
        auto state = SentenceState::Initial;
        if (props[0] == ATerm) {
            state = SentenceState::ATerm;
        } else if (props[0] == STerm) {
            state = SentenceState::STerm;
        }

        // Track previous effective properties incrementally
        auto prevEffective = Other;
        auto prevPrevEffective = Other;

        std::vector<std::vector<std::int32_t>> result;

        for (size_t i = 0; i + 1 < n; ++i) {
            const auto currentProp = props[i];
            const auto nextProp = props[i + 1];

            const auto nextEffective = props[nextEffectiveIndices[i]];
            if (!isIgnoredForSB5(currentProp)) {
                prevPrevEffective = prevEffective;
                prevEffective = currentProp;
            }

            bool breakSentence = false;
            if (currentProp == CR && nextProp == LF) {
                // SB3: CR × LF
            } else if (isParaSep(currentProp)) {
                // SB4: ParaSep ÷
                breakSentence = true;
            } else if (isIgnoredForSB5(nextProp)) {
                // SB5: X (Extend | Format)* → X
            } else if (prevEffective == ATerm && nextEffective == Numeric) {
                // SB6: ATerm × Numeric
            } else if ((prevPrevEffective == Upper || prevPrevEffective == Lower) && prevEffective == ATerm
                && nextEffective == Upper) {
                // SB7: (Upper | Lower) ATerm × Upper
            } else if ((state == SentenceState::ATerm || state == SentenceState::ASp) && hasLowerAfterCache[i + 1]) {
                // SB8: ATerm Close* Sp* × ( ¬(OLetter | Upper | Lower | ParaSep | SATerm) )* Lower
            } else if ((state == SentenceState::ATerm || state == SentenceState::STerm
                || state == SentenceState::ASp || state == SentenceState::SSp)
                && (nextEffective == SContinue || isSATerm(nextEffective))) {
                // SB8a: (STerm | ATerm) Close* Sp* × (SContinue | SATerm)
            } else if ((state == SentenceState::ATerm || state == SentenceState::STerm)
                && (nextEffective == Close || nextEffective == Sp || isParaSep(nextEffective))) {
                // SB9: (STerm | ATerm) Close* × (Close | Sp | ParaSep)
            } else if ((state == SentenceState::ATerm || state == SentenceState::STerm
                || state == SentenceState::ASp || state == SentenceState::SSp)
                && (nextEffective == Sp || isSATerm(nextEffective))) {
                // SB10: (STerm | ATerm) Close* Sp* × (Sp | ParaSep)
            } else if (((state == SentenceState::ATerm || state == SentenceState::STerm
                || state == SentenceState::ASp || state == SentenceState::SSp)
                && !isParaSep(nextEffective))
                || state == SentenceState::ParaSep) {
                // SB11: SATerm Close* Sp* ParaSep? ÷
                breakSentence = true;
            }
            // SB998: Any × Any (don't break)

            if (!isIgnoredForSB5(nextProp)) {
                switch (state) {
                    case SentenceState::Initial:
                        break;
                    case SentenceState::ATerm:
                        if (nextProp == Close) {
                            state = SentenceState::ATerm;
                        } else if (nextProp == Sp) {
                            state = SentenceState::ASp;
                        } else if (isParaSep(nextProp)) {
                            state = SentenceState::ParaSep;
                        } else {
                            state = SentenceState::Initial;
                        }
                        break;
                    case SentenceState::STerm:
                        if (nextProp == Close) {
                            state = SentenceState::STerm;
                        } else if (nextProp == Sp) {
                            state = SentenceState::SSp;
                        } else if (isParaSep(nextProp)) {
                            state = SentenceState::ParaSep;
                        } else {
                            state = SentenceState::Initial;
                        }
                        break;
                    case SentenceState::ASp:
                        if (nextProp == Sp) {
                            state = SentenceState::ASp;
                        } else if (isParaSep(nextProp)) {
                            state = SentenceState::ParaSep;
                        } else {
                            state = SentenceState::Initial;
                        }
                        break;
                    case SentenceState::SSp:
                        if (nextProp == Sp) {
                            state = SentenceState::SSp;
                        } else if (isParaSep(nextProp)) {
                            state = SentenceState::ParaSep;
                        } else {
                            state = SentenceState::Initial;
                        }
                        break;
                    case SentenceState::ParaSep:
                        state = SentenceState::Initial;
                        break;
                }
                if (state == SentenceState::Initial) {
                    if (nextProp == ATerm) {
                        state = SentenceState::ATerm;
                    } else if (nextProp == STerm) {
                        state = SentenceState::STerm;
                    }
                }
            }

            if (breakSentence) {
                const auto offset = static_cast<std::vector<std::int32_t>::difference_type>(i + 1);
                result.emplace_back(codepoints.begin() + lastBreak, codepoints.begin() + offset);
                lastBreak = offset;
            }
        }

        // SB2: Any ÷ eot
        result.emplace_back(codepoints.begin() + lastBreak, codepoints.end());
        return result;
    }

}
