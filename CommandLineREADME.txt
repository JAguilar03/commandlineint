For Linux on the C ++language, in the terminal type “sudo apt-get install g++”  

Enter your credentials and follow the directions to install. To get it to compile follow this: 

Go to where the directory is with the terminal cd <directory name> 

Type in “g++ -o run commandline.cpp”, this should compile the code, to execute, type in “./run” 

These are the necessary libraries to install

#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <sstream>
#include <iterator>
#include <cstdlib> 
#include <sys/stat.h> 
#include <cerrno> 


################BATCH#####################

For the batch file it is a batch.bat file. 

###README#####################################################
These are the commands used for the file system that works:
Note that if you want to test errors, a directory with a file needs to be made first. 
mkdir <directory>. 
cd <directory>.
cd ..
touch <filename>.
rm <file>.
rmdir <directory>. 
rm –rf <directory>. 


#########################################################################
This still has the batch file feature even though it is not necessary.

The shell command line interpreter runs in the interactive mode and the batch mode.

When you compile the program at first and want to run the batch file you need to put in this command:  
./run ~/Documents/batch.bat   

This assumes that the file is in the Documents directory, please change the directory if needed.

If you run some commands then want the batch file to be executed, input this: "./run batch.bat"

For a pipeline, use the "|" to pipeline. 


If multiple batches are ran, type exit that many times to exit the shell. 



