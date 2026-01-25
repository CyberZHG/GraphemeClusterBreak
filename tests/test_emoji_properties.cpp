#include <gtest/gtest.h>
#include "emoji_pictographic_properties.h"
using namespace grapheme_break;

TEST(TestEmojiProperty, uniform_sampling) {
    for (int32_t code = 0; code < 0xEFFFF; code += 377) {
        ASSERT_EQ(isExtendedPictographicBruteForce(code), isExtendedPictographic(code));
    }
}
