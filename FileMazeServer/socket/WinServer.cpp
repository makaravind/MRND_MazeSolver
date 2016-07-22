#define _CRT_SECURE_NO_WARNINGS
#include "stdafx.h"
#include <winsock2.h>
#include <windows.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h> 
struct node
{
	char msg[128];
	int msg_id;
	node *next;
}*flist,*alist,*printid;

struct bufserv{
	
		int userId;
		int forumId;
		int msgId;
		int commentId;
		int choice;
		char *forumname;
		char msg[128];
}buf1;
int currx, curry; // reading and populating the maze array
bool flag=true;
int mid = 0;
int count1 =0;
char *Data[100];
int count=1;
int values[100];
DWORD WINAPI SocketHandler(void*);
void replyto_client(char *buf, int *csock);

void socket_server() {

	//The port you want the server to listen on
	int host_port= 1101;

	unsigned short wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
 	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 || ( LOBYTE( wsaData.wVersion ) != 2 ||
		    HIBYTE( wsaData.wVersion ) != 2 )) {
	    fprintf(stderr, "No sock dll %d\n",WSAGetLastError());
		goto FINISH;
	}

	//Initialize sockets and set options
	int hsock;
	int * p_int ;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if(hsock == -1){
		printf("Error initializing socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if( (setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		goto FINISH;
	}
	free(p_int);

	//Bind and listen
	struct sockaddr_in my_addr;
	my_addr.sin_family = AF_INET ;
	my_addr.sin_port = htons(host_port);
	
	memset(&(my_addr.sin_zero), 0, 8);
	my_addr.sin_addr.s_addr = INADDR_ANY ;
	
	/* if you get error in bind 
	make sure nothing else is listening on that port */
	if( bind( hsock, (struct sockaddr*)&my_addr, sizeof(my_addr)) == -1 ){
		fprintf(stderr,"Error binding to socket %d\n",WSAGetLastError());
		goto FINISH;
	}
	if(listen( hsock, 10) == -1 ){
		fprintf(stderr, "Error listening %d\n",WSAGetLastError());
		goto FINISH;
	}
	
	//Now lets do the actual server stuff

	int* csock;
	sockaddr_in sadr;
	int	addr_size = sizeof(SOCKADDR);
	
	while(true){
		printf("waiting for a connection\n");
		csock = (int*)malloc(sizeof(int));
		
		if((*csock = accept( hsock, (SOCKADDR*)&sadr, &addr_size))!= INVALID_SOCKET ){
			//printf("Received connection from %s",inet_ntoa(sadr.sin_addr));
			CreateThread(0,0,&SocketHandler, (void*)csock , 0,0);
		}
		else{
			fprintf(stderr, "Error accepting %d\n",WSAGetLastError());
		}
	}

FINISH:
;
}
int p[10][2];
int count_p;
bool isSafe(int **maze, int x, int y, int x2, int y2)
{
	// if (x,y outside maze) return false
	if (x >= 0 && x <= x2 && y >= 0 && y <= y2 && maze[x][y] == 1 && x != 0 && y != 0)
		return true;
	return false;
}
void printSolution(int **sol, int m, int n)
{
	for (int i = 0; i < m; i++)
	{
		for (int j = 0; j < n; j++)
			printf(" %d ", sol[i][j]);
		printf("\n");
	}
}
bool solveMazeUtil(int **maze, int x, int y, int x2, int y2, int **sol)
{
	// if (x,y is goal) return true
	if (x == x2 && y == y2)
	{
		sol[x][y] = 1;
		p[count_p][0] = x;
		p[count_p][1] = y;
		count_p += 1;
		return true;
	}
	if (isSafe(maze, x, y, x2, y2) == true)
	{
		sol[x][y] = 1;

		if (solveMazeUtil(maze, x + 1, y, x2, y2, sol) == true){
			p[count_p][0] = x;
			p[count_p][1] = y;
			count_p += 1;
			return true;
		}


		if (solveMazeUtil(maze, x, y + 1, x2, y2, sol) == true){
			p[count_p][0] = x;
			p[count_p][1] = y;
			count_p += 1;
			return true;
		}

		if (solveMazeUtil(maze, x - 1, y, x2, y2, sol) == true){
			p[count_p][0] = x;
			p[count_p][1] = y;
			count1 += 1;
			return true;
		}

		if (solveMazeUtil(maze, x, y - 1, x2, y2, sol) == true){
			p[count_p][0] = x;
			p[count_p][1] = y;
			count_p += 1;
			return true;
		}


		sol[x][y] = 0;
		return false;
	}

	return false;
}
bool solveMaze(int **maze, int m, int n)
{


	int **sol = (int**)calloc(m, sizeof(int)*m);
	for (int j = 0; j < n; j++){
		sol[j] = (int*)calloc(n, sizeof(int));
	}
	int x1 = 1, y1 = 1, x2 = 2, y2 = 2;
	if (solveMazeUtil(maze, x1, y1, x2, y2, sol) == false)
	{
		printf("Solution doesn't exist");
		return false;
	}

	printSolution(sol, m, n);
	return true;
}
int getsocket()
{
	int hsock;
	int * p_int;
	hsock = socket(AF_INET, SOCK_STREAM, 0);
	if (hsock == -1){
		printf("Error initializing socket %d\n", WSAGetLastError());
		return -1;
	}

	p_int = (int*)malloc(sizeof(int));
	*p_int = 1;
	if ((setsockopt(hsock, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1) ||
		(setsockopt(hsock, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1)){
		printf("Error setting options %d\n", WSAGetLastError());
		free(p_int);
		return -1;
	}
	free(p_int);

	return hsock;
}
void printpath(int m, int n, int* csock){

	printf("sending this to client\n");
	for (int i = 0; i < count_p; i++)
		printf("%d %d", p[i][0], p[i][1]);
	int bytecount;
	char buffer[1024];
	int i,j;
	// copy from path to buufer
	for ( i = 0, j = 0; i < 2 * count_p; i += 2, j++){
		buffer[i] = p[j][0] + '0';
		buffer[i + 1] = p[j][1] + '0' ;
	}
	buffer[i] = '\0';
	printf("%s", buffer);
	if ((bytecount = send(*csock, buffer, strlen(buffer), 0)) == SOCKET_ERROR){
		fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
		free(csock);
	}
}
void process_input(char *recvbuf, int recv_buf_cnt, int* csock) 
{

	char replybuf[1024]={'\0'};
	recvbuf[recv_buf_cnt] = '\0';
	printf("%s",recvbuf);
	strcpy(replybuf, recvbuf);
	printf("%s", recvbuf);
	//replyto_client(recvbuf, csock);
	//replybuf[0] = '\0';
}
void replyto_client(char *buf, int *csock) {
	int bytecount;
	
	if((bytecount = send(*csock, buf, strlen(buf), 0))==SOCKET_ERROR){
		fprintf(stderr, "Error sending data %d\n", WSAGetLastError());
		free (csock);
	}
	printf("replied to client: %s\n",buf);
}
int GetInt(char c){
	return c - '0';
}
void Populate(char *recvbuf, int recv_byte_cnt, int **maze, int m,int n){

	int i;
	int j;
	int r_ptr = 0;
	recvbuf[recv_byte_cnt] = '\0';
	for ( i = currx; i < m; i++){
		for ( j = curry; j < n; ){
			if (isdigit(recvbuf[r_ptr])){
				maze[i][j] = GetInt(recvbuf[r_ptr]);
				j++;
			}
			r_ptr += 1;
		}
	}
	printSolution(maze, m, n);
	currx = i;
	curry = j;	
}
DWORD WINAPI SocketHandler(void* lp){
    int *csock = (int*)lp;

	char recvbuf[1024];
	char MNrecvbuff[3];
	int recvbuf_len = 1024;
	int recv_byte_cnt;
	int m, n;
	
	//memset(recvbuf, 0, recvbuf_len);

	if ((recv_byte_cnt = recv(*csock, MNrecvbuff, sizeof(MNrecvbuff), 0)) == SOCKET_ERROR){
		fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
		free (csock);
		return 0;
	}
	m = GetInt(MNrecvbuff[0]);
	n = GetInt(MNrecvbuff[1]);
	int packet_no=0;
	int **maze = (int**)calloc(m, sizeof(int)*m);
	for (int j = 0; j < n; j++){
		maze[j] = (int*)calloc(n, sizeof(int));
	}
	while (1){

		if ((recv_byte_cnt = recv(*csock, recvbuf, sizeof(recvbuf), 0)) == SOCKET_ERROR){
			fprintf(stderr, "Error receiving data %d\n", WSAGetLastError());
			free(csock);
			return 0;
		}
		packet_no += 1;
		Populate(recvbuf, recv_byte_cnt, maze, m, n);
		if ((m*n) * 2 < 1024*packet_no) break;
	}
	//printf("Received bytes %d\nReceived string \"%s\"\n", recv_byte_cnt, recvbuf);
	solveMaze(maze, m, n);
	printpath(m,n,csock);
	//process_input(MNrecvbuff, recv_byte_cnt, csock);

    return 0;
}