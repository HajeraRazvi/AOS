
/* User node - Implementation  */

#include<stdio.h>
#include<netinet/in.h>
#include<unistd.h>
#include<netdb.h>
#include<errno.h>
#include<stdlib.h>
#include<arpa/inet.h>
#include<string.h>
#include<poll.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<net/if.h>
#include<sys/ioctl.h>
#include<pthread.h>
#include<fcntl.h>
#include <stdbool.h>

#define LEN 1024
#define N 100

char * signup_check();
char * log_in();
char * sign_up();
void stdin_buf_clean(void);
int search();
void super_node_help();
char sign_user[50];
char present_user[20];
char *friend_id[N];
int friend_port[N];
char *friend_ip[N];
int rem_portno;
char* peer_ip[20];
char file_name[20];
FILE *f;
struct sockaddr_in ser_addres;
char read_buff[50];
int sock;
char *fname;
char self_ip[20];
short self_port;
char temp[1000];
char send_buff[LEN], temp_buff[LEN], recv_buff[LEN];
struct sockaddr_in self_addr, super_addr;
char buff1[1000];
char buf[100];
char *literal;
int checkuser(char filename, char usr_list[1024],int n);
int x;
int flag = 0;

void peer_calling(unsigned long val);
void chat_begin(int sock_fd, struct sockaddr_in *peer);

//FUNCTION FOR LOGIN DETAILS AND RETURNS THE INFO

char * log_in()
{
  char * log_details = malloc(20);
  char pass_wrd[20], usr_name[20];

  printf("\nEnter Username: ");

  fflush(stdin);
  scanf("%s", present_user);

  if(present_user[strlen(present_user)-1 ]== '\n')
    {
      present_user[strlen(present_user)-1] = '\0';
    }

  strcpy(usr_name,present_user);
  fflush(stdin);
  printf("\nEnter Password: ");
  scanf("%s", pass_wrd);

  if(pass_wrd[strlen(pass_wrd)-1 ]== '\n')
    {
      pass_wrd[strlen(pass_wrd)-1] = '\0';
    }

  strcat(usr_name,"|");
  printf("%s",usr_name);
  strcat(usr_name,pass_wrd);
  strcpy(log_details, usr_name);
  printf("\nInfo : login details = %s ", log_details);
  return log_details;
}

//Main Method

