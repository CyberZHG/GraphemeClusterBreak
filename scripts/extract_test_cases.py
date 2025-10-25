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


def main():
    test_cases = []

    with open(DATA_DIR / "GraphemeBreakTest.txt", encoding="utf-8") as f:
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
    cpp_code = '''#include <gtest/gtest.h>
#include <vector>
#include <cstdint>
#include "grapheme_cluster.h"

using namespace grapheme_cluster;

namespace {

struct TestCase {
    int line_no;
    std::vector<std::int32_t> input;
    std::vector<std::vector<std::int32_t>> expected;
    const char* comment;
};

class GraphemeBreakTest : public ::testing::TestWithParam<TestCase> {};

TEST_P(GraphemeBreakTest, Segmentation) {
    const auto& tc = GetParam();
    auto result = segmentGraphemeClusters(tc.input);
    EXPECT_EQ(result, tc.expected)
        << "Failed at line " << tc.line_no << ": " << tc.comment;
}

const TestCase test_cases[] = {
'''

    for line_no, codepoints, segments, comment in test_cases:
        input_str = codepoints_to_cpp_vector(codepoints)
        expected_str = segments_to_cpp_vector(segments)
        comment_escaped = escape_cpp_string(comment)
        cpp_code += f'    {{{line_no}, {input_str}, {expected_str}, "{comment_escaped}"}},\n'

    cpp_code += '''};

INSTANTIATE_TEST_SUITE_P(
    AllCases,
    GraphemeBreakTest,
    ::testing::ValuesIn(test_cases)
);

}  // namespace
'''

    with open(TESTS_DIR / "test_grapheme_break_test_all.cpp", "w", encoding="utf-8") as f:
        f.write(cpp_code)

    print(f"Generated {len(test_cases)} test cases")


if __name__ == "__main__":
    main()
