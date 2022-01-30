#include <stdio.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <netinet/in.h>
#define DEFAULT_PROTOCOL 0
#define START 1

int main()
{	
	// 클라이언트에서 사용할 변수들 
	int InputMoney,money,clientfd,result,fd;
	char useid[100],Smessage[300],Rmessage[300],Inputpwd[10],Outputpwd[10],password[10],password2[10],index[100],question;
	char emptynum,m,Possible;
	int j,k,Intuseid;
	
	// 클라이언트 소켓 생성 
	struct sockaddr_un serverUNIXaddr;
	clientfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverUNIXaddr.sun_family = AF_UNIX;
	strcpy(serverUNIXaddr.sun_path,"manager");
	
	// 서버에 연결 요청 
	do{ 
	result = connect(clientfd, (struct sockaddr *)&serverUNIXaddr, sizeof(serverUNIXaddr));
	sleep(1);
	}while(result!=-1);
	
	// (1) 서버에서 fd에 기록한 사물함개수를 index에 read
	fd = open("index",O_RDONLY);
	read(fd,index,100);

	// 사물함 개수 char* to int & save to k
	k = atoi(index);
	

	// (2) 사물함 정보 출력
	printf("****************  Rocker information ****************\n");
	printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
	for(int i=1;i<=k;i++)
	{	

		// (2) - 서버가 clientfd에 write한 emptynum을 클라이언트의 char* emtpynum에 read() 
		// emptynum = '1'이면 if문 실행
		// emptynum = '0'이면 else-if문 실행 하여 사물함 정보 출력
		read(clientfd,&emptynum, sizeof(emptynum));
		j = emptynum-'0';
		if(j==1)
		{
		printf("|      Rocker Num : %d        |       Empty :     Yes      |\n",i);
		printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
		}
		else if(j==0)
		{
		printf("|      Rocker Num : %d        |       Empty :     No       |\n",i);
		printf("ㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡㅡ\n");
		}
	}

	// 사물함 사용 모드 

	while(1)
	{	
		printf("\n\n사물함을 이용하시려면 사물함 번호를 입력하세요. * 사물함 번호 (1 to %d)\n또는 관리자에게 문의하려면 66096301을 입력해주세요\n : ",k);
		scanf("%s",useid);
		Intuseid = atoi(useid);

			// 입력한 번호가 66096301 -> 채팅 시작 
			if(!strcmp(useid,"66096301"))
			{	
				// (3)-1. 채팅모드임을 서버에 전달 
				m='c';
	 			write(clientfd,&m,1);
				printf("\n관리자와의 채팅을 시작합니다. 종료하시려면 Q를 입력하세요 \n");
				while(1)
				{
					printf("Me : ");
					scanf(" %[^\n]s",Smessage);	
					write(clientfd,&Smessage,sizeof(Smessage));
					if(!strcmp(Smessage,"Q")) break;
					else
					{	
					read(clientfd, &Rmessage, sizeof(Rmessage));
					printf("Manager : %s\n",Rmessage);
					}
				}
			continue;	
			}

			// 입력한 번호가 총 사물함 개수보다 크면 -> while loop cotinue
			else if(Intuseid>k)
			{
			continue;
			}
			
			// (3)-2. 사물함 번호를 입력시 -> 사물함 사용모드 
			else 
			{	
				m='n';

				// (3)-2. 서버에 사물함 사용모드임을 알리기 위해 clientfd에 'n'을 write()
		 		write(clientfd,&m,1);
				sleep(1);

				// (4) - 입력한 사물함번호를 clientfd에 write
				write(clientfd,&useid,sizeof(useid));

				// (5) - 사물함의 사용가능여부를 읽어들인다.
				read(clientfd,&Possible,1);


				// 비어있는 사물함에 접근하는 경우  
				if(Possible=='Y')
				{
					money = 1000;

						// 이용료 1000원을 투입 
						while(1)
						{
							printf("사물함을 이용하시려면 %d원을 넣어주세요 : ",money);
							scanf("%d",&InputMoney);
							money -= InputMoney;
							if(money==0) break;
							else if( money < 0 )
							{
							printf("거스름돈을 받으세요 -----> %d원  <-----\n",-money);
							break;
							}
						}	

					// 비밀번호 입력 
					printf("%s번 사물함의 비밀번호를 설정해주세요 : ",useid);
					scanf("%s",password);
					
					// + 추가기능 : 비밀번호확인을위해 재입력 
					while(1)
					{
					printf("설정하실 비밀번호를 다시 입력해주세요 : ");
					scanf("%s",password2);
					if(!strcmp(password,password2)) break;
					}

					// (6) - 비밀번호를 client에 write() 
					write(clientfd,&password,sizeof(password));
				}	

				// 비어있지 않은 사물함에 접근하는 경우 
				else if(Possible=='N')
				{
				printf("%s번 사물함을 사용을 마치려면 비밀번호를 입력하세요 : ",useid);
				scanf("%s", Inputpwd);


				// (7)- 입력한 비밀번호를 Inputpwd에 write 
				write(clientfd,&Inputpwd,sizeof(Inputpwd));

				sleep(1);

				// (8)- 사물함정보에 기록된 비밀번호를 outputpwd에 read()
				read(clientfd,&Outputpwd,sizeof(Outputpwd));

					// 만약 outputpwd와 Inputpwd가 같으면 
					if(!strcmp(Outputpwd,Inputpwd))
					{
						printf("사용을 종료합니다..\n\n");
					}

					// 그렇지 않으면 경고문 print후 while loop continue
					else if(strcmp(Outputpwd,Inputpwd))
					{
					printf("비밀번호가 틀렸습니다.\n 초기화면으로 돌아갑니다.\n\n");
					continue;
					}
				}
			}

	printf("계속하시겠습니까? (Y/N) : ");
	scanf(" %c",&question);

	// 클라이언트가 종료되었음을 알리기 위해 clientfd에 N write 
	write(clientfd,&question,1);
	
	// N입력시 클라이언트 종료 
	if(question=='N')
	{
		break;
	}

	}


	return 0;
} 
