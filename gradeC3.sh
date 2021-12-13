#! /bin/bash
#
#  Warning:  modifying this script would very likely defeat the purpose of having
#            it supplied to you.
#
#  This script can be used to test and grade your solution for the Hamming Code
#  programming assignment.  This is the script we will use to do that, so if your
#  source file does not work for you with this script, it won't work for us either.
#  That would be unfortunate.
#
#  Invoke as:  ./gradeC3.sh <C source file>
#
#  Last modified:  Nov 29 2021
#
#  To use this test script, you must have copied the distributed testing
#  tar file into a directory on CentOS.  That should have created the
#  following files:
#
#     ./gradeC3.sh                 - this script file
#     ./c3Grader.tar               - tar of the grading code
#         c3driver.c               - C source for the test driver
#         Hamming72_64.h           - supplied header file
#         HammingGrader.h          - 64-bit grading module
#         HammingGrader.o
#
#  We will refer to the extraction directory as the test directory.  
#  If necessary, set execute permissions for gradeC3.sh.
#  Prepare the C source file you intend to submit and place it into the 
#  same directory as gradeC3.sh.  We recommend naming the source file
#  PID.c, where PID is your VT email PID, since that's the naming
#  convention applied by the Curator.
#  Execute the command: ./gradeC3.sh <name of your tar file>
#  Check the results.
#
#  gradeC04.sh will create a subdirectory, ./build, copy your source file
#  into it, unpack the grading files ./build, and execute the build
#  command:
#                 gcc -o c3 -std=c11 -Wall -ggdb3 *.c *.o
#
#  The script will then check for the existence of an executable, c3.  If there
#  is no such file, the script will exit with an error message.  Otherwise, the
#  script will move the executable c3 into the test directory, and execute it
#  to test and grade your solution.
#
#  Relevant results can be found in the following files:
#         
#        buildLog.txt       output from the build process
#        PID.txt            full report, including all of the logs above,
#                           where "PID" is the first token of the name
#                           you gave your tar file
#
#  file and directory names
gradingCode="c3Grader.tar"
buildDir="build"
buildLog="buildLog.txt"
driverLog="HammingLog.txt"

#   name for executable file
stuExe="c3"

#  delimiter written at end of log files
Separator="============================================================"

