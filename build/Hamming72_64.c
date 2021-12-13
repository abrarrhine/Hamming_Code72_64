// On my honor:
//
// - I have not discussed the C language code in my program with
// anyone other than my instructor or the teaching assistants 
// assigned to this course.
//
// - I have not used C language code obtained from another student, 
// the Internet, or any other unauthorized source, either modified
// or unmodified. 
//
// - If any C language code or documentation used in my program 
// was obtained from an authorized source, such as a text book or
// course notes, that has been clearly noted with a proper citation
// in the comments of my program.
//
// - I have not designed this program in such a way as to defeat or
// interfere with the normal operation of the grading code.
//
// <Abrar Islam>
// <abrarr18>

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include "Hamming72_64.h"

/**********************************************************************/
/*     PLACE DECLARATIONS OF STATIC HELPER FUNCTIONS BELOW            */
/**********************************************************************/

char* intToBin(uint8_t integer);
uint8_t binToInt(char* bin);
void encodeParityIntoData(char* encoded, char* originalBits);
int powerOfTwo(int number, int power);
void putParityBits(char* hamming7264, char* pBits);
void getParityBits(char* hamming7264, char* pBits);
char calBits(char* hamming7264, uint8_t* integer);
uint8_t detectError(char* wholebits, char* hamming7264, uint8_t* syndrome); 
void putDataBits(uint8_t* dataBits, char* hamming7264); 
void flipBits(char* hamming7264, uint8_t index);
bool checkHamming7264(char* hamming7264);

/** Create the Hamming encoding for a sequence of 64 data bits.
* 
* Pre: *pH is a Hamming72_64 object
* *pBits is a block of 64 bits
* Post: pH->bits has been initialized to store the given data bits and
* corresponding parity bits, as described in the specification
* of the Hamming (72, 64) scheme.
*/
void set_Hamming72_64(Hamming72_64* const pH, const uint8_t* const pBits){
    char* hamming64Bits = calloc(65, sizeof(char));

    for(int x= 0; x < 8; x++){
        char* binary = intToBin(pBits[x]);
        for(int y = 7; y >=0; y--){
            strncat(hamming64Bits, &binary[y], 1);
        }
        free(binary); 
    }


    //hamming64Bits[64] = '\0';
    char* hamming72Bits = calloc(73, sizeof(char));
    encodeParityIntoData(hamming72Bits, hamming64Bits);
    char* parity = calloc(9, sizeof(char));
    getParityBits(hamming72Bits, parity);
    putParityBits(hamming72Bits, parity);
    uint8_t ind = 0;
    hamming72Bits[0] = calBits(hamming72Bits, &ind);
    putDataBits(pH->bits, hamming72Bits);
    //hamming72Bits[72] = '\0';
    free(hamming64Bits); 
    free(hamming72Bits); 
    free(parity);  
}

/** Checks the validity of the given Hamming encoding, and corrects errors
* when possible.
* 
* Pre: *pH is initialized
* Post: If correctable errors are found, they are corrected in pH->bits.
* Returns: A Report_Hamming72_64 object holding the computed syndrome
* and flag indicating what type of error, if any, was found.
*/
Report_Hamming72_64 correct_Hamming72_64(Hamming72_64* const pH){

    char* encodedBits = calloc(73, sizeof(char));
    uint8_t index = 0;
    uint8_t syndrome = 0;

    for(uint8_t i = 0; i < 9; i++){
        char* binary = intToBin(pH->bits[i]);
        for(int8_t b = 7; b >=0; b--){
            strncat(encodedBits, &binary[b], 1);
        }
        free(binary); 
    }
    encodedBits[72] = '\0';
    char* hamming72Bits = calloc(73, sizeof(char));
    strcpy(hamming72Bits, encodedBits);
    char* parity = calloc(9, sizeof(char));
    getParityBits(hamming72Bits, parity);
    putParityBits(hamming72Bits, parity);
    hamming72Bits[0] = calBits(hamming72Bits, &index);
    uint8_t detectedErrs = detectError(encodedBits, hamming72Bits, &syndrome);
    bool check = checkHamming7264(encodedBits);
    Report_Hamming72_64 report;
    bool bitFlipped = true;

    if(check && syndrome == 0){
        report.status = MASTERPARITY;
    }
    else if(!check && syndrome != 0){
        report.status = MULTIBIT;
        bitFlipped = false;
    }
    else if(detectedErrs == 0){
        report.status = NOERROR;
        bitFlipped = false;
    }
    else if(detectedErrs == 1){
        report.status = SINGLEPARITY;
    }
    else if(detectedErrs >= 2){
        report.status = SINGLEDATA;
    }
    report.syndrome = syndrome;

    if(bitFlipped){
        flipBits(encodedBits, syndrome);
    }

    putDataBits(pH->bits, encodedBits);
    free(encodedBits); 
    free(parity); 
    free(hamming72Bits); 
    return report;
}

