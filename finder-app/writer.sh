#!/bin/sh
# writer app script
# Author: Kyle Zogg

#example usage
# writer.sh /tmp/aesd/assignment1/text.txt linux

#collect input vars
#two inputs, 1 is a full path to a file, 2 is a write string

writefile=$1
writestr=$2

#put in empty line to separate the output from the input
#echo ""

#print the input parameters to make sure we collected them properly
#echo $path
#echo $search_str

#if there aren't two input parameters then exit with 1 and print statement
# $# is the number of arguements passed to the script
if [ $# != 2 ]
then
	echo "Wrong number of input parameters."
	echo "Total number of inputs should be 2."
	echo "The order of the inputs should be:"
	echo "    1) Full path to file."
	echo "    2) String to write."
	exit 1
fi

#create the new file and put the text in it
echo "$writestr" > $writefile

exit 0