##################################### fn to extract token from file name
#                 param1: (possibly fully-qualified) name of file
#  Note:  in production use, the first part of the file name will be the
#         student's PID
#
getPID() { 

   fname=$1
   # strip off any leading path info
   fname=${fname##*/}
   # extract first token of file name
   sPID=${fname%%.*}
}

#################################################### Verify command line
# verify existence of student's source file
   if [[ $# -ne 1 ]]; then
      echo "Invocation:  ./gradeC04.sh <name of source file>"
      exit 1
   fi
      
# See if user selected the cleaning option
  if [[ $1 == "-clean" ]]; then
     echo "Removing earlier test files..."
     rm -Rf *.txt $stuExe $buildDir $exeName
     exit 0
  fi

   studentFile=$1
   if [[ ! -e "$studentFile" ]]; then
      echo "Could not find file: $studentFile"
      exit 2
   fi
   if [[ ! -e $gradingCode ]]; then
      echo "Could not find the grading code file: $gradingCode"
      exit 3
   fi
   
# get student PID from file name
   getPID $studentFile
   
# set results file name
   summaryLog="$sPID.txt"

   
# Initiate header for grading log
   echo "Grading:  $1" > $summaryLog
   echo -n "Time:     " >> $summaryLog
   echo `date` >> $summaryLog
   echo >> $summaryLog

####################################################### Build executable
   # Create log file:
   echo "Executing gradeC04.sh..." > $buildLog
   echo >> $buildLog
   
   # Create build directory and put files into it:
   echo "Creating build subdirectory" >> $buildLog
   echo >> $buildLog
   # Create build directory if needed; empty it if it already exists
   if [[ -d $buildDir ]]; then
      rm -Rf ./$buildDir/*
   else
      mkdir $buildDir
   fi
   
   # Copy student's source file to the build directory
   echo "Copying student's file to the build directory:" >> $buildLog
   cp $studentFile ./$buildDir >> $buildLog
   echo >> $buildLog

   # Unpack grading code into build directory
   echo "Unpacking grading code to build directory..." >> $buildLog
   tar xvf $gradingCode -C $buildDir >> $buildLog

   # Build executable for test/grading harness
   cd $buildDir
   buildCmd="gcc -o $stuExe -std=c11 -Wall -ggdb3 *.c *.o"
   echo "Invoking $buildCmd" >> ../$buildLog
   
   $buildCmd >> ../$buildLog  2>&1
   echo >> ../$buildLog

   # Verify existence of executable
   if [[ ! -e $stuExe ]]; then
      echo "Build failed; the file $stuExe does not exist" >> ../$buildLog
      echo "Scores:    0     0     0" >> ../$buildLog
      echo $Terminus >> ../$buildLog
      cp ../$buildLog ../$summaryLog
      exit 4
   fi

   if [[ ! -x $stuExe ]]; then
      echo "Permissions error; the file $stuExe is not executable" >> ../$buildLog
      echo "Scores:    0     0     0     0" >> ../$buildLog
      echo $Terminus >> ../$buildLog
      cp ../$buildLog ../$summaryLog
      exit 5
   fi
   
   echo "Build succeeded..." >> ../$buildLog
   echo >> ../$buildLog
   
   # copy executable to parent directory and move there for testing
   mv $stuExe ..
   cd ..

######################################################## Perform testing
   
   echo "Executing test harness without -repeat flag." >> $summaryLog
   vgrindLog="vgrindLog.txt"
   vgrindSwitches=" --leak-check=full --show-leak-kinds=all --log-file=$vgrindLog --track-origins=yes -v"
   timeout -s SIGKILL 30 valgrind $vgrindSwitches ./$stuExe >> $summaryLog
   timeoutStatus=$?
   if [[ $timeoutStatus -eq 124 || $timeoutStatus -eq 137 ]]; then
      echo "The test of your solution timed out after 30 seconds." >> $testLog
      killed="yes"
   elif [[ $timeoutStatus -eq 134 ]]; then
      echo "The test of your solution was killed by a SIGABRT signal." >> $testLog
      echo "Possible reasons include:" >> $testLog
      echo "    - a segfault error" >> $testLog
      echo "    - a serious memory access error" >> $testLog
      killed="yes"
   fi
   echo >> $summaryLog
   
   # accumulate valgrind error counts
   if [[ -e $vgrindLog ]]; then
      vgrindIssues="vgrind_issues.txt"
      echo "Valgrind issues:" > $vgrindIssues
      grep "in use at exit" $vgrindLog >> $vgrindIssues
      grep "total heap usage" $vgrindLog >> $vgrindIssues
      grep "definitely lost" $vgrindLog >> $vgrindIssues
      echo "Invalid reads: `grep -c "Invalid read" $vgrindLog`" >> $vgrindIssues
      echo "Invalid writes: `grep -c "Invalid write" $vgrindLog`" >> $vgrindIssues
      echo "Uses of uninitialized values: `grep -c "uninitialised" $vgrindLog`" >> $vgrindIssues
   else
      echo "Error running Valgrind test." >> $vgrindLog
   fi

############################################################ Log results
   # Write summary score data into log file
   echo ">>Scores from testing<<" >> $summaryLog
   grep "1 >> " $driverLog >> $summaryLog
   grep "2 >> " $driverLog >> $summaryLog
   echo >> $summaryLog
   echo >> $summaryLog
   if [[ -e $vgrindLog ]]; then
      cat $vgrindIssues >> $summaryLog
   fi
   echo $Separator >> $summaryLog
   
   echo "Details of individual test phases:" >> $summaryLog
   cat $driverLog >> $summaryLog
   echo >> $summaryLog
   
   if [[ -e $vgrindLog ]]; then
      echo "Details from Valgrind analysis" >> $summaryLog
      cat $vgrindIssues >> $summaryLog
      echo $Separator >> $summaryLog
   fi
   
   # log build results
   echo $Separator >> $summaryLog
   cat $buildLog >> $summaryLog
   
   # terminate script
   exit 0

