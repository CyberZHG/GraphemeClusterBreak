import { describe, it } from "mocha";
import assert from "node:assert";
import { segmentGraphemeClusters } from "../index.js";

describe("Basic", () => {
    it("should segment ASCII and CJK characters", () => {
        assert.deepStrictEqual(segmentGraphemeClusters("Hello world!"), [
            "H", "e", "l", "l", "o", " ", "w", "o", "r", "l", "d", "!",
        ]);
        assert.deepStrictEqual(segmentGraphemeClusters("你好世界"), ["你", "好", "世", "界"]);
    });
});

describe("GB3: CRLF", () => {
    it("should not break between CR and LF", () => {
        assert.deepStrictEqual(segmentGraphemeClusters("\r\n"), ["\r\n"]);
        assert.deepStrictEqual(segmentGraphemeClusters("a\r\nb"), ["a", "\r\n", "b"]);
        assert.deepStrictEqual(segmentGraphemeClusters("\r\n\r\n"), ["\r\n", "\r\n"]);
    });
});

describe("GB4/GB5: Control", () => {
    it("should break before and after Control, CR, LF (except GB3)", () => {
        assert.deepStrictEqual(segmentGraphemeClusters("a\nb"), ["a", "\n", "b"]);
        assert.deepStrictEqual(segmentGraphemeClusters("a\rb"), ["a", "\r", "b"]);
        assert.deepStrictEqual(segmentGraphemeClusters("a\x00b"), ["a", "\x00", "b"]);
    });
});

describe("GB6/GB7/GB8: Hangul", () => {
    it("should handle Hangul syllable sequences", () => {
        // L × L
        assert.deepStrictEqual(segmentGraphemeClusters("\u1100\u1100"), ["\u1100\u1100"]);
        // L × V
        assert.deepStrictEqual(segmentGraphemeClusters("\u1100\u1161"), ["\u1100\u1161"]);
        // L × LV
        assert.deepStrictEqual(segmentGraphemeClusters("\u1100\uAC00"), ["\u1100\uAC00"]);
        // L × LVT
        assert.deepStrictEqual(segmentGraphemeClusters("\u1100\uAC01"), ["\u1100\uAC01"]);
        // LV × V
        assert.deepStrictEqual(segmentGraphemeClusters("\uAC00\u1161"), ["\uAC00\u1161"]);
        // LV × T
        assert.deepStrictEqual(segmentGraphemeClusters("\uAC00\u11A8"), ["\uAC00\u11A8"]);
        // V × V
        assert.deepStrictEqual(segmentGraphemeClusters("\u1161\u1161"), ["\u1161\u1161"]);
        // V × T
        assert.deepStrictEqual(segmentGraphemeClusters("\u1161\u11A8"), ["\u1161\u11A8"]);
        // LVT × T
        assert.deepStrictEqual(segmentGraphemeClusters("\uAC01\u11A8"), ["\uAC01\u11A8"]);
        // T × T
        assert.deepStrictEqual(segmentGraphemeClusters("\u11A8\u11A8"), ["\u11A8\u11A8"]);
        // Precomposed Hangul syllables
        assert.deepStrictEqual(segmentGraphemeClusters("한글"), ["한", "글"]);
    });
});

describe("GB9: Extend", () => {
    it("should not break before Extend or ZWJ", () => {
        // Combining acute accent (Extend)
        assert.deepStrictEqual(segmentGraphemeClusters("e\u0301"), ["e\u0301"]);
        // Multiple combining marks
        assert.deepStrictEqual(segmentGraphemeClusters("e\u0301\u0327"), ["e\u0301\u0327"]);
        // Devanagari vowel sign (Extend)
        assert.deepStrictEqual(segmentGraphemeClusters("\u0915\u093E"), ["\u0915\u093E"]);
    });
});