//This function converts integers(uint8_t) into binary
char* intToBin(uint8_t integer){
   
   char binaryArray[9] = {'0', '0', '0', '0', '0', '0', '0', '0', '\0'};
   uint8_t absVal = (uint8_t) integer;
   for(int x = 7; x >= 0; x--){
      if(absVal%2 == 1){
         binaryArray[x] = '1';
      }
      absVal /= 2;
   }
   char* bin = calloc(9, sizeof(char));
   strcpy(bin, binaryArray);
   return bin;
}

//This function converts binary into integers(uint8_t)
uint8_t binToInt(char* bin){
    uint8_t integer = 0;
    if(bin[0] == '1'){
        integer += 1;
    }
    if(bin[1] == '1'){
        integer += 2;
    }
    if(bin[2] == '1'){
        integer += 4;
    }
    if(bin[3] == '1'){
        integer += 8;
    }
    if(bin[4] == '1'){
        integer += 16;
    }
    if(bin[5] == '1'){
        integer += 32;
    }
    if(bin[6] == '1'){
        integer += 64;
    }
    if(bin[7] == '1'){
        integer += 128;
    }
    return integer;
}

// This function encodes parity bits into databits
void encodeParityIntoData(char* encoded, char* originalBits){
    strncat(encoded, "___", 4);
    strncat(encoded, &originalBits[0], 1);
    strncat(encoded, "_", 2);
    strncat(encoded, &originalBits[1], 3);
    strncat(encoded, "_", 2);
    strncat(encoded, &originalBits[4], 7);
    strncat(encoded, "_", 2);
    strncat(encoded, &originalBits[11], 15);
    strncat(encoded, "_", 2);
    strncat(encoded, &originalBits[26], 31);
    strncat(encoded, "_", 2);
    strncat(encoded, &originalBits[57], 7);
}

//Returns powerOfTwo
int powerOfTwo(int number, int power){
    if(power == 0){
        return 1;
    }
    int exponential = number;
    for(int i = 0; i < power -1; i++){
        exponential *= number;
    }
    return exponential;
}

void putParityBits(char* hamming7264, char* pBits){
    hamming7264[1] = pBits[1];
    hamming7264[2] = pBits[2];
    hamming7264[4] = pBits[3];
    hamming7264[8] = pBits[4];
    hamming7264[16] = pBits[5];
    hamming7264[32] = pBits[6];
    hamming7264[64] = pBits[7];
}

void getParityBits(char* hamming7264, char* pBits){
    for(int i = 7; i > 0; i--){
        bool skipBits = false;
        uint8_t x = (powerOfTwo(2,i)/2);
        uint8_t index = 0;
        uint8_t start = 0;
          
        for(int j = x+1; j < 72; j++){
            if(j == 2){
                ++j;
            }
            if(start < x-1 || (skipBits && start < x) || i == 1){
                if(hamming7264[j] == '1'){
                    index++;
                }
                start++;
            }
            if((start == x-1 && !skipBits) || start == x){
                j += x;
                skipBits = true;
                start = 0;
            }
        }
        if(index%2 == 0){
            pBits[i] = '0';
        }
        else{
            pBits[i] = '1';
        }
    }
}

char calBits(char* hamming7264, uint8_t* integer){
    for(uint8_t x = 1; x < 72; x++){
        if(hamming7264[x] == '1'){
            (*(integer))++;
        }
    }
    if((*(integer))%2 == 0){
        return '0';
    }
    else{
        return '1';
    } 
}

//This function detects errors
uint8_t detectError(char* wholebits, char* hamming7264, uint8_t* syndrome){
    uint8_t i = 0;
    for(uint8_t x = 1; x < 8; x++){
        uint8_t index = (powerOfTwo(2,x)/2);
        if(wholebits[index] != hamming7264[index]){
            i++;
            (*(syndrome)) += index;
        }
    }
    return i;
}

void putDataBits(uint8_t* dataBits, char* hamming7264){
    for(uint8_t x = 0; x < 9; x++){
        char* memBin = calloc(9, sizeof(char));
        strncat(memBin, & hamming7264[x*8], 8);
        dataBits[x] = binToInt(memBin);
        free(memBin); 
    }
}

void flipBits(char* hamming7264, uint8_t index){
    if(hamming7264[index] == '0'){
        hamming7264[index] = '1';
    }
    else{
        hamming7264[index] = '0';
    }
}

bool checkHamming7264(char* hamming7264){
    uint8_t index = 0;
    for(uint8_t x = 0; x < 72; x++){
        if(hamming7264[x] == '1'){
            index++;
        }
    }
    return index%2;
}

