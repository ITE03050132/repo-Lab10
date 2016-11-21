#include<stdio.h>
#include<string.h>    //strlen
#include<stdlib.h>    //strlen
#include<sys/socket.h>
#include<arpa/inet.h> //inet_addr
#include<unistd.h>    //write
#include<pthread.h> //for threading , link with lpthread
#include<time.h>
void *connection_handler(void *);
int main(int argc , char *argv[])
{
int socket_desc , new_socket , c , *new_sock,Ans;
struct sockaddr_in server , client;
char *message;
//Create socket
time_t mytime;
mytime=time(NULL);
Ans = (rand()%8999)+1000;
socket_desc = socket(AF_INET , SOCK_STREAM , 0);
if (socket_desc == -1)
{
printf("Could not create socket");
}
//Prepare the sockaddr_in structure
server.sin_family = AF_INET;
server.sin_addr.s_addr = INADDR_ANY;
server.sin_port = htons( 8888 );
//Bind
if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
{
puts("bind failed");
return 1;
}
puts("bind done");
//Listen
listen(socket_desc , 3);
//Accept and incoming connection
puts("Waiting for incoming connections...");
c = sizeof(struct sockaddr_in);
while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
{
puts("Connection accepted");
printf("Ans:%d",Ans);
//Reply to the client
message = "Hello Client , I have received your connection. And now I will assign a handler for you\n";
write(new_socket , message , strlen(message));
pthread_t sniffer_thread;
new_sock = malloc(1);
*new_sock = new_socket;
if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
{
perror("could not create thread");
return 1;
}
//Now join the thread , so that we dont terminate before the thread
//pthread_join( sniffer_thread , NULL);
puts("Handler assigned");
}
if (new_socket<0)
{
perror("accept failed");
return 1;
}
return 0;
}
void *connection_handler(void *socket_desc)
{
//Get the socket descriptor
int sock = *(int*)socket_desc,c,i,j,CAns,Ans,AAns[4],ACAns[4],A=0,B=0,AC,AA;
int read_size;
char *message , client_message[2000];
//Send some messages to the client
read_size = recv(sock , client_message , 2000 , 0);
A=0;
B=0;
AA = Ans;
bzero(client_message,4);
read(sock,client_message,4);
CAns = (client_message[0]-'0')*1000+(client_message[1]-'0')*100+(client_message[2]-'0')*10+(client_message[3]-'0');
ACAns[0] = CAns/1000;
CAns-=ACAns[0]*1000;
ACAns[1] = CAns/100;
CAns-=ACAns[1]*100;
ACAns[2] = CAns/10;
CAns-=ACAns[2]*10;
ACAns[3] = CAns/1;
AAns[0] = AA/1000;
AA-=AAns[0]*1000;
AAns[1] = AA/100;
AA-=AAns[1]*100;
AAns[2] = AA/10;
AA-=AAns[2]*10;
AAns[3] = AA/1;
for(i = 0;i<=3;i++)
{
if(ACAns[i]==AAns[i])
{
A++;
}
}
for(j=0;j<=3;j++)
{
for(i=0;i<=3;i++)
{
if(ACAns[i]==AAns[j]&&i!=j)
{
B++;
}
}
}
if(A==4)
{
message = "You Are RIGHT!!!";
write(sock, message, strlen(message)+1);
}
else
{
sprintf(message,"%dA%dB",A,B);
write(sock , message, strlen(message)+1);
}
//Receive a message from client

if(read_size == 0)
{
puts("Client disconnected");
fflush(stdout);
}
else if(read_size == -1)
{
perror("recv failed");
}
//Free the socket pointer
free(socket_desc);
return 0;
}
