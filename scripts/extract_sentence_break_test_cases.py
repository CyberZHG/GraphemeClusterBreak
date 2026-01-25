from pathlib import Path

DATA_DIR = Path(__file__).parent.parent / "ucd"
TESTS_DIR = Path(__file__).parent.parent / "tests"


def parse_test_line(line: str):
    """
    Parse a test line like:
    ÷ 000D × 000A ÷	#  ÷ [0.2] <CARRIAGE RETURN (CR)> ...

    Returns (codepoints, break_positions, comment) where:
    - codepoints: list of int
    - break_positions: list of bool (True = break after this codepoint)
    - comment: str
    """
    if "#" in line:
        data_part, comment = line.split("#", 1)
        comment = comment.strip()
    else:
        data_part = line
        comment = ""

    tokens = data_part.split()
    codepoints = []
    break_positions = []

    for token in tokens:
        if token == "÷":
            if codepoints:
                break_positions.append(True)
        elif token == "×":
            if codepoints:
                break_positions.append(False)
        else:
            codepoints.append(int(token, 16))

    return codepoints, break_positions, comment


def generate_expected_segments(codepoints: list, break_positions: list):
    """
    Generate expected segments from codepoints and break positions.
    """
    if not codepoints:
        return []

    segments = []
    current_segment = [codepoints[0]]

    for i, should_break in enumerate(break_positions):
        if should_break:
            segments.append(current_segment)
            current_segment = []
        if i + 1 < len(codepoints):
            current_segment.append(codepoints[i + 1])

    if current_segment:
        segments.append(current_segment)

    return segments


def codepoints_to_cpp_vector(codepoints: list) -> str:
    """Convert codepoints to C++ vector initializer."""
    return "{" + ", ".join(f"0x{cp:04X}" for cp in codepoints) + "}"


def segments_to_cpp_vector(segments: list) -> str:
    """Convert segments to C++ nested vector initializer."""
    inner = ", ".join(codepoints_to_cpp_vector(seg) for seg in segments)
    return "{" + inner + "}"


def escape_cpp_string(s: str) -> str:
    """Escape a string for C++ string literal."""
    return s.replace("\\", "\\\\").replace('"', '\\"').replace("\n", "\\n")


def codepoint_to_utf8_bytes(cp: int) -> bytes:
    """Convert a single codepoint to UTF-8 bytes."""
    if cp < 0x80:
        return bytes([cp])
    elif cp < 0x800:
        return bytes([0xC0 | (cp >> 6), 0x80 | (cp & 0x3F)])
    elif cp < 0x10000:
        return bytes([0xE0 | (cp >> 12), 0x80 | ((cp >> 6) & 0x3F), 0x80 | (cp & 0x3F)])
    else:
        return bytes(
            [
                0xF0 | (cp >> 18),
                0x80 | ((cp >> 12) & 0x3F),
                0x80 | ((cp >> 6) & 0x3F),
                0x80 | (cp & 0x3F),
            ]
        )


def codepoints_to_utf8_bytes(codepoints: list) -> bytes:
    """Convert codepoints to UTF-8 bytes."""
    result = b""
    for cp in codepoints:
        result += codepoint_to_utf8_bytes(cp)
    return result


def bytes_to_cpp_string_literal(data: bytes) -> str:
    """Convert bytes to a C++ UTF-8 string literal."""
    return (
        'std::string("'
        + "".join(f"\\x{byte:02X}" for byte in data)
        + f'", {len(data)})'
    )


def segments_to_cpp_string_vector(segments: list) -> str:
    """Convert segments to C++ vector<string> initializer."""
    literals = [
        bytes_to_cpp_string_literal(codepoints_to_utf8_bytes(seg)) for seg in segments
    ]
    return "{" + ", ".join(literals) + "}"


def main():
    test_cases = []

    with open(DATA_DIR / "SentenceBreakTest.txt", encoding="utf-8") as f:
        for line_no, line in enumerate(f, 1):
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            codepoints, break_positions, comment = parse_test_line(line)
            if not codepoints:
                continue

            segments = generate_expected_segments(codepoints, break_positions)
            test_cases.append((line_no, codepoints, segments, comment))

    # Generate C++ test file
    cpp_code = """#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include "setence_break.h"

using namespace sentence_break;

namespace {

struct TestCase {
    int line_no;
    std::vector<std::int32_t> input;
    std::vector<std::vector<std::int32_t>> expected;
    const char* comment;
};

class SentenceBreakTest : public ::testing::TestWithParam<TestCase> {};

TEST_P(SentenceBreakTest, Segmentation) {
    const auto& [line_no, input, expected, comment] = GetParam();
    const auto result = segmentSentences(input);
    EXPECT_EQ(result, expected) << "Failed at line " << line_no << ": " << comment;
}

const TestCase test_cases[] = {
"""

    for line_no, codepoints, segments, comment in test_cases:
        input_str = codepoints_to_cpp_vector(codepoints)
        expected_str = segments_to_cpp_vector(segments)
        comment_escaped = escape_cpp_string(comment)
        cpp_code += (
            f'    {{{line_no}, {input_str}, {expected_str}, "{comment_escaped}"}},\n'
        )

    cpp_code += """};

INSTANTIATE_TEST_SUITE_P(
    AllCases,
    SentenceBreakTest,
    ::testing::ValuesIn(test_cases)
);

}
"""

    with open(TESTS_DIR / "test_sentence_break_all.cpp", "w", encoding="utf-8") as f:
        f.write(cpp_code)

    print(f"Generated {len(test_cases)} test cases (codepoints)")

    # Generate UTF-8 string test file
    cpp_code_utf8 = """#include <gtest/gtest.h>
#include <vector>
#include <string>
#include "sentence_break.h"

using namespace sentence_break;

namespace {

struct TestCase {
    int line_no;
    std::string input;
    std::vector<std::string> expected;
    const char* comment;
};

class SentenceBreakUtf8Test : public ::testing::TestWithParam<TestCase> {};

TEST_P(SentenceBreakUtf8Test, Segmentation) {
    const auto& [line_no, input, expected, comment] = GetParam();
    const auto result = segmentSentences(input);
    EXPECT_EQ(result, expected) << "Failed at line " << line_no << ": " << comment;
}

const TestCase test_cases[] = {
"""

    for line_no, codepoints, segments, comment in test_cases:
        input_str = bytes_to_cpp_string_literal(codepoints_to_utf8_bytes(codepoints))
        expected_str = segments_to_cpp_string_vector(segments)
        comment_escaped = escape_cpp_string(comment)
        cpp_code_utf8 += (
            f'    {{{line_no}, {input_str}, {expected_str}, "{comment_escaped}"}},\n'
        )

    cpp_code_utf8 += """};

INSTANTIATE_TEST_SUITE_P(
    AllCases,
    SentenceBreakUtf8Test,
    ::testing::ValuesIn(test_cases)
);

}
"""

    with open(
        TESTS_DIR / "test_sentence_break_all_utf8.cpp", "w", encoding="utf-8"
    ) as f:
        f.write(cpp_code_utf8)

    print(f"Generated {len(test_cases)} test cases (utf8)")


if __name__ == "__main__":
    main()
