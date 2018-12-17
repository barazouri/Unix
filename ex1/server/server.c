#include<netdb.h>
#include<netinet/in.h>
#include<stdlib.h>
#include<stdio.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<arpa/inet.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/stat.h>
#include <fcntl.h>
#define BUF_SIZE 2000
#define PORT 8080

int main(int argc,char *argv[]){
  int socket_desc,accept_sock, c, rec, num=0;
  struct sockaddr_in server;
  char buf[BUF_SIZE];
  memset(buf, 0, BUF_SIZE);
pid_t main_pid = getpid();
  //create socket
  socket_desc = socket(AF_INET, SOCK_STREAM, 0);
  if(socket_desc == -1){
    fprintf(stderr,"couldn't create socket\n");
  }
  fprintf(stderr,"socket created\n");
bzero(&server, sizeof(server));
//prepere the sockaddr_in
server.sin_family = AF_INET;
server.sin_addr.s_addr = htonl(INADDR_ANY);
server.sin_port = htons(PORT);

  //bind
  if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) != 0){
    //print the error message
    perror("bind faild");
    return 1;
  }
  fprintf(stderr,"bind done\n");


  //listen
  if(listen(socket_desc, 5) != 0){
    printf("listen failed\n");
    return 1;
  }

  //accept and incoming connection
  printf("server listening\n");
  c = sizeof(struct sockaddr_in);


while(1)
{
  accept_sock = accept(socket_desc, (struct sockaddr *)&server, &c);
  if(accept_sock < 0){
    perror("accept failed");
    return 1;
  }
  printf("connection accepted\n");
  pid_t pid =  fork();

  if(pid<0){
    perror("fork error");
  }
  if(!pid){


  if((rec = recv(accept_sock, buf, 50, 0 )) < 0){
    perror("recive");
  }


  if(strncmp(buf,"list-file",9) == 0)
  {

    DIR* directory;
    struct dirent* ent;
    if((directory = opendir("./Files")) == NULL)
    {
    printf("cloud not open file %s\n",argv[1]);
    return 2;
    }
      while((ent = readdir(directory)) != NULL){
        if((strncmp(ent->d_name,".",1) == 0 )|| (strncmp(ent->d_name,"..",2) == 0)){
          continue;
        }
        num++;
      }
      send(accept_sock, &num, sizeof(num),0);

    if((directory = opendir("./Files")) == NULL)
    {
    printf("cloud not open file %s\n",argv[1]);
    return 2;
    }

   while((ent = readdir(directory)) !=NULL){
     if((strncmp(ent->d_name,".",1) == 0 )|| (strncmp(ent->d_name,"..",2) == 0)){
       continue;
     }
     char path[100] = "./Files/";
     strcat(path, ent->d_name);
     int fd = open(path, O_RDONLY);
     struct stat st;
     fstat(fd, &st);                         /// get the size of the file in bytes
     int size = st.st_size;
     close(fd);

     strcpy(buf, ent->d_name);
      send(accept_sock, buf, BUF_SIZE,0);
      send(accept_sock, &size, sizeof(int),0);


   }
  return 0;
}
  else if(strncmp(buf, "upload-file", 11) == 0){
    char path[100] = "./Files/";
    char fileName[50];
    if(recv(accept_sock, fileName, 50, 0) < 0)
    {
      printf("[-]Error in receiving data.\n" );
    }
    strcat(path, fileName);
    if(access(path, F_OK) != -1){
      strcpy(buf,"exist" );
      send(accept_sock, buf, BUF_SIZE, 0);
      return 0;
    }
    else{
      strcpy(buf,"not exist");
      send(accept_sock, buf, BUF_SIZE, 0);
    }
    int f = open(path, O_CREAT | O_WRONLY, S_IRUSR | S_IWUSR);
    if(f<0){
      printf("error\n");
      exit(1);
    }

    int size = 0;
    if(recv(accept_sock, &size, sizeof(int), 0) < 0)
    {
      printf("[-]Error in receiving data.\n" );
    }
    for(int i=0 ; i<size ; i++){
      if(recv(accept_sock, buf,BUF_SIZE, 0) < 0)
      {
        printf("[-]Error in receiving data.\n" );
      }

      write(f, buf, strlen(buf));
    }
    close(f);

  }
  else{
    num=0;
    send(accept_sock, &num, sizeof(num),0);
  }
  memset(buf, 0, BUF_SIZE);
}
rec=0;

}
}
