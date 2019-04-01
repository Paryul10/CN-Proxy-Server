// Client side C/C++ program to demonstrate Socket programming
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <dirent.h>
#define PORT 8080
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

int main(int argc, char const *argv[])
{
    while(1)
    {
        struct sockaddr_in address;
        int sock = 0, valread;
        struct sockaddr_in serv_addr;
        char *invalid = "Invalid input was entered";
        char buffer[1024] = {0};

        char *list = "listall";

        char *command = malloc(1024 * sizeof(char));

        if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        {
            printf("\n Socket creation error \n");
            return -1;
        }

        memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                    // which is meant to be, and rest is defined below

        serv_addr.sin_family = AF_INET;
        serv_addr.sin_port = htons(PORT);

        // Converts an IP address in numbers-and-dots notation into either a
        // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
        if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
        {
            printf("\nInvalid address/ Address not supported \n");
            return -1;
        }

        if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
        {
            printf("\nConnection Failed \n");
            return -1;
        }
        
        

        printf( ">> ");
        size_t cmdlength = 0;
        getline(&command,&cmdlength,stdin);
        printf("\nCommand entered is : %s  \n",command);

        // THE Commands given by the client are broke down and stored in broke_comms[i] , for further computation.        
        char * token = strtok(command," \n\t\r");
        int no_of_commands = 0;
        char broke_comms[10][100];


        // printf("-----------------Parsing command-------------\n");
        while(token != NULL)
        {
            strcpy(broke_comms[no_of_commands],token);
            // printf("%s\n",token);
            no_of_commands++;
            token = strtok(NULL," \n\t\r");
        }
        // printf("-----------------Parsing command done-------------\n");




        // checking of all possible scenarios
        if(no_of_commands > 2 || no_of_commands < 1)
        {
            printf("Invalid number of arguments given\n");
            printf("\nThe available commands are\n 1. 'listall' \n 2. 'send file_name.txt' \n 3. 'listmy' \n");
            free(command);
            continue;
        }

        else if(strcmp(broke_comms[0],"listmy")==0)
        {
                // open directory and list all files in the client directiory
                int k=1;
                DIR *d;
                struct dirent *dir;
                d = opendir(".");
                if (d)
                {
                    int c=0;
                    while ((dir = readdir(d)) != NULL)
                    {
                        if((strcmp(dir->d_name,".")==0) ||(strcmp(dir->d_name,"..")==0))
                        {
                            continue;
                        }
                        else
                        {
                            // strcat(send_msg,dir->d_name);
                            // strcat(send_msg,"\n");
                            printf("%s%s%s\n", KGRN, dir->d_name, KWHT);
                        }
                    }
                    closedir(d);
                    k=0;
                    continue;
                }
        }

        else if(strcmp(broke_comms[0],"listall")==0)
        {
            // open directory and list all files in the server directiory
            int k=1;
            send(sock , &broke_comms[0] , strlen(broke_comms[0]) , 0 );  // send the message.            
            printf("\nList message sent\n");
            valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
            printf("%s%s%s\n", KBLU,buffer,KWHT );
            free(command);            
            k=0;
            continue;
        }

        else if(strcmp(broke_comms[0],"send")==0)
        {
            int k=0;
            int len = strlen(broke_comms[1]);
            int cc = 0;
            const char *last_four = &broke_comms[1][len - 4];
            

            
            int c1 = strcmp(last_four, ".png");
            int c2 = strcmp(last_four, ".jpg");
            int c3 = strcmp(last_four, ".out");
            int c4 = strcmp(last_four, ".wav");
            int c5 = strcmp(last_four, ".mp3");

            if ( c1 != 0 &&  c2 != 0 &&  c3 != 0 &&  c4 != 0 &&  c5 != 0)
            {
                send(sock , &broke_comms[0] , strlen(broke_comms[0]) , 0 );  // send the message.            
                printf("\nSend message sent\n");
                                                                                                        // NEED TO SEND FILE HERE.
                valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
                printf("\n%s%s%s\n", KBLU,buffer,KWHT);

                for(int i=0;i<1024;i++)
                {
                    buffer[i] = 0;
                }

                send(sock , &broke_comms[1] , strlen(broke_comms[1]) , 0 );  // send the message.            
                k=1;
                valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
                
                printf("%s%s%s\n", KCYN,buffer,KWHT );

                if(strcmp(buffer,"-1")==0)
                {
                    printf("%sFIle Not Found%s\n",KRED,KWHT);
                }

                else
                {
                    FILE *f = fopen(broke_comms[1],"w");
                    int did_write = fputs(buffer,f);
                    if(did_write < 0)
                    {
                        printf("Failed to write\n");
                    }
                    else
                    {
                        printf("%sSuccesS : File Downloaded successfully%s\n", KYEL,KWHT);
                    }
                    fclose(f);
                }
            }
            else
            {
                printf("%sOnly text files can be downloaded%s\n",KRED,KWHT);
            }
            free(command);
            int k=0;
            continue;
        }

        else if(strcmp(command,"quit") == 0)
        {
            // printf("send 3 called\n");
            int tru = 1;
            close(sock);
            setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&tru,sizeof(int));
            free(command);
            printf("\nConnection is over. Address is now again in use\n");
            exit(-1);
        }
        else
        {
            // printf("send 4 called\n");
            send(sock , invalid , strlen(invalid) , 0 );  // send the message.            
            printf("\nNot a valid Command. !!!!! Please try again\n");
            printf("\nThe available commands are\n 1. 'listall' \n 2. 'send file_name.txt' \n 3. 'listmy'\n");
            free(command);
            continue;
        }
        
        close(sock);
    }

    return 0;
}
