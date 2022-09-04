#include <stdio.h>
#include <string.h>
#include <syslog.h>
//declare the two input variables as arrays with 100 characters
//note that if the input is more than 100 characters there will be an error
char writefile[100];
char writestr[100];


int main(int argc, char *argv[] ) {
	//open the syslog
	openlog(NULL,0,LOG_USER);

   // printf() displays the string inside quotation
   //printf("Hello, World!\n");
   //syslog(LOG_USER, "hello syslog\n");

   if(argc == 3){
 		//collect input vars
		//two inputs, 1 is a full path to a file, 2 is a write string

   	//use strcpy from the string.h to copy the input variables into the character arrays
   		strcpy(writefile, argv[1]);
   		strcpy(writestr, argv[2]);
   		
   		//printf("Input Path is: %s\n", writefile);
   		//printf("Input String is: %s\n", writestr);

   		syslog(LOG_USER, "Writing %s to %s", writestr, writefile);
   }
   else 
   {
   		//printf("Expecting 2 arguements not %d arguements\n", argc-1);
   		syslog(LOG_ERR, "Expecting 2 arguements not %d arguements", argc-1);
   		return 1;
   }

   //printf("create a file pointer\n");
   FILE *fp;

   //printf("open or create a file with name writefile\n");
   fp = fopen(writefile, "w");


   //printf("write file with writestr\n");
   fprintf(fp, "%s", writestr);


   //printf("close file\n");
   fclose(fp);


   return 0;
}

