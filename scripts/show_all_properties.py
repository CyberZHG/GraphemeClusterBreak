from pathlib import Path

DATA_DIR = Path(__file__).parent.parent / "ucd"

FILES = [
    "GraphemeBreakProperty.txt",
    "WordBreakProperty.txt",
    "SentenceBreakProperty.txt",
    "emoji-data.txt",
    "DerivedCoreProperties.txt",
]


def show_properties(filename: str):
    filepath = DATA_DIR / filename
    properties = set()
    with open(filepath) as f:
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue
            properties.add(line.split(";", 1)[-1].split("#")[0].strip())
    print(filename)
    print(properties)


def main():
    for filename in FILES:
        show_properties(filename)


if __name__ == "__main__":
    main()
