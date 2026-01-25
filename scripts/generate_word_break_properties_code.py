from pathlib import Path

DATA_DIR = Path(__file__).parent.parent / "ucd"

word_break_properties = []


def parse_unicode_range(unicode_range: str):
    if ".." in unicode_range:
        start, end = unicode_range.split("..")
        return int(start, 16), int(end, 16)
    else:
        unicode = int(unicode_range, 16)
        return unicode, unicode


def add_to_word_break_properties(unicode_range: str, break_property: str):
    start, end = parse_unicode_range(unicode_range)
    word_break_properties.append((start, end, break_property))


def compress_properties(properties, name):
    properties.sort()
    for i in range(len(properties) - 1):
        if properties[i][1] >= properties[i + 1][0]:
            print(properties[i])
            print(properties[i + 1])
            print("")
    print(f"# {name}: {len(properties)}")
    n, m = len(properties), 1
    for i in range(1, n):
        if (
            properties[m - 1][1] + 1 == properties[i][0]
            and properties[m - 1][2] == properties[i][2]
        ):
            properties[m - 1] = (
                properties[m - 1][0],
                properties[i][1],
                properties[m - 1][2],
            )
        else:
            properties[m] = properties[i]
            m += 1
    del properties[m:]
    print(f"# {name} (compressed): {len(properties)}")


def generate_codes():
    codes = ""

    # Word Break Properties
    codes += f"    static constexpr int NUM_WORD_BREAK_RANGES = {len(word_break_properties)};\n\n"
    codes += "    static const std::int32_t WORD_BREAK_RANGES[] = {\n"
    for i, (start, end, _) in enumerate(word_break_properties):
        if i != 0 and i % 5 == 0:
            codes += "\n"
        if i % 5 == 0:
            codes += "        "
        codes += f"0x{start:04X}, 0x{end:04X}, "
    codes += "\n    };\n\n"
    codes += "    static const WordBreakProperty WORD_BREAK_PROPERTIES[] = {\n"
    for i, (_, _, prop) in enumerate(word_break_properties):
        if i != 0 and i % 5 == 0:
            codes += "\n"
        if i % 5 == 0:
            codes += "        "
        codes += f"{prop}, "
    codes += "\n    };\n"

    with open(DATA_DIR / "_word_break_properties.cpp", "w") as f:
        f.write(codes)


def main():
    # Parse WordBreakProperty.txt
    with open(DATA_DIR / "WordBreakProperty.txt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            unicode_range, prop = line.split(";", 1)
            unicode_range = unicode_range.strip()
            prop = prop.split("#")[0].strip()
            add_to_word_break_properties(unicode_range, prop)

    compress_properties(word_break_properties, "Word Break Properties")
    generate_codes()


if __name__ == "__main__":
    main()
