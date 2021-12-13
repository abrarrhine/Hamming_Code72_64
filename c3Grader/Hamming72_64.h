#ifndef HAMMING72_64_H
#define HAMMING72_64_H
#include <stdio.h>
#include <inttypes.h>

/**  Library implementation for the Hamming 72-64 data encoding.
 *   
 *   Since the C Standard Library does not include an appropriate type for
 *   representing 72-bit chunks of data, we employ an array of 9 bytes.  The
 *   array is wrapped inside a struct so that it is possible to use an
 *   encoding as the return value from a function.
 * 
 *   Conceptually, the array is viewed as a monolothic chunk of bits.  Suppose
 *   we have 8 bytes of data, which look like this when printed:
 * 
 *      byte:   0   1   2   3   4   5   6   7
 *      data:  62  4E  F9  FE  7A  CC  E5  7B
 *
 *   The bits of the Hamming encoding are stored in the array in ascending order:
 * 
 *   Bit positions:
 *         Lo                                                                              Hi
 *          00000000 00111111 11112222 22222233 33333333 44444444 44555555 55556666 66666677
 *          01234567 89012345 67890123 45678901 23456789 01234567 89012345 67890123 45678901
 *          --------------------------------------------------------------------------------
 * Data:    01000110 01110010 10011111 01111111 01011110 00110011 10100111 11011110
 * Encoded: 00001100 00110011 11001010 01111101 11111110 10111100 01100111 01001111 11011110
 * Parity:  ^^^ ^    ^        ^                 ^                                   ^       
 *
 *   If the bit-ordering is confusing, remember that numeric values print in big-endian order;
 *   so, the first byte of data prints as "62", while the low nybble is 0010 and the high
 *   nybble is 0110.  The display of the bits, on the other hand, is purely little-endian (at
 *   the bit level within a byte), so the bits seem to be backwards, but the order shown is
 *   actually the natural ordering that would occur in little-endian hardware.
 */

#define NBYTES 9       // Total number of bytes in an encoding (data + parity)

//  Enumerated and struct types for reporting analysis of Hamming72_64
//   objects.
 
enum _Error {MASTERPARITY, SINGLEPARITY, SINGLEDATA, MULTIBIT, NOERROR};
typedef enum _Error Error;

struct _Report_Hamming72_64 {
	Error   status;
	uint8_t syndrome;
};
typedef struct _Report_Hamming72_64 Report_Hamming72_64;


// Struct type for representing a Hamming (72, 64) encoding.
struct _Hamming72_64 {

   uint8_t bits[NBYTES];       // 72-bit data chunk
};
typedef struct _Hamming72_64 Hamming72_64;


/**  Create the Hamming encoding for a sequence of 64 data bits.
 * 
 *   Pre:  *pH is a Hamming72_64 object
 *         *pbits is a block of 64 bits
 *   Post: pH->bits has been initialized to store the given data bits and
 *            corresponding parity bits, as described in the specification
 *            of the Hamming (72,64) scheme.
 */
void  set_Hamming72_64(Hamming72_64* const pH, const uint8_t* const pBits);

/**  Checks the validity of the given Hamming encoding, and corrects errors
 *   when possible.
 * 
 *   Pre:  *pH is initialized
 *   Post: If correctable errors are found, they are corrected in pH->bits.
 *   Returns:  A Report_Hamming72_64 object holding the computed syndrome
 *             and flag indicating what type of error, if any, was found.
 */
Report_Hamming72_64 correct_Hamming72_64(Hamming72_64* const pH);

#endif
