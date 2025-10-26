from grapheme_cluster_break import segment_grapheme_clusters


def test_special_cases_1():
    s = "Hello world!"
    segmented = segment_grapheme_clusters(s)
    expected = ["H", "e", "l", "l", "o", " ", "w", "o", "r", "l", "d", "!"]
    assert segmented == expected
    s = "你好世界"
    segmented = segment_grapheme_clusters(s)
    expected = ["你", "好", "世", "界"]
    assert segmented == expected
    s = "🇨🇳🇨🇳"
    segmented = segment_grapheme_clusters(s)
    expected = ["🇨🇳", "🇨🇳"]
    assert segmented == expected
