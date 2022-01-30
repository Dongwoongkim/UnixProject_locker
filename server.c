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

// 락커 구조체 
struct rocker
{
	int id;
	int empty;
	char pwd[10];
};


int main()
{	
	// 서버에서 사용할 변수들 
	char s[100],Rmessage[300],Smessage[300],useid[100],Rpassword[10];
	char receivedPassword[10],sendpassword[10];	
	char QuestionFromClient,emptynum,r,Possible;
	int clientfd, serverfd,k,clientlen,rID,status,child;
	
	// 락커 구조체 포인터 선언 
	struct rocker *rec2;

	// 소켓 구조체
	struct sockaddr_un serverUNIXaddr, clientUNIXaddr;

	// 서버 소켓 생성 
	serverfd = socket(AF_UNIX,SOCK_STREAM,DEFAULT_PROTOCOL);
	serverUNIXaddr.sun_family = AF_UNIX;
	strcpy(serverUNIXaddr.sun_path,"manager");
	unlink("manager");


	// 서버 소켓 bind()
	bind(serverfd,(struct sockaddr *)&serverUNIXaddr, sizeof(serverUNIXaddr));

	// 서버 최초실행시 사물함 세팅

	printf("사물함 개수를 입력하세요 : ");

	// 사물함 개수 입력 ( 문자열 )
	scanf("%s",s);


	// 사물함 개수 Save to int type in k
	k = atoi(s);

	printf("* 최대 고객수는 5명입니다. *\n");


	// 구조체 포인터 동적할당 ( 사물함 개수 만큼 )
	rec2 = (struct rocker *) malloc((k+1)*sizeof(struct rocker));
		
	printf("\n* * * * * * * * * * * * *  Locker * * * * * * * * * * * * * \n");
	printf("*ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ*\n");

	// 1번 사물함부터 k번 사물함까지 사물함 초기상태 setting
	for(int i=1;i<=k;i++)
	{
	rec2[i].id = i;
	rec2[i].empty = 1;
	strcpy(rec2[i].pwd,"0000");
	printf("|      Rocker Num : %d        |       Empty :     Yes       |\n",i);
	printf("*ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ * ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ*\n");
	}
	
	// 사물함 개수기록할 fd 
	int fd;
	mkfifo("index",0666);
	fd = open("index",O_WRONLY);
	
	// 대기 큐 생성 (클라이언트 최대 연결개수 5개)
	listen(serverfd,5);

	while(1)
	{
	
		// 클라이언트 시작시에 서버에서 진행할 업무를 자식프로세스로 생성하기 위해 pid = fork() 사용할 예정
		int pid;

		// 클라이언트로부터  연결요청 수락 
		clientlen = sizeof(clientUNIXaddr);
		clientfd = accept(serverfd,(struct sockaddr *) &clientUNIXaddr,&clientlen);	
		if(clientfd == -1) printf("error\n");

		// 파이프 생성 for 클라이언트로부터 업데이트된 사물함상태를 서버에 Update and save 하기위해
		int lfd[2];
		pipe(lfd);
		

		// for 클라이언트가 실행되면 서버에서 수행할 작업을 자식프로세스로 처리 
		if( (pid=fork())==0)
		{
			close(lfd[0]);

			// (1) - fd에 사물함개수 s write 
			write(fd,s,100);


			// (2) - 현재 사물함 상태 클라이언트에 전달 
			// 사물함이 비어있으면 emptynum='1'을 clientfd에 기록
			// 사물함이 비어있지 않으면 emptynum='0'을 clientfd에 기록	
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

			// 사물함 사용할 모드 - 클라이언트로 부터 받은 string이 '66096301' -> 채팅모드 
			// 													 else 사물함번호 -> 사물함사용모드 
			while(1)
			{

			// (3)-1. 클라이언트로부터 읽은 문자가 c면 채팅모드 시작
				read(clientfd,&r,1);

				if(r=='c')
				{
					printf("\n\n* 고객으로부터 채팅이 왔습니다. 채팅을 시작합니다.\n");		
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

				// (3)-2. 클라이언트로부터 읽은 문자가 n이면 사물함 사용모드로 진입
				else if(r=='n')
				{
				// (4) - clientfd에 기록된 사물함번호를 useid에 저장
				read(clientfd,&useid,100);
				rID = atoi(useid);

					// 사물함이 비어있는 경우 
					if(rec2[rID].empty==1)
					{	
					Possible = 'Y';

					// (5)- clientfd에 'Y'를 write 
					write(clientfd,&Possible,1);

					// (6)- clientfd에 write한 password를 Rpassword에 read()
					read(clientfd,&Rpassword,sizeof(Rpassword));

					// Rpassword를 비밀번호 설정완료  
					strcpy(rec2[rID].pwd, Rpassword);
					
					// 사물함 empty = 0으로 설정 
					rec2[rID].empty=0;
			
					}

					// 사물함이 비어있지않은 경우 
					else if(rec2[rID].empty==0)
					{	
						Possible='N';

						// (5)- clientfd에 'N'를 write  
						write(clientfd,&Possible,1);

						strcpy(sendpassword,rec2[rID].pwd);

						// (7) - 클라이언트에서 입력한 비밀번호를 Inputpwd에 write 
						read(clientfd,&receivedPassword,sizeof(receivedPassword));

						// (8) - 사물함정보에 기록된 비밀번호를 sendpassword에 write
						write(clientfd, &sendpassword, sizeof(sendpassword));
						if(!strcmp(receivedPassword,rec2[rID].pwd))
						{
							rec2[rID].empty=1;
							strcpy(rec2[rID].pwd,"0000");
						}
					}

					printf("\n**************  Updated Rocker Information  **************\n");
					printf("*ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ*ㅡㅡㅡㅡㅡㅡㅡㅡ*ㅡㅡㅡㅡㅡㅡㅡㅡ*\n");


					// 바뀐  사물함 상태  - 비밀번호, 비어있는 여부, 아이디  
					for(int i=1;i<=k;i++)
					{	
						if(rec2[i].empty==0)
						{
							printf("|   Rocker Num : %d  |   Empty : No   |  Password : %s |\n",rec2[i].id,rec2[i].pwd);
							printf("*ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ*ㅡㅡㅡㅡㅡㅡㅡㅡ*ㅡㅡㅡㅡㅡㅡㅡㅡ*\n");
						}
						else
						{
							printf("|   Rocker Num : %d  |   Empty : Yes   |  Password : %s |\n",rec2[i].id,rec2[i].pwd);
							printf("*ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ*ㅡㅡㅡㅡㅡㅡㅡㅡ*ㅡㅡㅡㅡㅡㅡㅡㅡ*\n");

						}
					}
					
				}		
			
			char QuestionFromClient;

			// (9) - 클라이언트 종료여부를 QuestionFromClient에 read()
			read(clientfd,&QuestionFromClient,1);
			if(QuestionFromClient=='N') break;
			// 클라이언트 종료시 와일문 빠져나옴 
			}

			// (10) - 클아이언트에서 업데이트된 사물함 정보 파이프에 기록 
			for(int i=1;i<=k;i++)
			write(lfd[1],&rec2[i],sizeof(struct rocker));
			
		} // fork 


		// 클라이언트 종료 -> if문 빠져나옴  
		child = wait(&status);
		
		close(lfd[1]);

		// (10) - 파이프에 기록된 업데이트된 사물함 상태 서버에 저장 
		for(int i=1;i<=k;i++)
		{
			read(lfd[0],&rec2[i],sizeof(struct rocker));
		}

	close(clientfd);
	
	} // 클라이언트 종료시 다음 실행한 클라이언트 진행  while문  

	return 0;
}
