// driver for Hamming72_64 code project
//
// Requires:  HammingGrader.h, HammingGrader.o   <-- test harness
//            Hamming72_64.h                     <-- supplied
//            Hamming72_64.c                     <-- student file
//
// Invocation options:
//
//    c3driver [-repeat]
//       -repeat:  (optional) 
//                 if present, a preexisting seed file will be used, and the
//                    most recent test cases will be used;
//                 if absent, or if no seed file is found, random tests are used
//
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <assert.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "Hamming72_64.h"
#include "HammingGrader.h"

extern unsigned int scoreForGenerationTests;
extern unsigned int scoreForCorrectionTests;

bool passed = true;

int main(int argc, char** argv) {
   
   FILE* log = fopen("HammingLog.txt", "w");
   uint32_t seed;
   
   bool useRandomData = true;
   if ( argc > 1 && strcmp(argv[1], "-repeat") == 0 ) {
      FILE* fp = fopen("seed.txt", "r");
      if ( fp == NULL ) {
         printf("Could not open seed file; generating random data.\n");
      }
      else {
         fscanf(fp, "%"SCNu32, &seed);
         fclose(fp);
         useRandomData = false;
		}
   }
   if ( useRandomData ) {
      seed = time(NULL);
      FILE* fp = fopen("seed.txt", "w");
      fprintf(fp, "%"PRIu32, seed);
      fclose(fp);
   }
   srand(seed);
   
   fprintf(log, "First, we will test your ability to properly generate Hamming encodings:\n");
   // run generation of five Hamming encodings
   for (uint8_t test = 0; passed && test < 10; test++) {
       runGenerationTest(log); 
   }
   if ( !passed ) {
      fprintf(log, "Well, you failed some of those... no further testing until you fix that.\n");
      goto exit;
   }

   fprintf(log, "\nNow we will try testing your ability to determine the correctness of Hamming encodings with no errors:\n");
   for (uint8_t pos = 0; pos < 8; pos++) {
      runCorrectionTest(log, -1, -1);
      if ( !passed ) {
          fprintf(log, "Well, you failed that one... no further testing until you fix that.\n");
       goto exit;
     }
   }
   
   fprintf(log, "\nGood!  Now we will try testing your ability to determine the correctness of Hamming encodings with a single error:\n");
   for (uint8_t pos = 0; pos < 8*NBYTES; pos++) {
      // run checker with munged bit at pos:
     uint8_t toMunge = pos;
      runCorrectionTest(log, toMunge, -1);
   }
   if ( !passed ) {
      fprintf(log, "Well, you failed some of those... no further testing until you fix that.\n");
      goto exit;
   }

   fprintf(log, "Good!  Now we will try testing your ability to determine the correctness of a Hamming encoding with two errors:\n");
   for (uint8_t test = 0; test < 10; test++) {
      // run checker with munged bits at two places:
      uint8_t toMunge1 = rand() % 8*NBYTES;
      uint8_t toMunge2 = rand() % 8*NBYTES;
      if ( toMunge1 == toMunge2 )
         toMunge2 = (toMunge2 + 1) % 8*NBYTES;

      runCorrectionTest(log, toMunge1, toMunge2);
   }

   unsigned int maxScore;
exit:
   maxScore = maxForCorrection() + maxForGeneration();
   fprintf(log, "Score information (maximum possible is %d):\n", maxScore);
   fprintf(log, "   Generation tests: %4d / %4d\n", scoreForGenerationTests, maxForGeneration());
   fprintf(log, "   Correction tests: %4d / %4d\n", scoreForCorrectionTests, maxForCorrection());
   fprintf(log, "              Total: %4d / %4d\n", scoreForGenerationTests + scoreForCorrectionTests, maxScore);

   // harvester-friendly score summary:
   fprintf(log, "\n");
   fprintf(log, ">>Scores from testing<<\n");
   fprintf(log, "  1 >>  generation tests: %4d / %4d\n", scoreForGenerationTests, maxForGeneration());
   fprintf(log, "  2 >>  correction tests: %4d / %4d\n", scoreForCorrectionTests, maxForCorrection());
           
   fclose(log);
   return 0;
}



