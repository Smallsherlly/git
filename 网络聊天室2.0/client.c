#include "struct.h"


char user_name[20] = {};
char buf[255] = {};
char getin[255] = {};
int fd_c = 0;
int flag = 0;
int key =0;

SA ss_addr;
SA recv_addr ;
SA check_addr;
MSG msg;
SA send_addr;
socklen_t len = sizeof(send_addr);

int create_sockfd(void)
{
	int fd = socket(AF_INET,SOCK_DGRAM,0);
	if(0 >= fd)
	{
		perror("sockfd");
		return -1;
	}
	return fd;
}



void* recv_func(void* arg)
{
	int i = 0;
	char ans;
	
	char ss_name[20] = {};
	
	while(1)
	{	
		char str[255] = {};
		//接收消息
		recvfrom(fd_c,&msg.msg[msg.msg_num],sizeof(char)*255,0,(SP)&check_addr,&len);
		msg.msg_num++;
		if(ntohs(check_addr.sin_port)!=ntohs(ss_addr.sin_port)&&ntohs(check_addr.sin_port)!=ntohs(recv_addr.sin_port))
		{
			flag = 0;
		}
		//接收到线程发送的消息，开头为 “s#s“  即私聊
		sscanf(msg.msg[msg.msg_num-1],"%s %s",str,ss_name);
		
		if(str[0]=='c'&&str[1]=='#'&&str[2]=='c')
		{
			recvfrom(fd_c,&ss_addr,sizeof(SA),0,NULL,NULL);
			flag = 1;
		}
		if(str[0]=='s'&&str[1]=='#'&&str[2]=='s')
		{
			recvfrom(fd_c,&ss_addr,sizeof(SA),0,NULL,NULL);
			key = 1;
		}
	
		//发送 “ #q#”退出私聊
		if(flag==1)
		{
			if(ss_name[0]=='#'&&ss_name[1]=='q'&&ss_name[2]=='#')
			{
				sendto(fd_c,ss_name,strlen(ss_name)+1,0,(SP)&ss_addr,len);
				flag = 0;
			}	
		}
		
		system("clear");
		printf("%s\n",getin);
		for(i=0; i<msg.msg_num; i++)
		{
			printf("%s\n",msg.msg[i]);
		}
		if(key==1)
			printf("%s向你发出了私聊请求，是否同意？（y／其他）\n",ss_name);
		printf("\n--------------\n");
		printf("%s:",user_name);
		fflush(stdout);//清空输出缓存区
	}
		
}

void* inp_func(void* arg)
{
	char str[255] = {};
	while(1)
	{
			usleep(400000);
			
			gets(buf);
		
			if(key)
			{
				if(buf[0] == 'y')
				{
					flag = 1;		
				}
			}
			key = 0;
			sprintf(str,"%s:%s",user_name,buf);
			if(flag == 0)
			{
				sendto(fd_c,str,strlen(str)+1,0,(SP)&send_addr,len);
			}
			else if(flag == 1)
			{
				char sl[255] = {};
				int i = 0;
				sprintf(sl,"私聊:%s",str);
				sendto(fd_c,sl,strlen(sl)+1,0,(SP)&recv_addr,len);
				sendto(fd_c,sl,strlen(sl)+1,0,(SP)&ss_addr,len);

			}
	

	}
}

int main()
{
	srand(time(NULL));
	
	//服务端通信地址
	
	send_addr.sin_family = AF_INET;
	send_addr.sin_port = htons(12346);
	send_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	//随机产生客户端通信端口号
	recv_addr = send_addr;
	recv_addr.sin_port = htons(rand()%30000+1024);
	fd_c = create_sockfd();
	
	int ret = bind(fd_c,(SP)&recv_addr,sizeof(recv_addr));
	if(0 > ret)
	{
		perror("bind");
		return -1;
	}


	printf("请输入您的用户名：");
	scanf("%s",user_name);
	getchar();
	pthread_t pth_id1,pth_id2;
	
	sendto(fd_c,user_name,sizeof(recv_addr),0,(SP)&send_addr,sizeof(send_addr));
	recvfrom(fd_c,getin,sizeof(char)*255,0,(SP)&send_addr,&len);
	
	//建立发送和接收两个子线程
	ret = pthread_create(&pth_id1,NULL,recv_func,user_name);
	if(0 > ret)
	{
		perror("pthread_create");
		return -1;
	}
	
	ret = pthread_create(&pth_id2,NULL,inp_func,user_name);
	if(0 > ret)
	{
		perror("pthread_create");
		return -1;
	}
	
	pthread_join(pth_id1,NULL);
	pthread_join(pth_id2,NULL);
	
}
