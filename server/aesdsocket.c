#include <stdio.h>
#include <syslog.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>
#include <syslog.h>
#include <netdb.h>
#include <signal.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define SIZE sizeof(char)*100000

int exit_program = 0;

//Create a signal handler
void sig_handler(int signum){
    if(signum == SIGINT || signum == SIGTERM){
        printf("\nProgram recieved the SIGINT or SIGTERM signal and will now exit\n");
        exit_program = 1;
    }
}


/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////
// START MAIN
/////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////

int main (int argc, char *argv[]) {
// make the variables we need


//make buffer for storing strings
    char *buffer;
    buffer = malloc(SIZE);


//open syslog
     openlog("", 0, LOG_USER);
     //syslog(LOG_USER, "this is a syslog teest");


//open output file
    FILE *io_file;
    io_file = fopen("/var/tmp/aesdsocketdata", "w+r");

//bind the signal handler
    signal(SIGINT, sig_handler); //register the signal handler
    signal(SIGTERM, sig_handler);


//while loop to test the signal handler: not needed in final program
// int i = 0;
// while( (i < 20) && !exit_program ){
//     printf("\ninside hte main funciton\n");
//     usleep(1000000);
// }


//create sockaddr
    struct addrinfo *res;
    struct addrinfo hints;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = 0;
    hints.ai_flags = SOCK_STREAM;
    hints.ai_addr = NULL;
    hints.ai_canonname = NULL;
    hints.ai_next = NULL;

    int addr_info = getaddrinfo(NULL, "9000", &hints, &res);
    if (addr_info < 0) {
        syslog(LOG_USER, "Get addrinfo failed");
        free(buffer);
        closelog();
        freeaddrinfo(res);
        freeaddrinfo(&hints);
        return -1;
    }
    //make the sock_fd
    int sock_fd;
    for (; res != NULL; res = res->ai_next) {
        sock_fd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
        if (sock_fd < 0) {
            continue;
        }
        int enable = 1;
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEADDR, &enable, (socklen_t)sizeof(int));
        setsockopt(sock_fd, SOL_SOCKET, SO_REUSEPORT, &enable, (socklen_t)sizeof(int));

        //bind the sock_fg
        if (bind(sock_fd, res->ai_addr, res->ai_addrlen) == 0){
            break;
        }
    }


//Deal with the Daemon
    if (argc > 1 && strcmp(argv[1], "-d") == 0) {
        int rc = 0;
        rc = fork();
        if (rc < 0) {
            syslog(LOG_USER, "fork failed");
            free(buffer);
            closelog();
            return -1;
        } else if (rc > 0) {
            //wait 50ms
            usleep(50000);
            return 0;
        } else {
            syslog(LOG_USER, "something weird in fork not sure why we would get here");
            setsid();
            rc = fork();
            if (rc < 0) {
                free(buffer);
                closelog();
                return -1;
            } else if (rc > 0) {
                return 0;
            }
        }
    }


//listen
    listen(sock_fd, 10);


