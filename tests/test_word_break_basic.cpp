#include <gtest/gtest.h>
#include "word_break.h"
using namespace std;
using namespace word_break;

TEST(TestWordBreakBasic, EmptyString) {
    const string s;
    EXPECT_TRUE(segmentWords(s).empty());
    constexpr vector<int32_t> codepoints;
    EXPECT_TRUE(segmentWords(codepoints).empty());
}
