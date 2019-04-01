#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#define PORT 8080

int flag = 0;
int main(int argc, char const *argv[])
{
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  // sockaddr_in - references elements of the socket address. "in" for internet
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";
    char *listing = "listall";
    char *sending = "send";


    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc.
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    printf("Server set up done..\n");
    printf("Ready to listen.....\n");
    while(1)
    {
        // printf("Waiting\n");
        new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
        if (new_socket < 0)
        {
            perror("ERROR on accept");
            exit(-1);
        }

        char *send_msg = malloc(1024 * sizeof(char));
        int pid = fork();                                    // To handle multiple clients we need to fork , create multiple processes to process each request.
        if (pid < 0)
            perror("ERROR on fork");
        if (pid == 0)
        {
            for(int i=0;i<1024;i++)
            {
                buffer[i] = 0;
            }
            valread = read(new_socket, buffer, 1024);


            printf("\n\nCommand recieved by client : %s\n",buffer );

            if(strcmp(buffer,listing)==0)
            {
                printf("\nlistall called\n");
                
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
                            strcat(send_msg,dir->d_name);
                            strcat(send_msg,"\n");
                        }
                    }
                    closedir(d);
                    printf("%s\n",send_msg);
                    send(new_socket , send_msg , strlen(send_msg) , 0 );  // use sendto() and recvfrom() for DGRAM
                    printf("List of all the files in the present directory sent\n");
                    free(send_msg);
                    continue;
                }
            }
            else if(strcmp(buffer,"send")==0)
            {
                for(int i=0;i<1024;i++)
                {
                    buffer[i] = 0;
                }
                
                send_msg = "Checking file availabiltiy\n";
                send(new_socket , send_msg , strlen(send_msg) , 0 );

                valread = read(new_socket, buffer, 1024);
                printf("Reading File...\n");
                FILE *f;
                f = fopen(buffer,"r");
                if(f==NULL)
                {
                    printf("Error opening file\n");

                    // send_msg = "-1";
                    char * fail = "-1";
                    send(new_socket , fail , strlen(fail) , 0 );
                    continue;
                }


                fseek( f , 0L , SEEK_END);
                long lSize = ftell( f );
                rewind( f );        

                send_msg = calloc( 1, lSize+1 );
                if( !send_msg )
                {   
                    fclose(f),fputs("memory alloc fails",stderr);
                    exit(0);
                }

                if( 1!=fread( send_msg , lSize, 1 , f) )
                {
                    fclose(f),free(send_msg),fputs("entire read fails",stderr);
                    exit(0);
                }

                // printf("%d\n",f);
                // fscanf(f,"%s",send_msg);
                // write(server_fd,send_msg,1000);

                printf("%s\n",send_msg);

                send(new_socket , send_msg , strlen(send_msg) , 0 );  // use sendto() and recvfrom() for DGRAM
                printf("Sent the file asked for\n");
                free(send_msg);
                // exit(0);
            }
            else if(strcmp(buffer,"quit")==0)
            {
                printf("Connection terminated with IP %s\n",buffer);
                close(new_socket);
                free(send_msg);
                break;
            }
            else
            {
                send_msg = "Enter a vaild command";
                send(new_socket , send_msg , strlen(send_msg) , 0 );  // use sendto() and recvfrom() for DGRAM
                printf("inVaild command entered\n");
                // free(send_msg);
                // exit(0);
            }
            
            exit(0);
            // send_msg
        }
        else
        {
            close(new_socket);
        }
        
    }
    
    
    return 0;
}
