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
	// Ŭ���̾�Ʈ���� ����� ������ 
	int InputMoney,money,clientfd,result,fd;
	char useid[100],Smessage[300],Rmessage[300],Inputpwd[10],Outputpwd[10],password[10],password2[10],index[100],question;
	char emptynum,m,Possible;
	int j,k,Intuseid;
	
	// Ŭ���̾�Ʈ ���� ���� 
	struct sockaddr_un serverUNIXaddr;
	clientfd = socket(AF_UNIX, SOCK_STREAM, DEFAULT_PROTOCOL);
	serverUNIXaddr.sun_family = AF_UNIX;
	strcpy(serverUNIXaddr.sun_path,"manager");
	
	// ������ ���� ��û 
	do{ 
	result = connect(clientfd, (struct sockaddr *)&serverUNIXaddr, sizeof(serverUNIXaddr));
	sleep(1);
	}while(result!=-1);
	
	// (1) �������� fd�� ����� �繰�԰����� index�� read
	fd = open("index",O_RDONLY);
	read(fd,index,100);

	// �繰�� ���� char* to int & save to k
	k = atoi(index);
	

	// (2) �繰�� ���� ���
	printf("****************  Rocker information ****************\n");
	printf("�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�\n");
	for(int i=1;i<=k;i++)
	{	

		// (2) - ������ clientfd�� write�� emptynum�� Ŭ���̾�Ʈ�� char* emtpynum�� read() 
		// emptynum = '1'�̸� if�� ����
		// emptynum = '0'�̸� else-if�� ���� �Ͽ� �繰�� ���� ���
		read(clientfd,&emptynum, sizeof(emptynum));
		j = emptynum-'0';
		if(j==1)
		{
		printf("|      Rocker Num : %d        |       Empty :     Yes      |\n",i);
		printf("�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�\n");
		}
		else if(j==0)
		{
		printf("|      Rocker Num : %d        |       Empty :     No       |\n",i);
		printf("�ѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤѤ�\n");
		}
	}

	// �繰�� ��� ��� 

	while(1)
	{	
		printf("\n\n�繰���� �̿��Ͻ÷��� �繰�� ��ȣ�� �Է��ϼ���. * �繰�� ��ȣ (1 to %d)\n�Ǵ� �����ڿ��� �����Ϸ��� 66096301�� �Է����ּ���\n : ",k);
		scanf("%s",useid);
		Intuseid = atoi(useid);

			// �Է��� ��ȣ�� 66096301 -> ä�� ���� 
			if(!strcmp(useid,"66096301"))
			{	
				// (3)-1. ä�ø������ ������ ���� 
				m='c';
	 			write(clientfd,&m,1);
				printf("\n�����ڿ��� ä���� �����մϴ�. �����Ͻ÷��� Q�� �Է��ϼ��� \n");
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

			// �Է��� ��ȣ�� �� �繰�� �������� ũ�� -> while loop cotinue
			else if(Intuseid>k)
			{
			continue;
			}
			
			// (3)-2. �繰�� ��ȣ�� �Է½� -> �繰�� ����� 
			else 
			{	
				m='n';

				// (3)-2. ������ �繰�� ��������� �˸��� ���� clientfd�� 'n'�� write()
		 		write(clientfd,&m,1);
				sleep(1);

				// (4) - �Է��� �繰�Թ�ȣ�� clientfd�� write
				write(clientfd,&useid,sizeof(useid));

				// (5) - �繰���� ��밡�ɿ��θ� �о���δ�.
				read(clientfd,&Possible,1);


				// ����ִ� �繰�Կ� �����ϴ� ���  
				if(Possible=='Y')
				{
					money = 1000;

						// �̿�� 1000���� ���� 
						while(1)
						{
							printf("�繰���� �̿��Ͻ÷��� %d���� �־��ּ��� : ",money);
							scanf("%d",&InputMoney);
							money -= InputMoney;
							if(money==0) break;
							else if( money < 0 )
							{
							printf("�Ž������� �������� -----> %d��  <-----\n",-money);
							break;
							}
						}	

					// ��й�ȣ �Է� 
					printf("%s�� �繰���� ��й�ȣ�� �������ּ��� : ",useid);
					scanf("%s",password);
					
					// + �߰���� : ��й�ȣȮ�������� ���Է� 
					while(1)
					{
					printf("�����Ͻ� ��й�ȣ�� �ٽ� �Է����ּ��� : ");
					scanf("%s",password2);
					if(!strcmp(password,password2)) break;
					}

					// (6) - ��й�ȣ�� client�� write() 
					write(clientfd,&password,sizeof(password));
				}	

				// ������� ���� �繰�Կ� �����ϴ� ��� 
				else if(Possible=='N')
				{
				printf("%s�� �繰���� ����� ��ġ���� ��й�ȣ�� �Է��ϼ��� : ",useid);
				scanf("%s", Inputpwd);


				// (7)- �Է��� ��й�ȣ�� Inputpwd�� write 
				write(clientfd,&Inputpwd,sizeof(Inputpwd));

				sleep(1);

				// (8)- �繰�������� ��ϵ� ��й�ȣ�� outputpwd�� read()
				read(clientfd,&Outputpwd,sizeof(Outputpwd));

					// ���� outputpwd�� Inputpwd�� ������ 
					if(!strcmp(Outputpwd,Inputpwd))
					{
						printf("����� �����մϴ�..\n\n");
					}

					// �׷��� ������ ��� print�� while loop continue
					else if(strcmp(Outputpwd,Inputpwd))
					{
					printf("��й�ȣ�� Ʋ�Ƚ��ϴ�.\n �ʱ�ȭ������ ���ư��ϴ�.\n\n");
					continue;
					}
				}
			}

	printf("����Ͻðڽ��ϱ�? (Y/N) : ");
	scanf(" %c",&question);

	// Ŭ���̾�Ʈ�� ����Ǿ����� �˸��� ���� clientfd�� N write 
	write(clientfd,&question,1);
	
	// N�Է½� Ŭ���̾�Ʈ ���� 
	if(question=='N')
	{
		break;
	}

	}


	return 0;
} 
