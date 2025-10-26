#include <gtest/gtest.h>
#include "grapheme_cluster.h"
using namespace std;
using namespace grapheme_cluster;

TEST(TestSegmentBasic, EmptyString) {
    const string s;
    EXPECT_TRUE(segmentGraphemeClusters(s).empty());
    constexpr vector<int32_t> codepoints;
    EXPECT_TRUE(segmentGraphemeClusters(codepoints).empty());
}

TEST(TestSegmentBasic, GB3) {
    const string s = "\r\n";
    const auto expected = vector<string>{"\r\n"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB3__Inverse) {
    const string s = "\n\r";
    const auto expected = vector<string>{"\n", "\r"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB4) {
    const string s = "\na";
    const auto expected = vector<string>{"\n", "a"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB5) {
    const string s = "a\n";
    const auto expected = vector<string>{"a", "\n"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB4_5__n) {
    const string s = "\n\n";
    const auto expected = vector<string>{"\n", "\n"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB4_5__r) {
    const string s = "\r\r";
    const auto expected = vector<string>{"\r", "\r"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB4__utf8) {
    const string s("\x0A\x00", 2);
    const auto expected = vector<string>{"\x0A", string("\x00", 1)};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected, segmented);
}

TEST(TestSegmentBasic, GB9c__ZWJ) {
    const vector<int32_t> codepoints = {0x0915, 0x094D, 0x200D, 0x0924};
    const auto expected = vector<vector<int32_t>>{codepoints};
    const auto segmented = segmentGraphemeClusters(codepoints);
    EXPECT_EQ(expected, segmented);
}