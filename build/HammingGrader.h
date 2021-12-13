#ifndef HAMMINGGRADER_H
#define HAMMINGGRADER_H
#include <stdio.h>
#include <inttypes.h>

/**  Generates a random chunk of 64 data bits and checks whether the student
 *   implementation of set_Hamming72_64() yields a correct encoding.
 * 
 *   Pre:
 *     log is open on an output file.
 */
void runGenerationTest(FILE* log);

/**  Generates a random chunk of 64 data bits, and a correct Hamming
 *   encoding, then flips bits of the correct encoding, and checks whether
 *   the student implementation of correct_Hamming72_64() performs a
 *   correct analysis of the flaws in the resulting damaged encoding.
 * 
 * Pre:
 *    log is open on an output file.
 *    mPos1 and mPos2 are negative or in the range [0 , 71]
 * 
 * Test cases:
 *    mPos1 < 0 and mPos2 < 0:   no bits are flipped
 *    mPos1 >= 0 and mPos2 < 0:  flip bit at mPos1
 *    mPos1 >= 0 and mPos2 >= 0: flip bits at mPos1 and mPos2
 */
void runCorrectionTest(FILE* log, int8_t mPos1, int8_t mPos2);

/**  Returns the maximum score for the generation tests.
 */
unsigned int maxForGeneration();

/**  Returns the maximum score for the correction tests.
 */
unsigned int maxForCorrection();

#endif