describe("GB9a: SpacingMark", () => {
    it("should not break before SpacingMark", () => {
        // Tamil vowel sign
        assert.deepStrictEqual(segmentGraphemeClusters("\u0B95\u0BBE"), ["\u0B95\u0BBE"]);
        // Bengali vowel sign
        assert.deepStrictEqual(segmentGraphemeClusters("\u0995\u09BE"), ["\u0995\u09BE"]);
    });
});

describe("GB9b: Prepend", () => {
    it("should not break after Prepend", () => {
        // Arabic number sign (Prepend)
        assert.deepStrictEqual(segmentGraphemeClusters("\u0600\u0031"), ["\u0600\u0031"]);
    });
});

describe("GB9c: Indic Conjunct", () => {
    it("should not break within Indic conjunct clusters", () => {
        // Devanagari: क + ् + ष = क्ष (ksha)
        assert.deepStrictEqual(segmentGraphemeClusters("\u0915\u094D\u0937"), ["\u0915\u094D\u0937"]);
        // Devanagari: Multiple conjuncts
        assert.deepStrictEqual(segmentGraphemeClusters("\u0915\u094D\u0937\u094D\u0923"), ["\u0915\u094D\u0937\u094D\u0923"]);
        // Bengali conjunct: ক + ্ + ক
        assert.deepStrictEqual(segmentGraphemeClusters("\u0995\u09CD\u0995"), ["\u0995\u09CD\u0995"]);
    });
});

describe("GB11: Emoji ZWJ", () => {
    it("should not break within Emoji ZWJ sequences", () => {
        // Family emoji (ZWJ sequence)
        assert.deepStrictEqual(segmentGraphemeClusters("👨‍👩‍👧‍👦"), ["👨‍👩‍👧‍👦"]);
        // Man + ZWJ + laptop
        assert.deepStrictEqual(segmentGraphemeClusters("👨‍💻"), ["👨‍💻"]);
        // Woman + ZWJ + heart + ZWJ + man
        assert.deepStrictEqual(segmentGraphemeClusters("👩‍❤️‍👨"), ["👩‍❤️‍👨"]);
        // Rainbow flag
        assert.deepStrictEqual(segmentGraphemeClusters("🏳️‍🌈"), ["🏳️‍🌈"]);
    });
});

describe("GB12/GB13: Regional Indicator", () => {
    it("should pair Regional Indicators correctly", () => {
        // Single flag (two RI)
        assert.deepStrictEqual(segmentGraphemeClusters("🇨🇳"), ["🇨🇳"]);
        // Two flags (four RI, should pair up)
        assert.deepStrictEqual(segmentGraphemeClusters("🇨🇳🇺🇸"), ["🇨🇳", "🇺🇸"]);
        // Three RI (first two pair, third alone)
        assert.deepStrictEqual(segmentGraphemeClusters("🇨🇳🇺"), ["🇨🇳", "🇺"]);
        // Four flags
        assert.deepStrictEqual(segmentGraphemeClusters("🇨🇳🇺🇸🇯🇵🇬🇧"), ["🇨🇳", "🇺🇸", "🇯🇵", "🇬🇧"]);
    });
});

describe("Emoji Modifiers", () => {
    it("should handle emoji with skin tone modifiers", () => {
        // Person with skin tone
        assert.deepStrictEqual(segmentGraphemeClusters("👋🏽"), ["👋🏽"]);
        // Multiple people with skin tones
        assert.deepStrictEqual(segmentGraphemeClusters("👋🏻👋🏿"), ["👋🏻", "👋🏿"]);
    });
});

describe("Edge Cases", () => {
    it("should handle empty string", () => {
        assert.deepStrictEqual(segmentGraphemeClusters(""), []);
    });

    it("should handle single characters", () => {
        assert.deepStrictEqual(segmentGraphemeClusters("a"), ["a"]);
        assert.deepStrictEqual(segmentGraphemeClusters("中"), ["中"]);
        assert.deepStrictEqual(segmentGraphemeClusters("🎉"), ["🎉"]);
    });
});
