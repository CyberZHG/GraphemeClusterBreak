from pathlib import Path

DATA_DIR = Path(__file__).parent.parent / "ucd"

properties = []

def add_to_properties(unicode_range: str, break_property: str):
    if ".." in unicode_range:
        start, end = unicode_range.split("..")
        start, end = int(start, 16), int(end, 16)
        properties.append((start, end, break_property))
    else:
        unicode = int(unicode_range, 16)
        properties.append((unicode, unicode, break_property))


def compress_properties():
    global properties
    properties.sort()
    print(f"# Properties: {len(properties)}")
    n, m = len(properties), 1
    for i in range(1, n):
        if properties[m - 1][1] + 1 == properties[i][0] and properties[m - 1][2] == properties[i][2]:
            properties[m - 1] = (properties[m - 1][0], properties[i][1], properties[m - 1][2])
        else:
            properties[m] = properties[i]
            m += 1
    properties = properties[:m]
    print(f"# Properties (compressed): {len(properties)}")


def generate_codes():
    global properties
    codes = "static const std::int32_t GRAPHEME_CLUSTER_BREAK_RANGES[] = {\n"
    for i, (start, end, _) in enumerate(properties):
        if i != 0 and i % 5 == 0 and i + 1 != len(properties):
            codes += "\n"
        if i % 5 == 0:
            codes += "    "
        codes += f"0x{start:04X}, 0x{end:04X}, "
    codes += "\n};\n\n"
    codes += "static const GraphemeClusterBreakProperty GRAPHEME_CLUSTER_BREAK_PROPERTIES[] = {\n"
    for i, (_, _, break_property) in enumerate(properties):
        if i != 0 and i % 5 == 0 and i + 1 != len(properties):
            codes += "\n"
        if i % 5 == 0:
            codes += "    "
        codes += f"{break_property}, "
    codes += "\n};\n"
    with open(DATA_DIR / "_properties.cpp", "w") as f:
        f.write(codes)

def main():
    with open(DATA_DIR / "GraphemeBreakProperty.txt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            unicode_range, break_property = line.split(";", 1)
            unicode_range = unicode_range.strip()
            break_property = break_property.split("#")[0].strip()
            add_to_properties(unicode_range, break_property)
    with open(DATA_DIR / "DerivedCoreProperties.txt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            unicode_range, break_property = line.split(";", 1)
            unicode_range = unicode_range.strip()
            break_property = break_property.split("#")[0].strip()
            if not break_property.startswith("InCB"):
                continue
            break_property = break_property.replace("; ", "_")
            add_to_properties(unicode_range, break_property)
    with open(DATA_DIR / "emoji-data.txt") as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            unicode_range, break_property = line.split(";", 1)
            unicode_range = unicode_range.strip()
            break_property = break_property.split("#")[0].strip()
            if break_property != "Extended_Pictographic":
                continue
            add_to_properties(unicode_range, break_property)
    compress_properties()
    generate_codes()


if __name__ == "__main__":
    main()
