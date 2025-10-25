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
    const auto expected = vector{"\r\n"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected.size(), segmented.size());
}

TEST(TestSegmentBasic, GB3__Inverse) {
    const string s = "\n\r";
    const auto expected = vector{"\n", "\r"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected.size(), segmented.size());
}

TEST(TestSegmentBasic, GB4) {
    const string s = "\na";
    const auto expected = vector{"\n", "a"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected.size(), segmented.size());
}

TEST(TestSegmentBasic, GB5) {
    const string s = "a\n";
    const auto expected = vector{"a", "\n"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected.size(), segmented.size());
}

TEST(TestSegmentBasic, GB4_5__n) {
    const string s = "\n\n";
    const auto expected = vector{"\n", "\n"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected.size(), segmented.size());
}

TEST(TestSegmentBasic, GB4_5__r) {
    const string s = "\r\r";
    const auto expected = vector{"\r", "\r"};
    const auto segmented = segmentGraphemeClusters(s);
    EXPECT_EQ(expected.size(), segmented.size());
}