int main(int argc, char *argv[])
{
  int sock_fd,port_num,n;
  char choice;
  char * log_details, *sign_detail, *token_set, *token, *signup_valid;
  char send_buff[LEN], temp_buff[LEN], recv_buff[LEN];
  char usr_name[1024],pass_wrd[1024];
  char buff[1024];
  struct hostent *serv;

  char super_ip[20];
  int super_port;

  bool auth_fg = false;

//Usage error : num of parameters:
  if (argc < 4){
    printf("Usage: auth_serv_IP client_IP port_num");
    exit(1);
  }

  sock_fd = socket(AF_INET, SOCK_STREAM,0);

  if(sock_fd < 0)
    {
      printf("Error in opening socket");
      exit(1);
    }

  serv = gethostbyname(argv[1]);


  //NO SERVER HOST EXIST ERROR
  if(serv == NULL){
    printf("No such host exist");
    exit(1);
  }
  port_num = 8888;
  bzero((char *)&ser_addres,sizeof(ser_addres));
  ser_addres.sin_family = AF_INET;
  ser_addres.sin_addr.s_addr = inet_addr(argv[1]);
  ser_addres.sin_port = htons(port_num);

  //Creating the Socket for server communication :
  if(connect(sock_fd,(struct sockaddr*)&ser_addres,sizeof(ser_addres))<0)
    {
      printf("Error: Socket creation");
      exit(1);
    }

  setbuf(stdout,NULL);

  while(1) {
    int op = 0;
    while(op == 0)
    {	
	    printf("\n");
	    printf("Select from the following options : \n");
	    printf(" *	For Login : type 'l' \n");
	    printf(" *	For New User to Registration : type 'r' \n");
	    printf(" *	To exit : type 'e' \n");
	    scanf("%s",&choice);
	    printf("Info : Option selected is : %s ",&choice);
	    switch(choice){
	    	case 'l':
			op = 1; 
			break;
		case 'r':
			op = 1; 
			break;
		case 'e':
			op = 2; 
			break;
		default:
			printf("\nPlease enter the appropriate option : 'l', 'r' or 'e' \n");
			op = 0;
			break;
	    }
	    if(op == 2){
		printf("\nExiting the Program ..... \n");
		exit(1);
	    }
	    if(op == 1){
	    	buff[0] = choice;
		break;
	    }
    }
    memset(buff,'\0', sizeof(buff));
    buff[0] = choice;
    n = write(sock_fd, buff, strlen(buff) + 1);

    while(1){
      memset(recv_buff,'\0',sizeof(recv_buff));
      n = read(sock_fd,recv_buff,sizeof(recv_buff));

      if(strcmp(recv_buff,"signup") == 0)
      {
	  // TO initiate the call of signup :
	  printf(" ---- signup");
	  memset(send_buff , '\0', sizeof(send_buff));
	  signup_valid = signup_check();
	  strcpy(send_buff,"signup");
	  strcat(send_buff,"|");
	  strcat(send_buff,signup_valid);
	  printf("-->%s\n",send_buff );
	  n= write(sock_fd, send_buff, strlen(send_buff)+1 );

      }

      if(strcmp(recv_buff,"exist") == 0)
      {
	  printf("\nThis username is already exist please choose different username\n");
	  memset(recv_buff,'\0',sizeof(recv_buff));
	  break;
      }

      if(strcmp(recv_buff,"dont") == 0)
      {
	  printf("\nsuccess\n");

    // Call to sing_up finction :
	  sign_detail = sign_up();
	  memset(send_buff , '\0', sizeof(send_buff));
	  strcpy(send_buff,"dosignup");
	  strcat(send_buff,"|");
	  strcat(send_buff,sign_detail);
	  printf("-->%s\n",send_buff );


	  n= write(sock_fd, send_buff, strlen(send_buff)+1 );


      }

      // To check the user has opted login service
      if(strcmp(recv_buff,"login")==0)
      {
      //LOGIN FUNCTION CALL
	  log_details=log_in();

	  // To populate the buffer with login details:
	  memset(send_buff,'\0',sizeof(send_buff));
	  strcpy(send_buff,"login");
	  strcat(send_buff, "|");
	  strcat(send_buff,log_details);

	  n= write(sock_fd, send_buff, strlen(send_buff)+1 );

   }

    //After the confirmation of login credentials :

    if(strcmp(recv_buff,"true") == 0)
    {
	  token = strtok(log_details, "|");
	  strcpy(present_user, token);
	  system("clear");
	  printf("\n\t\t CURRENT USER >>> %s \t\t\n", present_user);
	  auth_fg = true;
	  break;
	  }

      //LOGIN FAIL

      if(strcmp(recv_buff,"false") == 0)
      {
	 system("clear");
         printf("Authentication failed, Username and Password did not match, Please try again \n");
	 memset(recv_buff,'\0',sizeof(recv_buff));
	 break;
      }

    }//1st while loop closed

    close(sock_fd);

    //registering to Supernode and establishing the socket connection with supernode

    if(auth_fg)
    {
	// Socket Creation :
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0) {
		fprintf(stderr, "%s\n", "error - error creating socket.");
		abort();
	}

	memcpy(self_ip, argv[2], (strlen(argv[2]) + 1 > sizeof(self_ip)) ? sizeof(self_ip) : strlen(argv[2]));
	self_port = atoi(argv[3]);
	self_addr.sin_family = AF_INET;
	self_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	self_addr.sin_port = htons(self_port);

	if (bind(sock, (struct sockaddr *)&self_addr, sizeof(self_addr))) {
		fprintf(stderr, "%s\n", "error - error binding.");
		abort();
	}

	//Getting super node details to register to it 

    	printf("Enter the super node IP address \n");
    	scanf("%s", &super_ip);
    	printf("Info : The super node IP is : %s \n", super_ip);
    	printf("Enter the super node port\n");
    	scanf("%d",&super_port);
    	printf("Info : The super node port is : %d \n",super_port);
    	printf("Self ip is : %s \n",self_ip);
    	super_addr.sin_family = AF_INET;
    	if (inet_aton(super_ip, &super_addr.sin_addr) == 0) {
		fprintf(stderr, "%s\n", "error - error parsing tracker ip.");
		abort();
	}
	super_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	super_addr.sin_port = htons(super_port);
    	/*connecting to Super node */

    	if(connect(sock,(struct sockaddr*)&super_addr,sizeof(super_addr))<0)
    	{
		printf("\nERROR connecting");
		exit(1);
    	}

	// To notify the supernode that it is a valid user("u")  : 
      
      	write(sock, "u", 1);
    	printf("\n Info : connected to super Node \n");
	/* Sending User ID and IP address to SuperNode */
	strcpy(temp,self_ip);
	strcat(temp,"|");
	strcat(temp,present_user);
	n = write(sock, temp, strlen(temp));

	if (n < 0) {
		printf("\nERROR writing to socket");
	  	exit(1);
	}

	/* Getting confirmation from super node on registartion */

    	int v = read(sock,read_buff,sizeof(read_buff));
    	strcpy(buf,read_buff);
    	int ret_v = strcmp(buf,"r");
    	if(ret_v == 0)
    	{
      		flag = 1;
      		printf("Info :Registered \n");
    	}
    	else
    	{
        	printf("Info :Not registered \n");
    	}
	int x = search();
	if(x)
	{
		//For Peer communication :
		printf("Info : Calling Peer ..... Port no : %s \n",argv[3]);
		peer_calling(atoi(argv[3]));;
	}
	if(x==0)
	{
		super_node_help();
	}

	
    }
    break;
  }//while closed
  close(sock);
}

