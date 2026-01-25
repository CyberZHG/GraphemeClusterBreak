import urllib.request
from pathlib import Path

UNICODE_VERSION = "17.0.0"
BASE_URL = f"https://www.unicode.org/Public/{UNICODE_VERSION}/ucd/"
OUTPUT_DIR = Path(__file__).parent.parent / "ucd"

FILES = [
    "auxiliary/GraphemeBreakProperty.txt",
    "auxiliary/GraphemeBreakTest.txt",
    "auxiliary/WordBreakProperty.txt",
    "auxiliary/WordBreakTest.txt",
    "emoji/emoji-data.txt",
    "DerivedCoreProperties.txt",
]

def download_file(filename: str):
    url = BASE_URL + filename
    output_path = OUTPUT_DIR / filename.split("/")[-1]

    print(f"Downloading {filename}...")
    try:
        urllib.request.urlretrieve(url, output_path)
        print(f"  Saved to {output_path}")
    except Exception as e:
        print(f"  Error: {e}")

def main():
    if not OUTPUT_DIR.exists():
        OUTPUT_DIR.mkdir(exist_ok=True)
    for filename in FILES:
        download_file(filename)

if __name__ == "__main__":
    main()
