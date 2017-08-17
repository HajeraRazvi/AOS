 // Implementation of Login Authentication Server

#include<stdio.h>
#include<netdb.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<string.h>
#include<netinet/in.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<unistd.h>
#include<fcntl.h>
#define LEN 1024

char * login_sys(char ini_buf[LEN]);
void signup_sys(char ini_buf[LEN]);
int lines_in_file(FILE *file_pt);
int user_authentication(FILE *file_pt, char usr_list[20], char pwd[20], int n);
char * checkuser(FILE *file_pt, char usr_list[1024], int n);
char present_usr[20];

// Implementation of Login function :

char * login_sys(char ini_buf[LEN])
{
	int no_lines, check;
	FILE * file_pt;
	char * literal, usr_name[20], passwrd[20];
	char * login_authen = malloc(20);
	memset(present_usr,0,20);
	memset(usr_name,0,20);
	memset(passwrd,0,20);
	literal = strtok(ini_buf,"|");
	literal = strtok(NULL, "|");
	strcpy(usr_name, literal);
	printf("\nuser name = %s",usr_name);
	literal = strtok(NULL, "|");
	strcpy(passwrd,literal);
	//printf("\npassword = %s",passwrd);
	no_lines = lines_in_file(file_pt);
	strcpy(present_usr,usr_name);
	check = user_authentication(file_pt, usr_name, passwrd,no_lines);
	if(check == 1 )
	{
		return "true";
	}
	if(check == 0)
	{
		return "false";
	}
}

// MAIN Function


int main(int argc, char *argv[])
{
struct sockaddr_in ser_add,clin_add;
int fir_sockfd,sec_sockfd;
int i,j,len_cli,n,check;
char rec_buf[1024], send_buf[1024], tempVar;
char usr_name[1204],passwrd[1204], temp_buf[1024], *login_authen , *valid_usr;
FILE *file_pt_cfg;
FILE *file_pt;
char *literal;
int pid;
int no_lines;
int temp[50], flag_var = 1;
//printf("Printing the port no : %d ",port_num);

	/* Argument Check */
	if (argc < 2){
		printf("Usage: Port_Num");
		exit(1);
	}

	/* Creating the Socket for client communication : */

	fir_sockfd = socket(AF_INET,SOCK_STREAM,0);
	if(fir_sockfd < 0)
	{
		printf("Error: Socket creation");
		exit(1);
	}

	/*Initializtion of Socket*/

	bzero((char *)&ser_add,sizeof(ser_add));
	ser_add.sin_family = AF_INET;
	ser_add.sin_addr.s_addr = INADDR_ANY;
	ser_add.sin_port = htons(atoi(argv[1]));

	/*Binding of Socket*/

	if (bind(fir_sockfd, (struct sockaddr *) &ser_add, sizeof(ser_add)) < 0)
	{
		printf("\nError: Socket bind error ");
		exit(1);
	}

	/* Implementation of listen client call */
	listen(fir_sockfd,5);
	len_cli = sizeof(clin_add);

	/*Accept connecttion from client*/
	for(;;) {
		sec_sockfd = accept(fir_sockfd,(struct sockaddr *)&clin_add,&len_cli);
		if (sec_sockfd < 0){
			printf("\nError: Error in Socket accept");
			exit(1);
		}
		pid = fork();
		if (pid == -1) {
			printf("\nError: Forking error ");
			exit(1);
		}
		if (pid == 0) {
			setbuf(stdout,NULL);
			close(fir_sockfd);
			while(1) {
				memset(rec_buf, '\0', sizeof(rec_buf));
				n = read(sec_sockfd, rec_buf, sizeof(rec_buf));
				if (n < 0) {
					printf("Error in read\n");
					exit(1);
				}
				if(rec_buf[strlen(rec_buf)-1] == '\n'){
					  rec_buf[strlen(rec_buf)-1]= '\0';
				}
				strcpy(temp_buf,rec_buf);
				literal = strtok(temp_buf,"|");
				tempVar = rec_buf[0];
				
				if(tempVar == 'l')
				{
					memset(send_buf, '\0', sizeof(send_buf));
					strcpy(send_buf,"login");
				}
				if(tempVar == 'r')
				{
					memset(send_buf, '\0', sizeof(send_buf));
					strcpy(send_buf,"signup");
				}
				/* when login option is choosed */
				if(strcmp(literal, "login")==0)
				{
					printf("\n Login Info : %s ", rec_buf);
					login_authen = login_sys(rec_buf);

					if(strcmp(login_authen,"true")==0)
					{
						memset(send_buf, '\0', sizeof(send_buf));
						strcpy(send_buf, "true");
					}
					else if(strcmp(login_authen,"false")==0)
					{
						memset(send_buf, '\0', sizeof(send_buf));
						strcpy(send_buf, "false");
					}
				}
				// When signup option is choosed
				if(strcmp(literal, "signup")==0)
				{
					no_lines = lines_in_file(file_pt);
					valid_usr = checkuser(file_pt,rec_buf,no_lines );
					if(strcmp(valid_usr,"exist")==0)
					{
						memset(send_buf, '\0', sizeof(send_buf));
						strcpy(send_buf, "exist");
					}
					else if(strcmp(valid_usr,"dont")==0)
					{
						memset(send_buf, '\0', sizeof(send_buf));
						strcpy(send_buf, "dont");
					}
				}
				if(strcmp(literal, "dosignup")==0)
				{
					printf("\n Signup Info : signing up for new user : ");
					signup_sys(rec_buf);
					memset(send_buf, '\0', sizeof(send_buf));
					strcpy(send_buf, "login");
				}
				n = write(sec_sockfd,send_buf, strlen(send_buf)+1);
				if(n < 0){
					printf("\nError: Write Error");
					exit(1);
				}
			}

			exit(0);
		}
		else {
			close(sec_sockfd);
		}
	}
} // End of Main