// function to clear the buffer cache :

void stdin_buf_clean(void)
{
  int val;
  do {
    val = getchar();
  } while (val != '\n' && val != EOF);
}

//Validating the user details :

char * signup_check()
{
  char * signed_detail = malloc(20);
  printf("\nEnter Username: ");
  stdin_buf_clean();
  fgets(sign_user,sizeof(sign_user), stdin);

  if(sign_user[strlen(sign_user)-1 ]== '\n')
    {
      sign_user[strlen(sign_user)-1] = '\0';
    }
  strcpy(signed_detail, sign_user);
  return signed_detail;

}

//Sing_up function implementation

char * sign_up()
{
  char * sign = malloc(500);
  strcpy(sign, sign_user);
  strcat(sign,"|");
  char temp_v[50];
  printf("\nEnter a password : ");
  fgets(temp_v,sizeof(temp_v), stdin);

  if(temp_v[strlen(temp_v)-1 ]== '\n')
    {
      temp_v[strlen(temp_v)-1] = '\0';
    }
  strcat(sign, temp_v);
  strcat(sign,"|");
  return sign;

}

// Implementation of chat_begin method :
void chat_begin(int sock_pfd, struct sockaddr_in *peer)
{
  int ret_v;
  ssize_t byts;
  char inp_buf[1024];
  char out_buf[1024];
  struct pollfd p_fds[2];

  /* Initialing polling varibles */
  p_fds[0].fd = 0;
  p_fds[1].fd = sock_pfd;
  p_fds[0].events = POLLIN | POLLPRI;
  p_fds[1].events = POLLIN | POLLPRI;

  /* Looping to check the messages */
  while (1) {
    /* Call poll() */
    ret_v = poll(p_fds, 2, -1);

    if (ret_v < 0) {
      printf("Error - error in polling : %s\n", strerror(errno));
      break;
    }
    if (ret_v > 0) {
      // checking for errors
      if (p_fds[0].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        printf("Error - STDIN error in polling \n");
        break;
      }
      if (p_fds[1].revents & (POLLERR | POLLHUP | POLLNVAL)) {
        printf("Error - Socket error in polling \n");
        break;
      }

      /* To read data from STDIN */
      if (p_fds[0].revents & (POLLIN | POLLPRI)) {
        byts = read(0, out_buf, sizeof(out_buf));
        if (byts < 0) {
          printf("Error - error in STDIN: %s\n", strerror(errno));
          break;
        }
        printf("send msg>> %.*s\n", (int)byts, out_buf);
        byts = sendto(sock_pfd, out_buf, byts, 0,
                       (struct sockaddr *)peer, sizeof(struct sockaddr_in));
        if (byts < 0) {
          printf("Error - error in sending the message : %s\n", strerror(errno));
          break;
        }
      }

      /* To read data from socket by polling  */
      if (p_fds[1].revents & (POLLIN | POLLPRI)) {
        byts = recvfrom(sock_pfd, inp_buf, sizeof(inp_buf),
                         0, NULL, NULL);
        if (byts < 0) {
          printf("Error - error in receving the data : %s\n", strerror(errno));
          break;
        }
        if (byts > 0) {
          printf("recv msg>> %.*s\n", (int)byts, inp_buf);
        }
      }
    }
  }
}

