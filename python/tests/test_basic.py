from grapheme_cluster_break import segment_grapheme_clusters


def test_basic():
    """Test basic ASCII and CJK characters."""
    assert segment_grapheme_clusters("Hello world!") == [
        "H",
        "e",
        "l",
        "l",
        "o",
        " ",
        "w",
        "o",
        "r",
        "l",
        "d",
        "!",
    ]
    assert segment_grapheme_clusters("你好世界") == ["你", "好", "世", "界"]


def test_gb3_crlf():
    """GB3: CR × LF - Do not break between CR and LF."""
    assert segment_grapheme_clusters("\r\n") == ["\r\n"]
    assert segment_grapheme_clusters("a\r\nb") == ["a", "\r\n", "b"]
    assert segment_grapheme_clusters("\r\n\r\n") == ["\r\n", "\r\n"]


def test_gb4_gb5_control():
    """GB4/GB5: Break before and after Control, CR, LF (except GB3)."""
    assert segment_grapheme_clusters("a\nb") == ["a", "\n", "b"]
    assert segment_grapheme_clusters("a\rb") == ["a", "\r", "b"]
    assert segment_grapheme_clusters("a\x00b") == ["a", "\x00", "b"]  # NUL control


def test_gb6_gb7_gb8_hangul():
    """GB6/GB7/GB8: Hangul syllable sequences."""
    # L × L
    assert segment_grapheme_clusters("\u1100\u1100") == ["\u1100\u1100"]  # ᄀᄀ (L + L)
    # L × V
    assert segment_grapheme_clusters("\u1100\u1161") == ["\u1100\u1161"]  # 가 (L + V)
    # L × LV
    assert segment_grapheme_clusters("\u1100\uAC00") == [
        "\u1100\uAC00"
    ]  # ᄀ가 (L + LV)
    # L × LVT
    assert segment_grapheme_clusters("\u1100\uAC01") == [
        "\u1100\uAC01"
    ]  # ᄀ각 (L + LVT)
    # LV × V
    assert segment_grapheme_clusters("\uAC00\u1161") == ["\uAC00\u1161"]  # 가ᅡ (LV + V)
    # LV × T
    assert segment_grapheme_clusters("\uAC00\u11A8") == ["\uAC00\u11A8"]  # 각 (LV + T)
    # V × V
    assert segment_grapheme_clusters("\u1161\u1161") == ["\u1161\u1161"]  # ᅡᅡ (V + V)
    # V × T
    assert segment_grapheme_clusters("\u1161\u11A8") == ["\u1161\u11A8"]  # ᅡᆨ (V + T)
    # LVT × T
    assert segment_grapheme_clusters("\uAC01\u11A8") == [
        "\uAC01\u11A8"
    ]  # 각ᆨ (LVT + T)
    # T × T
    assert segment_grapheme_clusters("\u11A8\u11A8") == ["\u11A8\u11A8"]  # ᆨᆨ (T + T)
    # Precomposed Hangul syllables
    assert segment_grapheme_clusters("한글") == ["한", "글"]


def test_gb9_extend():
    """GB9: Do not break before Extend or ZWJ."""
    # Combining acute accent (Extend)
    assert segment_grapheme_clusters("e\u0301") == ["e\u0301"]  # é
    assert segment_grapheme_clusters("e\u0301\u0327") == [
        "e\u0301\u0327"
    ]  # Multiple combining marks
    # Devanagari vowel sign (Extend)
    assert segment_grapheme_clusters("\u0915\u093E") == ["\u0915\u093E"]  # का


def test_gb9a_spacing_mark():
    """GB9a: Do not break before SpacingMark."""
    # Tamil vowel sign
    assert segment_grapheme_clusters("\u0B95\u0BBE") == ["\u0B95\u0BBE"]  # கா
    # Bengali vowel sign
    assert segment_grapheme_clusters("\u0995\u09BE") == ["\u0995\u09BE"]  # কা


def test_gb9b_prepend():
    """GB9b: Do not break after Prepend."""
    # Arabic number sign (Prepend)
    assert segment_grapheme_clusters("\u0600\u0031") == ["\u0600\u0031"]  # ؀1


def test_gb9c_indic_conjunct():
    """GB9c: Do not break within Indic conjunct clusters (Consonant + Virama + Consonant)."""
    # Devanagari: क + ् + ष = क्ष (ksha)
    assert segment_grapheme_clusters("\u0915\u094D\u0937") == ["\u0915\u094D\u0937"]
    # Devanagari: Multiple conjuncts
    assert segment_grapheme_clusters("\u0915\u094D\u0937\u094D\u0923") == [
        "\u0915\u094D\u0937\u094D\u0923"
    ]
    # Bengali conjunct: ক + ্ + ক
    assert segment_grapheme_clusters("\u0995\u09CD\u0995") == ["\u0995\u09CD\u0995"]


def test_gb11_emoji_zwj():
    """GB11: Do not break within Emoji ZWJ sequences."""
    # Family emoji (ZWJ sequence)
    assert segment_grapheme_clusters("👨‍👩‍👧‍👦") == ["👨‍👩‍👧‍👦"]
    # Man + ZWJ + laptop
    assert segment_grapheme_clusters("👨‍💻") == ["👨‍💻"]
    # Woman + ZWJ + heart + ZWJ + man
    assert segment_grapheme_clusters("👩‍❤️‍👨") == ["👩‍❤️‍👨"]
    # Flag in rainbow
    assert segment_grapheme_clusters("🏳️‍🌈") == ["🏳️‍🌈"]


def test_gb12_gb13_regional_indicator():
    """GB12/GB13: Regional Indicator pairs."""
    # Single flag (two RI)
    assert segment_grapheme_clusters("🇨🇳") == ["🇨🇳"]
    # Two flags (four RI, should pair up)
    assert segment_grapheme_clusters("🇨🇳🇺🇸") == ["🇨🇳", "🇺🇸"]
    # Three RI (first two pair, third alone)
    assert segment_grapheme_clusters("🇨🇳🇺") == ["🇨🇳", "🇺"]
    # Four flags
    assert segment_grapheme_clusters("🇨🇳🇺🇸🇯🇵🇬🇧") == ["🇨🇳", "🇺🇸", "🇯🇵", "🇬🇧"]


def test_emoji_modifiers():
    """Test emoji with skin tone modifiers (Extend)."""
    # Person with skin tone
    assert segment_grapheme_clusters("👋🏽") == ["👋🏽"]
    # Multiple people with skin tones
    assert segment_grapheme_clusters("👋🏻👋🏿") == ["👋🏻", "👋🏿"]


def test_empty_string():
    """Test empty string."""
    assert segment_grapheme_clusters("") == []


def test_single_character():
    """Test single characters."""
    assert segment_grapheme_clusters("a") == ["a"]
    assert segment_grapheme_clusters("中") == ["中"]
    assert segment_grapheme_clusters("🎉") == ["🎉"]