//to implement the signup of user:

void signup_sys(char ini_buf[LEN])
{
	char usr_name[50], passwrd[50];
	char *literal;
	char signin[LEN];
	FILE *file_pt_cfg;
	literal = strtok(ini_buf,"|");
	literal = strtok(NULL,"|");
	strcpy(usr_name, literal);
	literal = strtok(NULL,"|");
	strcpy(passwrd, literal);
	file_pt_cfg = fopen("user_details.txt", "a");
	fprintf(file_pt_cfg,"%s:%s\n",usr_name,passwrd);
	fclose(file_pt_cfg);
}

// to the check the presence of user in the file :user_details.txt

char * checkuser(FILE *file_pt, char users[1024], int n)
{
	char * validate_user = malloc(20);
	char line_file[1024];
	int i,flag_var;
	char * literal;
	char usr[20];
	literal = strtok(users,"|");
	literal = strtok(NULL,"|");
	strcpy(usr,literal);
	printf("\n --> %s user", usr);
	file_pt = fopen("user_details.txt", "r");
	for(i=0 ; i < n ; i++)
	{
		if(fgets(line_file, 1024,(FILE *)file_pt)!=NULL)
		{
			//printf("line :%s" , line_file);
		}
		literal = strtok(line_file,":");
		if(strcmp(literal,usr)==0)
		{
			flag_var=1;
			break;
		}
		else
		{
			flag_var=0;
		}
	}

	fclose(file_pt);
	if(flag_var==1)
	{
		return "exist";
	}
	else
	{
		return "dont";
	}

}


//check authentication
int user_authentication(FILE *file_pt, char usr_list[20], char pwd[20], int n)
{
	char line_file[1024];
	int i,flag_var;
	int checkuser = 1;
	char * literal;
	file_pt = fopen("user_details.txt", "r");
	for(i=0 ; i <= n ; i++)
	{
		if(fgets(line_file, 1024,(FILE *)file_pt)!=NULL)
		{

			if(line_file[strlen(line_file)-1 ]== '\n')
			{
			line_file[strlen(line_file)-1] = '\0';
			}

			literal = strtok(line_file,":");
			if(i<n)
			{
				if(checkuser==1)
				{
					if(strcmp(literal,usr_list)==0)
					{

						checkuser=0;
						flag_var=1;
					}
				}
			}
			if((i==n) && (checkuser==1))
			{

				flag_var=0;
				break;
			}
			if(flag_var==1)
			{
				literal = strtok(NULL,":");
				if(strcmp(literal,pwd)==0)
				{
					flag_var=1;
					break;
				}
				else
				{

					flag_var=0;
					break;
				}

			}
		}

	}
	fclose(file_pt);
	return flag_var;
}

// To count the number of users in the file : user_details.txt

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
