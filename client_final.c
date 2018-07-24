#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include<ctype.h>
#include <fcntl.h>


int main(int argc, char *argv[]){

  int server, portNumber;
  struct sockaddr_in servAdd;     // server socket address

  if(argc != 3){
    printf("Call model: %s <IP Address> <Port Number>\n", argv[0]);
    exit(0);
  }

  if ((server = socket(AF_INET, SOCK_STREAM, 0)) < 0){
     fprintf(stderr, "Cannot create socket\n");
     exit(1);
  }

  servAdd.sin_family = AF_INET;
  sscanf(argv[2], "%d", &portNumber);
  servAdd.sin_port = htons((uint16_t)portNumber);

  if(inet_pton(AF_INET, argv[1], &servAdd.sin_addr) < 0){
  fprintf(stderr, " inet_pton() has failed\n");
  exit(2);
}

 if(connect(server, (struct sockaddr *) &servAdd, sizeof(servAdd))<0){
  fprintf(stderr, "connect() has failed, exiting\n");
  exit(3);
 }

 while(1)
 {

    char content[255]=" ";
    char input[255];
    char *fileName;
    char *pch;
    char *command;
    //FILE *file;

    fgets(input, sizeof(input), stdin);
    write(server, input, strlen(input)+1);


    //Getting command
    pch = strtok(input, " ");
    //strcpy(command,pch);
    command = pch;

    //Getting filename
    pch = strtok(NULL," ");
    fileName = pch;

//----------------------------------------

    if (strcmp(command, "quit\n") == 0) {
      close(server);
      fprintf(stderr, "Client: Quits..\n");
      exit(0);
    }
//----------------------------------------
    if (strcmp(command, "get") == 0)
    {
      strcpy(content, "");
      read(server, content, sizeof(content));
      fprintf(stderr, "Client: Receiving file...\n");
      //content[strlen(content) - 1] = '\0';
      //puts(content);
      fileName[strlen(fileName) - 1] = '\0';
      int fd;
      fd = open(fileName, O_RDWR | O_CREAT | O_TRUNC, 0700);
      write(fd, content, strlen(content)+1);
      fprintf(stderr, "Client: File Received...\n");
      close(fd);
      strcpy(command, "");
    }

  //----------------------------------------

  if(strcmp(command, "put") == 0)
        {
          fileName[strlen(fileName) - 1] = '\0';
          int fd;
          fd = open(fileName, O_RDONLY);
          int fileseize = lseek(fd, 0, SEEK_END);
          lseek(fd, 0, SEEK_SET);

          read(fd, content, fileseize);
          //puts(content);
          fprintf(stderr, "Client: Sending file...\n");
          write(server, content, sizeof(content));
          fprintf(stderr, "Client: File sent...\n");
          close(fd);
        }
 }
}
