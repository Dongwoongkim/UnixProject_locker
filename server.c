#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#include <unistd.h>
#include <sys/wait.h>
#define DEFAULT_PROTOCOL 0
#define START 1

// ��Ŀ ����ü 
struct rocker
{
	int id;
	int empty;
	char pwd[10];
};


int main()
{	
	// �������� ����� ������ 
	char s[100],Rmessage[300],Smessage[300],useid[100],Rpassword[10];
	char receivedPassword[10],sendpassword[10];	
	char QuestionFromClient,emptynum,r,Possible;
	int clientfd, serverfd,k,clientlen,rID,status,child;
	
	// ��Ŀ ����ü ������ ���� 
	struct rocker *rec2;

	// ���� ����ü
	struct sockaddr_un serverUNIXaddr, clientUNIXaddr;

	// ���� ���� ���� 
	serverfd = socket(AF_UNIX,SOCK_STREAM,DEFAULT_PROTOCOL);
	serverUNIXaddr.sun_family = AF_UNIX;
	strcpy(serverUNIXaddr.sun_path,"manager");
	unlink("manager");


	// ���� ���� bind()
	bind(serverfd,(struct sockaddr *)&serverUNIXaddr, sizeof(serverUNIXaddr));

	// ���� ���ʽ���� �繰�� ����

	printf("�繰�� ������ �Է��ϼ��� : ");

	// �繰�� ���� �Է� ( ���ڿ� )
	scanf("%s",s);


	// �繰�� ���� Save to int type in k
	k = atoi(s);

	printf("* �ִ� ������ 5���Դϴ�. *\n");


	// ����ü ������ �����Ҵ� ( �繰�� ���� ��ŭ )
	rec2 = (struct rocker *) malloc((k+1)*sizeof(struct rocker));
		
	printf("\n* * * * * * * * * * * * *  Locker * * * * * * * * * * * * * \n");
	printf("*�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�*\n");

	// 1�� �繰�Ժ��� k�� �繰�Ա��� �繰�� �ʱ���� setting
	for(int i=1;i<=k;i++)
	{
	rec2[i].id = i;
	rec2[i].empty = 1;
	strcpy(rec2[i].pwd,"0000");
	printf("|      Rocker Num : %d        |       Empty :     Yes       |\n",i);
	printf("*�ѤѤѤѤѤѤѤѤѤѤѤѤѤ� * �ѤѤѤѤѤѤѤѤѤѤѤѤѤ�*\n");
	}
	
	// �繰�� ��������� fd 
	int fd;
	mkfifo("index",0666);
	fd = open("index",O_WRONLY);
	
	// ��� ť ���� (Ŭ���̾�Ʈ �ִ� ���ᰳ�� 5��)
	listen(serverfd,5);

	while(1)
	{
	
		// Ŭ���̾�Ʈ ���۽ÿ� �������� ������ ������ �ڽ����μ����� �����ϱ� ���� pid = fork() ����� ����
		int pid;

		// Ŭ���̾�Ʈ�κ���  �����û ���� 
		clientlen = sizeof(clientUNIXaddr);
		clientfd = accept(serverfd,(struct sockaddr *) &clientUNIXaddr,&clientlen);	
		if(clientfd == -1) printf("error\n");

		// ������ ���� for Ŭ���̾�Ʈ�κ��� ������Ʈ�� �繰�Ի��¸� ������ Update and save �ϱ�����
		int lfd[2];
		pipe(lfd);
		

		// for Ŭ���̾�Ʈ�� ����Ǹ� �������� ������ �۾��� �ڽ����μ����� ó�� 
		if( (pid=fork())==0)
		{
			close(lfd[0]);

			// (1) - fd�� �繰�԰��� s write 
			write(fd,s,100);


			// (2) - ���� �繰�� ���� Ŭ���̾�Ʈ�� ���� 
			// �繰���� ��������� emptynum='1'�� clientfd�� ���
			// �繰���� ������� ������ emptynum='0'�� clientfd�� ���	
		 	for(int i=1;i<=k;i++)
		 	{	
				if(rec2[i].empty==1)
				{
				emptynum='1';
				write(clientfd,&emptynum,sizeof(emptynum));
				}
			
				else
				{
				emptynum='0';
				write(clientfd,&emptynum,sizeof(emptynum));
				}
			}

			// �繰�� ����� ��� - Ŭ���̾�Ʈ�� ���� ���� string�� '66096301' -> ä�ø�� 
			// 													 else �繰�Թ�ȣ -> �繰�Ի���� 
			while(1)
			{

			// (3)-1. Ŭ���̾�Ʈ�κ��� ���� ���ڰ� c�� ä�ø�� ����
				read(clientfd,&r,1);

				if(r=='c')
				{
					printf("\n\n* �����κ��� ä���� �Խ��ϴ�. ä���� �����մϴ�.\n");		
					while(1)
					{
						read(clientfd,&Rmessage,sizeof(Rmessage));
						printf("Client : %s\n",Rmessage);
						if(!strcmp(Rmessage,"Q")) break;
						else
						{
						printf("Me : ");
						scanf(" %[^\n]s",Smessage);
						write(clientfd,&Smessage,sizeof(Smessage));
						}
					}
					continue;
				}

				// (3)-2. Ŭ���̾�Ʈ�κ��� ���� ���ڰ� n�̸� �繰�� ������ ����
				else if(r=='n')
				{
				// (4) - clientfd�� ��ϵ� �繰�Թ�ȣ�� useid�� ����
				read(clientfd,&useid,100);
				rID = atoi(useid);

					// �繰���� ����ִ� ��� 
					if(rec2[rID].empty==1)
					{	
					Possible = 'Y';

					// (5)- clientfd�� 'Y'�� write 
					write(clientfd,&Possible,1);

					// (6)- clientfd�� write�� password�� Rpassword�� read()
					read(clientfd,&Rpassword,sizeof(Rpassword));

					// Rpassword�� ��й�ȣ �����Ϸ�  
					strcpy(rec2[rID].pwd, Rpassword);
					
					// �繰�� empty = 0���� ���� 
					rec2[rID].empty=0;
			
					}

					// �繰���� ����������� ��� 
					else if(rec2[rID].empty==0)
					{	
						Possible='N';

						// (5)- clientfd�� 'N'�� write  
						write(clientfd,&Possible,1);

						strcpy(sendpassword,rec2[rID].pwd);

						// (7) - Ŭ���̾�Ʈ���� �Է��� ��й�ȣ�� Inputpwd�� write 
						read(clientfd,&receivedPassword,sizeof(receivedPassword));

						// (8) - �繰�������� ��ϵ� ��й�ȣ�� sendpassword�� write
						write(clientfd, &sendpassword, sizeof(sendpassword));
						if(!strcmp(receivedPassword,rec2[rID].pwd))
						{
							rec2[rID].empty=1;
							strcpy(rec2[rID].pwd,"0000");
						}
					}

					printf("\n**************  Updated Rocker Information  **************\n");
					printf("*�ѤѤѤѤѤѤѤѤѤ�*�ѤѤѤѤѤѤѤ�*�ѤѤѤѤѤѤѤ�*\n");


					// �ٲ�  �繰�� ����  - ��й�ȣ, ����ִ� ����, ���̵�  
					for(int i=1;i<=k;i++)
					{	
						if(rec2[i].empty==0)
						{
							printf("|   Rocker Num : %d  |   Empty : No   |  Password : %s |\n",rec2[i].id,rec2[i].pwd);
							printf("*�ѤѤѤѤѤѤѤѤѤ�*�ѤѤѤѤѤѤѤ�*�ѤѤѤѤѤѤѤ�*\n");
						}
						else
						{
							printf("|   Rocker Num : %d  |   Empty : Yes   |  Password : %s |\n",rec2[i].id,rec2[i].pwd);
							printf("*�ѤѤѤѤѤѤѤѤѤ�*�ѤѤѤѤѤѤѤ�*�ѤѤѤѤѤѤѤ�*\n");

						}
					}
					
				}		
			
			char QuestionFromClient;

			// (9) - Ŭ���̾�Ʈ ���Ῡ�θ� QuestionFromClient�� read()
			read(clientfd,&QuestionFromClient,1);
			if(QuestionFromClient=='N') break;
			// Ŭ���̾�Ʈ ����� ���Ϲ� �������� 
			}

			// (10) - Ŭ���̾�Ʈ���� ������Ʈ�� �繰�� ���� �������� ��� 
			for(int i=1;i<=k;i++)
			write(lfd[1],&rec2[i],sizeof(struct rocker));
			
		} // fork 


		// Ŭ���̾�Ʈ ���� -> if�� ��������  
		child = wait(&status);
		
		close(lfd[1]);

		// (10) - �������� ��ϵ� ������Ʈ�� �繰�� ���� ������ ���� 
		for(int i=1;i<=k;i++)
		{
			read(lfd[0],&rec2[i],sizeof(struct rocker));
		}

	close(clientfd);
	
	} // Ŭ���̾�Ʈ ����� ���� ������ Ŭ���̾�Ʈ ����  while��  

	return 0;
}
