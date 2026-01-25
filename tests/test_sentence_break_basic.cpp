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
