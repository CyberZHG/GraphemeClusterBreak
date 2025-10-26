import createModule from "./wasm/GraphemeClusterBreakWASM.js";

const GraphemeClusterBreakWASM = await createModule();

/**
 * Segment a UTF-8 string into grapheme clusters.
 *
 * This function breaks a UTF-8 encoded string into user-perceived characters
 * (grapheme clusters) according to Unicode Text Segmentation (UAX #29).
 *
 * @param {string} s - The input string to segment.
 * @param {boolean} [extended=true] - If true, use extended grapheme cluster rules.
 * @returns {string[]} An array of strings, each representing one grapheme cluster.
 */
export function segmentGraphemeClusters(s, extended = true) {
    const result = GraphemeClusterBreakWASM._segmentGraphemeClusters(s, extended);
    const arr = [];
    for (let i = 0; i < result.size(); i++) {
        arr.push(result.get(i));
    }
    result.delete();
    return arr;
}
