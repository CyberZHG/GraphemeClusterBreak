#include <gtest/gtest.h>
#include <sstream>
#include "sentence_break_properties.h"
using namespace sentence_break;

TEST(TestFindSentenceBreakProperty, uniform_sampling) {
    for (int32_t code = 0; code < 0xEFFFF; code += 377) {
        ASSERT_EQ(findSentenceBreakPropertyBruteForce(code), findSentenceBreakProperty(code));
    }
}

TEST(TestSentenceBreakPropertyOutput, all_values) {
    auto to_string = [](const SentenceBreakProperty p) {
        std::ostringstream oss;
        oss << p;
        return oss.str();
    };

    EXPECT_EQ(to_string(SentenceBreakProperty::CR), "CR");
    EXPECT_EQ(to_string(SentenceBreakProperty::LF), "LF");
    EXPECT_EQ(to_string(SentenceBreakProperty::Sep), "Sep");
    EXPECT_EQ(to_string(SentenceBreakProperty::Extend), "Extend");
    EXPECT_EQ(to_string(SentenceBreakProperty::Format), "Format");
    EXPECT_EQ(to_string(SentenceBreakProperty::STerm), "STerm");
    EXPECT_EQ(to_string(SentenceBreakProperty::ATerm), "ATerm");
    EXPECT_EQ(to_string(SentenceBreakProperty::Numeric), "Numeric");
    EXPECT_EQ(to_string(SentenceBreakProperty::Upper), "Upper");
    EXPECT_EQ(to_string(SentenceBreakProperty::Sp), "Sp");
    EXPECT_EQ(to_string(SentenceBreakProperty::Lower), "Lower");
    EXPECT_EQ(to_string(SentenceBreakProperty::OLetter), "OLetter");
    EXPECT_EQ(to_string(SentenceBreakProperty::SContinue), "SContinue");
    EXPECT_EQ(to_string(SentenceBreakProperty::Close), "Close");
    EXPECT_EQ(to_string(SentenceBreakProperty::Other), "Other");
}
