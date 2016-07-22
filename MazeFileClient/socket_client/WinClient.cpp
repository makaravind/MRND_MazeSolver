#define _CRT_SECURE_NO_WARNINGS
#include <winsock2.h>
#include <StdAfx.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <conio.h>
int getsocket()
{
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		return -1;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return -1;
	}
	free(p_int);

	return hsock;
}
int GetInt(char c){
	return c - '0';
}
void socket_client()
{

	//The port and address you want to connect to
	int host_port = 1101;
	char* host_name = "127.0.0.1";

	//Initialize socket support WINDOWS ONLY!
	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD(2, 2);
	err = WSAStartup(wVersionRequested, &wsaData);
	if (err != 0 || (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)) {
		fprintf(stderr, "Could not find sock dll %d\n", WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set any options

	//Connect to the server
	struct sockaddr_in my_addr;

	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(host_port);

	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = inet_addr(host_name);

	//if( connect( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
	//	fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
	//	goto FINISH;
	//}

	//Now lets do the client related stuff
	char buffer[1024];
	char MNbuff[3];
	int buffer_len = 1024;
	int bytecount;
	int c;
	int m, n;
	FILE *fp;
	fp = fopen("maze.txt", "r");
	int last;
	char *total_file_buf;
	fseek(fp, 0, SEEK_END);
	last = ftell(fp);
	rewind(fp);
	total_file_buf = (char*)malloc(sizeof(char)*(last + 1));
	last = fread(total_file_buf, sizeof(char), last, fp);
	total_file_buf[last] = '\0';
	printf("%s", total_file_buf);
	
	int recv_byte_cnt;
	
	while(true) {

		int hsock = getsocket();
		//add error checking on hsock...
		if( connect(hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == SOCKET_ERROR ){
			fprintf(stderr, "Error connecting socket %d\n", WSAGetLastError());
			goto FINISH;
		}

		//memset(buffer, '\0', buffer_len);
		/*
		printf("Enter your message to send here\n");
		for(char* p=buffer ; (c=getch())!=13 ; p++){
		printf("%c", c);
		*p = c;
		}
		*/
		MNbuff[0] = total_file_buf[0];
		MNbuff[1] = total_file_buf[2];
		MNbuff[2] = '\0';
		m = GetInt(MNbuff[0]);
		n = GetInt(MNbuff[1]);

		if ((bytecount = send(hsock, MNbuff, strlen(MNbuff), 0)) == SOCKET_ERROR){
			fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
			goto FINISH;
		}
		printf("Sent bytes %d\n", bytecount);

		while (1){

			for (int i = 0; i < 1024; i++) buffer[i] = total_file_buf[i+3];

			if ((bytecount = send(hsock, buffer, sizeof(buffer), 0)) == SOCKET_ERROR){
				fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
				goto FINISH;
			}

			if (m * n < 1024) break;
		}
		if ((recv_byte_cnt = recv(hsock, buffer, sizeof(buffer), 0)) == SOCKET_ERROR){
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
			goto FINISH;
		}
		/*
			if((bytecount = recv(hsock, buffer, buffer_len, 0))==SOCKET_ERROR){
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
			goto FINISH;
		}
		*/
		
		//printf("Recieved bytes %d\nReceived string \"%s\"\n", bytecount, buffer);
		buffer[recv_byte_cnt] = '\0';
		printf("%s\n",buffer);

		closesocket(hsock);
		break;
	}
	fclose(fp);
	fp = fopen("maze.txt", "w+");
	fseek(fp, 0, SEEK_END);
	fprintf( fp, "%s", buffer);
	fclose(fp);
	
	//closesocket(hsock);
FINISH:
;
}