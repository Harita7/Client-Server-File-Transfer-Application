#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <ctype.h>
#include<string.h>
#include <fcntl.h>


void serviceClient(int);


int main(int argc, char *argv[]){

  int sd, client, portNumber;
  //socklen_t clienlen;
  struct sockaddr_in servAdd;     // server socket address

 if(argc != 2){
    printf("Call model: %s <Port Number>\n", argv[0]);
    exit(0);
  }

  sd = socket(AF_INET, SOCK_STREAM, 0);

  if (sd < 0){
    perror("Error opening socket");
    exit(1);
  }

  servAdd.sin_family = AF_INET;
  servAdd.sin_addr.s_addr = htonl(INADDR_ANY);
  sscanf(argv[1], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  if(bind(sd, (struct sockaddr *) &servAdd, sizeof(servAdd)) < 0)
    printf("Error binding\n");

  listen(sd, 5);

  while(1){
    char quit_msg[10];
    client = accept(sd, NULL, NULL);
    printf("Got a client\n");

    if(!fork())
      serviceClient(client);

    // fgets(quit_msg, sizeof(quit_msg), stdin);
    // if(strcmp(quit_msg, "quit\n") == 0){
    //   close(sd);
    //   close(client);
    //   exit(0);
    // }
    close(client);
  }
}


void serviceClient(int client)
{

    char content[255]=" ";
    char input[255];
    char *command;
    char *fileName;
    char *pch;
    int words = 1;

    while(1){
      read(client, input, sizeof(input));

      //split command
      //Getting Command Name
      pch = strtok(input, " ");
      command = pch;

      //Getting File Name
      pch = strtok(NULL," ");
      fileName = pch;
//----------------------------------------
      if(strcmp(command,"quit\n") == 0)
      {
        close(client);
        fprintf(stderr, "Server: Client Quits..\n");
        exit(0);
      }

//----------------------------------------

      if(strcmp(command,"get") == 0)
      {
        strcpy(content, "");
        fileName[strlen(fileName) - 1] = '\0';
        int fd;
        fd = open(fileName, O_RDONLY);
        int fileseize = lseek(fd, 0, SEEK_END);
        lseek(fd, 0, SEEK_SET);

        read(fd, content, fileseize);
        fprintf(stderr, "Server: Sending file...\n");  
        printf("%s", content);      
        write(client, content, sizeof(content));
        close (fd);
        fprintf(stderr, "Server: File Sent...\n");  
        strcpy(command, "");
        
      }

//--------------------------------------------

      if(strcmp(command,"put") == 0)
      {
      
      read(client, content, sizeof(content));
      fprintf(stderr, "Server: Receiving file...\n");
      //content[strlen(content) - 1] = '\0';
      //puts(content);
      fileName[strlen(fileName) - 1] = '\0';
      int fd;
      fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0700);
      write(fd, content, strlen(content)+1);
      fprintf(stderr, "Server: File Received...\n");
      close(fd);
      strcpy(command, "");
      strcpy(content, "");
      }

}
}
 