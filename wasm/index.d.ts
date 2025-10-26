/**
 * Segment a UTF-8 string into grapheme clusters.
 *
 * This function breaks a UTF-8 encoded string into user-perceived characters
 * (grapheme clusters) according to Unicode Text Segmentation (UAX #29).
 *
 * @param s - The input string to segment.
 * @param extended - If true, use extended grapheme cluster rules (default: true).
 * @returns An array of strings, each representing one grapheme cluster.
 *
 * @example
 * ```typescript
 * import { init, segmentGraphemeClusters } from "grapheme-cluster-wasm";
 *
 * await init();
 * segmentGraphemeClusters("hello"); // ["h", "e", "l", "l", "o"]
 * segmentGraphemeClusters("👨‍👩‍👧‍👦"); // ["👨‍👩‍👧‍👦"]
 * ```
 */
export function segmentGraphemeClusters(s: string, extended?: boolean): string[];
