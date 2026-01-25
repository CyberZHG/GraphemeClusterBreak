from pathlib import Path

DATA_DIR = Path(__file__).parent.parent / "ucd"

break_properties = []
incb_properties = []


def parse_unicode_range(unicode_range: str):
    if ".." in unicode_range:
        start, end = unicode_range.split("..")
        return int(start, 16), int(end, 16)
    else:
        unicode = int(unicode_range, 16)
        return unicode, unicode


def add_to_break_properties(unicode_range: str, break_property: str):
    start, end = parse_unicode_range(unicode_range)
    break_properties.append((start, end, break_property))


def add_to_incb_properties(unicode_range: str, break_property: str):
    start, end = parse_unicode_range(unicode_range)
    incb_properties.append((start, end, break_property))


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

    # Grapheme Cluster Break Properties
    codes += f"    static constexpr int NUM_GRAPHEME_CLUSTER_BREAK_RANGES = {len(break_properties)};\n\n"
    codes += "    static const std::int32_t GRAPHEME_CLUSTER_BREAK_RANGES[] = {\n"
    for i, (start, end, _) in enumerate(break_properties):
        if i != 0 and i % 5 == 0:
            codes += "\n"
        if i % 5 == 0:
            codes += "        "
        codes += f"0x{start:04X}, 0x{end:04X}, "
    codes += "\n    };\n\n"
    codes += "    static const GraphemeClusterBreakProperty GRAPHEME_CLUSTER_BREAK_PROPERTIES[] = {\n"
    for i, (_, _, prop) in enumerate(break_properties):
        if i != 0 and i % 5 == 0:
            codes += "\n"
        if i % 5 == 0:
            codes += "        "
        codes += f"{prop}, "
    codes += "\n    };\n\n"

    # Indic Conjunct Break Properties
    codes += f"    static constexpr int NUM_INDIC_CONJUNCT_BREAK_RANGES = {len(incb_properties)};\n\n"
    codes += "    static const std::int32_t INDIC_CONJUNCT_BREAK_RANGES[] = {\n"
    for i, (start, end, _) in enumerate(incb_properties):
        if i != 0 and i % 5 == 0:
            codes += "\n"
        if i % 5 == 0:
            codes += "        "
        codes += f"0x{start:04X}, 0x{end:04X}, "
    codes += "\n    };\n\n"
    codes += "    static const IndicConjunctBreakProperty INDIC_CONJUNCT_BREAK_PROPERTIES[] = {\n"
    for i, (_, _, prop) in enumerate(incb_properties):
        if i != 0 and i % 5 == 0:
            codes += "\n"
        if i % 5 == 0:
            codes += "        "
        codes += f"{prop}, "
    codes += "\n    };\n"

    with open(DATA_DIR / "_grapheme_break_properties.cpp", "w") as f:
        f.write(codes)


def main():
    # Parse GraphemeBreakProperty.txt
    with open(DATA_DIR / "GraphemeBreakProperty.txt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            unicode_range, prop = line.split(";", 1)
            unicode_range = unicode_range.strip()
            prop = prop.split("#")[0].strip()
            add_to_break_properties(unicode_range, prop)

    # Parse DerivedCoreProperties.txt for InCB properties
    with open(DATA_DIR / "DerivedCoreProperties.txt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            unicode_range, prop = line.split(";", 1)
            unicode_range = unicode_range.strip()
            prop = prop.split("#")[0].strip()
            if not prop.startswith("InCB"):
                continue
            prop = prop.replace("; ", "_")
            add_to_incb_properties(unicode_range, prop)

    compress_properties(break_properties, "Break Properties")
    compress_properties(incb_properties, "InCB Properties")
    generate_codes()


if __name__ == "__main__":
    main()
