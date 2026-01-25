#include <gtest/gtest.h>
#include <sstream>
#include "grapheme_break_properties.h"
using namespace grapheme_break;

TEST(TestFindBreakProperty, uniform_sampling) {
    for (int32_t code = 0; code < 0xEFFFF; code += 377) {
        ASSERT_EQ(findBreakPropertyBruteForce(code), findBreakProperty(code));
        ASSERT_EQ(findIndicBreakPropertyBruteForce(code), findIndicBreakProperty(code));
    }
}

TEST(TestGraphemeClusterBreakPropertyOutput, all_values) {
    auto to_string = [](const GraphemeClusterBreakProperty p) {
        std::ostringstream oss;
        oss << p;
        return oss.str();
    };

    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::CR), "CR");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::LF), "LF");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::Control), "Control");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::L), "L");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::V), "V");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::T), "T");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::LV), "LV");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::LVT), "LVT");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::Extend), "Extend");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::ZWJ), "ZWJ");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::SpacingMark), "SpacingMark");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::Prepend), "Prepend");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::Extended_Pictographic), "Extended_Pictographic");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::Regional_Indicator), "Regional_Indicator");
    EXPECT_EQ(to_string(GraphemeClusterBreakProperty::Other), "Other");
}

TEST(TestIndicConjunctBreakPropertyOutput, all_values) {
    auto to_string = [](const IndicConjunctBreakProperty p) {
        std::ostringstream oss;
        oss << p;
        return oss.str();
    };

    EXPECT_EQ(to_string(IndicConjunctBreakProperty::InCB_Consonant), "InCB_Consonant");
    EXPECT_EQ(to_string(IndicConjunctBreakProperty::InCB_Linker), "InCB_Linker");
    EXPECT_EQ(to_string(IndicConjunctBreakProperty::InCB_Extend), "InCB_Extend");
    EXPECT_EQ(to_string(IndicConjunctBreakProperty::InCB_Other), "InCB_Other");
}
