#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include <pthread.h>
#include<arpa/inet.h>
#define MAX_USERS 5

int sock,newsockfd;
int *new_sock;
struct sockaddr_in self_addr;
struct sockaddr_in user_addr;
struct sockaddr_in hser_addres;
socklen_t user_addr_len;
char recv_buff[1000];
char send_buff[100];
char recv_buff1[100];
char *user_ip[MAX_USERS];
char *user_id[MAX_USERS][100];
int i=0;
int dec_flag;
char *fname;
char *literal;
int user_pno[MAX_USERS];
int self_pno;
int flag = 0;
int sno=0;
char tmp[100];
char temp[10000];
char self_ip[100];
char superh_ip[100];
int superh_pno;
struct hostent *hserv;
int hsockfd;
short parse_args(int argc, char **argv);
int search(char*);
void register_user();

int main(int argc, char **argv){
  short port = parse_args(argc, argv);

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    fprintf(stderr, "%s\n", "error - error creating sock.");
    abort();
  }
  
  self_addr.sin_family = AF_INET; 
  self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  self_addr.sin_port = htons(port);
  user_addr.sin_family = AF_INET; 
  user_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  user_addr.sin_port = htons(port);
  
  memcpy(self_ip, argv[1], (strlen(argv[1]) + 1 > sizeof(self_ip)) ? sizeof(self_ip) : strlen(argv[1]));
  printf("Info : Self ip is %s \n",self_ip);
  self_pno = ntohs(self_addr.sin_port);
  
  if (bind(sock, (struct sockaddr *)&self_addr, sizeof(self_addr)) < 0) {
    fprintf(stderr, "%s\n", "error - error binding sock.");
    abort();
  }
  
  listen(sock,5);
 user_addr_len = sizeof(user_addr);

// Super node accepting user and other super node connections
 
while(newsockfd = accept(sock, (struct sockaddr *)&user_addr, &user_addr_len))
 {
   
   
   if (newsockfd < 0) {
			perror("ERROR on accept\n");
			exit(1);
			}
      printf("connection accepted \n");
      char* t_buff[1];
      memset(t_buff,'\0',strlen(t_buff));
        read(newsockfd, t_buff, 1);
        char *ff[1];
        strcpy(ff,t_buff);
	// Identifies if user or super node is connected to it
      if((strcmp(ff,"u"))==0)
      {
        pthread_t client_thread;
        new_sock = malloc(100);
        *new_sock = newsockfd;
        
        if( pthread_create( &client_thread , NULL ,  register_user , (void*) new_sock) < 0)
        {
            perror("Info : Could not create thread");
            return 1;
        }
      }
      else if ((strcmp(ff,"s"))==0)
      {
          
        printf("Info : Need to search in me \n");
        memset(recv_buff,'\0',sizeof(recv_buff));
        printf("Info : Reading buffer \n");
        read(newsockfd, recv_buff, sizeof(recv_buff));
        char *hname[200];
        strcpy(hname,recv_buff);
        printf("Info : Received name %s \n", hname);
        search(hname);
  
      }
  }
 
  exit(0);
}

//Super node searches in its list if the user id is present

int search(char* name)
{
  
   
    int j=0, count = 0;
    for(j=0;j<MAX_USERS;j++)
    {
      if((strcmp(name,user_id[j])) == 0)
      {
        printf("Info : Match found \n");
        count++;
        printf("Info : The user details are : %s %s %d \n",user_id[j],user_ip[j],user_pno[j]);
        return 1;
      }
      
    }
    
    if(count == 0)
    {
      printf("Info: No Match \n");
      return 0;
      
    }
 }


void register_user()
{
  
   
     memset(recv_buff,'\0',sizeof(recv_buff));
    if(read(newsockfd, recv_buff, sizeof(recv_buff)) < 0)
		{
			perror("Info: Read Error\n");
      			printf("Error");
			exit(0);
		}

    //Each Super node registers upto 5 users 
    
    if(i>4)
    {
      printf("\n User limit exceeded");
      exit(1);
    }
    else
    {
      const char s[2] = "|";
      
    printf("Registering user %d in Super Node \n", i+1);
    literal = strtok(recv_buff, s);
    strcpy(tmp,literal);
    user_ip[i]=tmp;
    printf("Received user ip addr to register is %s \n",user_ip[i]);
    user_pno[i] = ntohs(user_addr.sin_port);
    printf("User port number %d \n" , user_pno[i]);
    literal = strtok(NULL, s);
    strcpy(user_id[i],literal);
    printf("Received user id is %s \n", user_id[i]);
    i++;

    //sending confirmation to client on registartion
    strcpy(send_buff,"r");
    int n = write(newsockfd,send_buff,strlen(send_buff));
       
  
    }
    memset(recv_buff,'\0',sizeof(recv_buff));
    read(newsockfd, recv_buff, sizeof(recv_buff));
   if(strcmp(recv_buff,"")==0)
   {
     printf("closing \n");
     close(newsockfd);
   }
    //searching user

     else
      {
     char* token = strtok(recv_buff, "|");
     if(strcmp(token,"NoMatch") == 0)
     {
       printf("Searching the user in me \n");
       token = strtok(NULL, "|");
       char *name = token;
       printf("Name is %s \n", name);
       int sno = search(name);
       
     
     
     if(sno ==0)
     {
      printf("\nThe user details does not exist in me. \n ");
      printf("Enter the other super node details to search \n");
      printf("Enter Super node ip : \n");
      scanf("%s",superh_ip);
      printf("Enter super node port: \n");
      scanf("%d",&superh_pno);
      
	//connecting to other super node to fetch user details
      
      hsockfd = socket(AF_INET, SOCK_STREAM,0);

      if(hsockfd < 0)
      {
      printf("Error in opening socket");
      exit(1);
      }

  
  bzero((char *)&hser_addres,sizeof(hser_addres));
  hser_addres.sin_family = AF_INET;
  hser_addres.sin_addr.s_addr = inet_addr(superh_ip);
  hser_addres.sin_port = htons(superh_pno);
      if(connect(hsockfd,(struct sockaddr*)&hser_addres,sizeof(hser_addres))<0)
    {
      printf("Error: Socket creation");
      exit(1);
    }
     //Informing super node that it is another super node 
      write(hsockfd, "s", 1);
      printf("\n Info: Connected to help super node \n");
      int n = write(hsockfd, name, strlen(name));
      printf("Info: sending name %s",name);
    }
  }
 }

    
}
/* To Parse port number from command line */

short parse_args(int argc, char **argv){
  if (argc < 3)
  {
    printf("Error: Argument error \n");
    exit(1);
  }
  else
  {
    errno = 0;
    char *endptr = NULL;
    unsigned long ulPort = strtoul(argv[2], &endptr, 10);

    if (0 == errno)
    {
      // If no other error, check for invalid input and range
      if ('\0' != endptr[0])
        errno = EINVAL;
      else if (ulPort > USHRT_MAX)
        errno = ERANGE;
    }
    if (0 != errno)
    {
      // Report any errors and abort
      fprintf(stderr, "Failed to parse port number \"%s\": %s\n",
              argv[1], strerror(errno));
      abort();
    }
    printf("Starting server on port: %lu \n", ulPort);
    return ulPort;
  }
}

