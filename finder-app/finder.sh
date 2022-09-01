#!/bin/sh
# finder app script
# Author: Kyle Zogg

#example usage
# finder.sh /tmp/aesd/assignment1 linux

#collect input vars
#two inputs, 1 is a path to a dir, 2 is a search string

path=$1
search_str=$2

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
	echo "    1) Path to directory."
	echo "    2) Search String."
	exit 1
fi

#exit with return value 1 and print statement if filesdir does not represent a directory on filesystem
if [ -d $path ]
then
	#echo "searching files in: $path for search string: $search_str"
	continue
else
	echo "$path is not a valid path"
	exit 1
fi

#Prints a message "The number of files are X and the number of matching lines are Y" where X is the 
#number of files in the directory and all subdirectories and Y is the number of matching lines found in respective files.

#first print the number of files at path
#num_files=$(ls | wc -l )
##remove 1 since the above command gets one extra 
#num_files=$((num_files-1))

num_files=$(find -L $path -type f | wc -l )


#echo "Number of Files in $path and its subdirectories = $num_files"


#second find number of times search_str appears in the files

#this will find the number of matches even if there are multiple in one line
#num_occurances=$(grep -R -o "$search_str" . | wc -l )

#this will find the number of lines with matches but not the total number of occurances
#note that the -o option is not used here
num_occurances=$(grep -R  "$search_str" $path | wc -l )

#echo "number of times $search_str appears in the files: $num_occurances"

#print text: The number of files are X and the number of matching lines are Y
echo "The number of files are $num_files and the number of matching lines are $num_occurances"

#echo ""
exit 0
