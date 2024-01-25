#include <iostream>  // All the essential libraries
#include <string>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/uio.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <fstream>
using namespace std;
//admin side implementation

// IMP NOTE: I have used Beej's Network Programming using C++, ChatGPT and https://github.com/bozkurthan/Simple-TCP-Server-Client-CPP-Example as references
 
int main()
{
    //we need 2 things: ip address and port number, in that order
    const char* servername = "localhost";   // loopback/self address. 127.0.0.1
    int port = 45074; 
    char msg[15000];   // msg buffer
// standard client socket creation and connection establishment procedure
    struct hostent* host = gethostbyname(servername);   
    sockaddr_in sendSockAddr;   
    bzero((char*)&sendSockAddr, sizeof(sendSockAddr)); 
    sendSockAddr.sin_family = AF_INET; 
    sendSockAddr.sin_addr.s_addr = 
        inet_addr(inet_ntoa(*(struct in_addr*)*host->h_addr_list));
    sendSockAddr.sin_port = htons(port);
    int clientSd = socket(AF_INET, SOCK_STREAM, 0);  // a TCP Socket
    int status = connect(clientSd,
                         (sockaddr*) &sendSockAddr, sizeof(sendSockAddr));
    if(status < 0)
    {
        cout<<"Error connecting to socket!"<<endl; // unsuccessful connection
        return -1;
    }
    cout << "Admin is up and running" << endl;
    int bytesRead, bytesWritten = 0;
    while(1)
    {
        cout << "Department Name: ";
        string data;
        getline(cin, data);
		cout<<endl;
		cout<<"Student ID ('Enter' to skip): ";  // either query the student data or just query the department statistics.
		string sata;
		getline(cin,sata);
		data="admin"+data+" "+sata;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        strcpy(msg, data.c_str());
        if(data == "exit")
        {
            send(clientSd, (char*)&msg, strlen(msg), 0);  // send to server
            break;
        }
        bytesWritten += send(clientSd, (char*)&msg, strlen(msg), 0);
        cout << "Client has sent ";
		cout<<msg;
		cout<<" to Main server using TCP." << endl;
        memset(&msg, 0, sizeof(msg));//clear the buffer
        bytesRead += recv(clientSd, (char*)&msg, sizeof(msg), 0);  // read out the received message from server
        if(!strcmp(msg, "exit"))
        {
            cout << "Server has quit the session" << endl;
            break;
        }
        cout <<msg << endl;
		cout<<"====Start a New Query===="<<endl;    // start the process all over again anew!
    }
    close(clientSd);

    return 0;    
}
