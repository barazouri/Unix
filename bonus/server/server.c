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
#include<ctype.h>
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

   if(strncmp(buf, "download-file", 13) == 0){
    char path[100] = "./Files/";
    char fileName[50];
    num=0;
    if(recv(accept_sock, fileName, 50, 0) < 0)
    {
      printf("[-]Error in receiving data.\n" );
    }
    strcat(path, fileName);
		char c;
		int f = open(path,O_RDONLY);
		if(f < 0){
			printf("the file coudnt be open\n");
			return 0;
		}

		int line = 0, ret;
		while((ret = read(f,buf, BUF_SIZE )) > 0 ){
			line++;
		}
		send(accept_sock, &line, sizeof(int),0);
		lseek(f, 0, SEEK_SET);

		while((ret = read(f,buf, BUF_SIZE )) > 0){
			buf[ret] = '\0';
			send(accept_sock, buf, BUF_SIZE,0);
		}

		printf("The file has been successfully sent.\n" );

  }
  if(strncmp(buf, "search", 6) == 0){
    char tmp[100] = "";
    char fileName[51];
    if(recv(accept_sock, fileName, 50, 0) < 0)
    {
      printf("[-]Error in receiving data.\n" );
    }
    int fd =open("forsearch.txt", O_CREAT | O_WRONLY, 0666);
    if(fd<0){
      printf("file error\n" );
      return 1;
    }
    num=0;
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
           write(fd,ent->d_name,strlen(ent->d_name));
           write(fd,"\n",1);
           strcat(tmp,ent->d_name);
           strcat(tmp," ");
         }
     lseek(fd, 0, SEEK_SET);
    char command[50] = "grep ";
    strcat(command,fileName);
    strcat(command," ");
    strcat(command,"forsearch.txt");
    int fd1 =open("out.txt", O_CREAT | O_RDWR | O_TRUNC, 0666);
    if(fd1<0){
      printf("file error\n" );
      return 1;
    }

    close(STDOUT_FILENO);
    dup(fd1);
    system(command);
    lseek(fd1, 0, SEEK_SET);

    int x=0;
    memset(buf, 0, BUF_SIZE);
    while((x=read(fd1,buf,50)) > 0){
      num++;
    }
    send(accept_sock,&num,sizeof(int),0);
    lseek(fd1, 0, SEEK_SET);
    while((x=read(fd1,buf,50)) > 0){
      send(accept_sock,buf,50,0);
    }
    close(fd1);
    close(fd);
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
