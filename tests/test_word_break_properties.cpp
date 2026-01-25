#include <gtest/gtest.h>
#include <sstream>
#include "word_break_properties.h"
using namespace word_break;

TEST(TestFindWordBreakProperty, uniform_sampling) {
    for (int32_t code = 0; code < 0xEFFFF; code += 377) {
        ASSERT_EQ(findWordBreakPropertyBruteForce(code), findWordBreakProperty(code));
    }
}

TEST(TestWordBreakPropertyOutput, all_values) {
    auto to_string = [](const WordBreakProperty p) {
        std::ostringstream oss;
        oss << p;
        return oss.str();
    };

    EXPECT_EQ(to_string(WordBreakProperty::CR), "CR");
    EXPECT_EQ(to_string(WordBreakProperty::LF), "LF");
    EXPECT_EQ(to_string(WordBreakProperty::Newline), "Newline");
    EXPECT_EQ(to_string(WordBreakProperty::Extend), "Extend");
    EXPECT_EQ(to_string(WordBreakProperty::ZWJ), "ZWJ");
    EXPECT_EQ(to_string(WordBreakProperty::Regional_Indicator), "Regional_Indicator");
    EXPECT_EQ(to_string(WordBreakProperty::Format), "Format");
    EXPECT_EQ(to_string(WordBreakProperty::Katakana), "Katakana");
    EXPECT_EQ(to_string(WordBreakProperty::Hebrew_Letter), "Hebrew_Letter");
    EXPECT_EQ(to_string(WordBreakProperty::ALetter), "ALetter");
    EXPECT_EQ(to_string(WordBreakProperty::Single_Quote), "Single_Quote");
    EXPECT_EQ(to_string(WordBreakProperty::Double_Quote), "Double_Quote");
    EXPECT_EQ(to_string(WordBreakProperty::MidNumLet), "MidNumLet");
    EXPECT_EQ(to_string(WordBreakProperty::MidLetter), "MidLetter");
    EXPECT_EQ(to_string(WordBreakProperty::MidNum), "MidNum");
    EXPECT_EQ(to_string(WordBreakProperty::Numeric), "Numeric");
    EXPECT_EQ(to_string(WordBreakProperty::ExtendNumLet), "ExtendNumLet");
    EXPECT_EQ(to_string(WordBreakProperty::WSegSpace), "WSegSpace");
    EXPECT_EQ(to_string(WordBreakProperty::Other), "Other");
}