//accept
//NEED TO UPDATE
    struct pollfd polling_v;
    polling_v.fd = sock_fd;
    polling_v.events  = POLLIN | POLLOUT;
    polling_v.revents = 0;

    //define the client address var
    struct sockaddr *client_addr;

    while (!exit_program) {
        fd_set sock_fd_set;
        FD_ZERO(&sock_fd_set);
        FD_SET(sock_fd, &sock_fd_set);
        if (poll(&polling_v, (nfds_t)1, 100) <= 0) {
            continue;
        }
        socklen_t clilen = (socklen_t)sizeof(struct sockaddr);
        client_addr = malloc((size_t)clilen);
        int new_sock_fd = accept(sock_fd, client_addr, &clilen);
        if (new_sock_fd < 0) {
            syslog(LOG_USER, "The accept failed");
            free(buffer);
            free(client_addr);
            closelog();
            return -1;
        }
        syslog(LOG_DEBUG, "Accepted connection from %d\n", ((struct sockaddr_in*)client_addr)->sin_addr.s_addr);
        

        //
        //  NEED TO DO RECV IN A LOOP
        //

        long fsize = 0;
        ssize_t size = 0;
        int loop_num = 0;

        while(1){

            loop_num++;

            //write the whole contents of buffer to null characters
            memset(buffer,'\0',SIZE);   

            //recv stores the incoming message in buffer
            size = recv(new_sock_fd, buffer, SIZE, 0);
            if(size < 0){
                syslog(LOG_USER, "The recv failed at size and was %ld", size);
            }
            if(size == 0) {
                //we finished reading the stream
                syslog(LOG_USER,"DONE recieving data we looped %d times", loop_num);
                break;
            }
            if(size > 0){
                //log how many bytes were recieved.
                syslog(LOG_USER, "the size of the recv is %ld bytes and should be 19890 for the long case", size);
            }

            //find the end of the io file
            fseek(io_file, 0, SEEK_END);
            

            //append the message to the end of the file    LOOK AT THIS CODE HERE
            fwrite(buffer, sizeof(char), size, io_file);


            //find the new end of the file
            fseek(io_file, 0, SEEK_END);

            //don't need to loop again if size is small
            //I DON'T KNOW WHY THIS IS NEEDED BUT IT WORKS IF WE LOOP AGAIN IT DOESN'T WORK
            //THIS REALLY DOENS'T MAKE SENSE TO ME BUT IT NEEDS TO NOT LOOP AGAIN AND CHECK FOR A == 0 CASE
            //COULD TRY LOOPING THROUGH BUFFER TO SEE IF IT CONTAINS A NEW LINE INSTEAD OF LOOKING AT SIZE

            //IT WORKED TO CHECK FOR A NEWLINE NOT SURE WHY THIS IS THE CASE BUT I DON'T CARE
            int i;
            for(i = 0; i < size; i++) {
                if (buffer[i] == '\n') {
                    syslog(LOG_USER,"Broke out cuz newline found. Looped %d times", loop_num);
                    break;
                }
            }


            //if(size < 16000){
            //    syslog(LOG_USER,"Broke out with size was less that 1000 we looped %d times", loop_num);
            //    break;
            //}
        }
            loop_num = 0;

            //find the new end of the file
            fseek(io_file, 0, SEEK_END);

            //use ftell to get the new length of the io file
            fsize = ftell(io_file);

            //move file position back to start of file
            fseek(io_file, 0, SEEK_SET);

            //clear out buffer
            memset(buffer,'\0', SIZE);

            //store the entire contents of the io file in buffer to get sent back to client
            fread(buffer, sizeof(char), fsize, io_file);

        
        //send

        //for debug use this for loop to print what is in buffer at this point
        int i;
        printf("\n\n\n sending data \n\n\n\n");
        for(i = 0; i < fsize; i++){
           printf("%c", buffer[i]);
        }

        syslog(LOG_USER, "The send expected size was: %ld", fsize);
        
        //create a pointer to where we are in buffer
        char *ptr = (char*) buffer;

        //keep sending data until fsize is 0
        while(fsize > 0){

            int i = send(new_sock_fd, buffer, fsize, 0);
            
            if(i < 0){
                syslog(LOG_USER, "The send failed and size and was %ld", fsize);
            }
            syslog(LOG_USER, "The send actual data was: %d", i);
            //update the ptr spot
            ptr += i;
            fsize -= i;

        }        


        //size = send(new_sock_fd, buffer, fsize, 0);
        //if(size < 0){
        //    syslog(LOG_USER, "The send failed and size and was %ld", size);
        //}

        //syslog(LOG_USER, "The send actual size was %ld", size);

        close(new_sock_fd);
        syslog(LOG_DEBUG, "Closed connection from %d\n", ((struct sockaddr_in*)client_addr)->sin_addr.s_addr);
        free(client_addr);
    }

//END UPDATE

//free everything
    //syslog(LOG_USER, "free 1");
    freeaddrinfo(res);
    //syslog(LOG_USER, "free 2");
    //freeaddrinfo(&hints);
    //syslog(LOG_USER, "free 3");
    free(buffer);
    //syslog(LOG_USER, "free 4");
    close(sock_fd);
    //syslog(LOG_USER, "free 5");
    fclose(io_file);
    //syslog(LOG_USER, "free 6");
    remove("/var/tmp/aesdsocketdata");
    //syslog(LOG_USER, "free 7");
    closelog();


    //syslog(LOG_USER, "made it to the end with no errors");
    return 0;
}