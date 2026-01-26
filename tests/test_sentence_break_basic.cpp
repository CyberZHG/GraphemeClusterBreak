#include <gtest/gtest.h>
#include "sentence_break.h"
using namespace std;
using namespace sentence_break;

TEST(TestSentenceBreakBasic, EmptyString) {
    const string s;
    EXPECT_TRUE(segmentSentences(s).empty());
    constexpr vector<int32_t> codepoints;
    EXPECT_TRUE(segmentSentences(codepoints).empty());
}

// ATerm = '.' (U+002E)
// STerm = '!' (U+0021)
// Sp = ' ' (U+0020)
// ParaSep = '\n' (U+000A, LF)
// Other = '@' (U+0040)

TEST(TestSentenceBreakBasic, ATerm_Sp_Sp_ParaSep_Other) {
    // ".  \n@" -> ATerm Sp Sp LF Other
    // SB11: ATerm Sp* ParaSep ÷
    // SB4: ParaSep ÷
    const string s = ".  \n@";
    const auto expected = vector<string>{".  \n", "@"};
    const auto segmented = segmentSentences(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSentenceBreakBasic, STerm_Sp_STerm) {
    // "! !" -> STerm Sp STerm
    // SB11: STerm Sp* ÷
    const string s = "! !";
    const auto expected = vector<string>{"! !"};
    const auto segmented = segmentSentences(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSentenceBreakBasic, STerm_Sp_ParaSep_Other) {
    // "! \n@" -> STerm Sp LF Other
    // SB4: ParaSep ÷ (break after LF)
    const string s = "! \n@";
    const auto expected = vector<string>{"! \n", "@"};
    const auto segmented = segmentSentences(s);
    EXPECT_EQ(expected, segmented);
}