// Calling peer_calling method:
void peer_calling(unsigned long val)
{
   unsigned long loc_portno;
  int sock_pfd;
  struct sockaddr_in serv_addrs, peer_adrs;

  printf("\nPrinting remote : %d and IP : %s \n",rem_portno,peer_ip);

  loc_portno = val;
  if (loc_portno < 1 || loc_portno > 65535) {
    printf("Error - Local port '%ld' is Invalid\n", loc_portno);
    exit(1);
  }

  if (rem_portno < 1 || rem_portno > 65535) {
    printf("Error - Remote port '%ld' is Invalid \n", rem_portno);
    exit(1);
  }

  /* Parsing argument for connection establishment */
  peer_adrs.sin_family = AF_INET;
  peer_adrs.sin_port = htons(rem_portno);
  if (inet_aton(peer_ip, &peer_adrs.sin_addr) == 0) {
    printf("Error - Remote IP '%s' is Invalid \n", peer_ip);
    exit(1);
  }

  /* Socket creation */
  sock_pfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock_pfd < 0) {
    printf("Error - socket creation error: %s\n", strerror(errno));
    exit(1);
  }

  /* Socket binding*/
  serv_addrs.sin_family = AF_INET;
  serv_addrs.sin_addr.s_addr = htonl(INADDR_ANY);
  serv_addrs.sin_port = htons(loc_portno);
  if (bind(sock_pfd, (struct sockaddr *)(&serv_addrs),
           sizeof(serv_addrs)) < 0) {
    printf("Error - Socket binding error : %s\n", strerror(errno));
    exit(1);
  }

  /* calling the method to start the chat between peers */
  chat_begin(sock_pfd, &peer_adrs);

  close(sock_pfd);
}


// Search method for checking whether user is known(in friend list) or unknown :

int search()
{
  strcpy(file_name,present_user);
  strcat(file_name,".txt");
  printf("File name is %s \n",file_name);
  /*Searching in users friend list */
  fname = malloc(500);
  if (flag)
  {
  	printf("\n\n Enter the friend name you wanna chat with : \n");
	scanf("%s",fname);
	int n = lines_in_file(f);
	char line_file[1024];
	int i,flag_var;
	char * literal;
	printf("Checking the presence of user %s in file \n", fname);
	printf("File name is %s \n", file_name);
	f = fopen(file_name, "a");
	fclose(f);
	f = fopen(file_name, "r");
	for(i=0 ; i < n ; i++)
	{
		if(fgets(line_file, 1024,(FILE *)f)!=NULL)
		{
			printf("line :%s" , line_file);
		}
		else
    		{
      			break;
    		}
		literal = strtok(line_file,"|");
		if(strcmp(literal,fname)==0)
		{
      			printf("Info: user found \n");
			literal = strtok(NULL,"|");
			strcpy(peer_ip,literal);
			rem_portno = atoi(strtok(NULL,"|"));
			printf("Info: peer ip is %s \n",peer_ip);
			printf("Info: remote port is %d \n",rem_portno);
			flag_var=1;
			break;
		}
		else
		{
			flag_var=0;
		}
	}

	fclose(f);
	if(flag_var==1)
	{
		return 1;
	}
	else
	{
		printf("\n User not found in Friend List \n");
		return 0;
	}
      
    }
  else
  {
    printf("\n\n ***** Try registering to another super node******\n");
  }
}


// Contacting the Super Node to fetch the user information :

void super_node_help()
{
  printf("\n Info: Getting help from super node \n");
  memset(send_buff,'\0',sizeof(send_buff));
  char *user_tmp[20];
  int pno;
   
      // Sending unmatched User name to Supernode
      strcpy(send_buff,"NoMatch");
      strcat(send_buff,"|");
      strcat(send_buff,fname);
      int s = write(sock,send_buff,strlen(send_buff));
      // Enter the peer Information obtained from Super node:
      printf("\n Enter the Peer Information got from Super Node :\n");
      printf("Enter the peer_ip \n");
      scanf("%s",peer_ip);
      printf("peer_ip is %s \n ",peer_ip);
      printf("Enter remote port \n");
      scanf("%d",&rem_portno);
      printf("Adding user info %s to friend list \n",user_tmp);
      f = fopen(file_name, "a");
      if (f == NULL)
      {
	    printf("Error opening file!\n");
	    exit(1);
      }
      strcpy(user_tmp,fname);
      strcat(user_tmp,"|");
      strcat(user_tmp,peer_ip);
      strcat(user_tmp,"|");
      char pnum[100];
      sprintf(pnum,"%d",rem_portno);
      strcat(user_tmp,pnum);
      fprintf(f,"%s \n",user_tmp);
      printf("\n Info: friend added in list \n");
      fclose(f);
      peer_calling(self_port);
      
      
}
      
// To find numner of friends :      
int lines_in_file(FILE *file_pt)
{

	int n=0,ch_count=0;
	file_pt = fopen("user_details.txt", "r");
	while ((ch_count= fgetc(file_pt)) != EOF)
	{
		if (ch_count == '\n')
		n++;
	}
	fclose(file_pt);
	return n;

}
