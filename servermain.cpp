#include <fstream>
#include <iostream>             // All the essential Libraries
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
#include<set>
#include<map>
#include<vector>
using namespace std;
struct sockaddr_in sa;
#define PORT "45074"
//#define MAXBUFLEN 100
#define BACKLOG 3

// I have used Beej's Network Programming and ChatGPT as primary references. 

// some preconditional functions neccessary to form a socket
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

void sigchld_handler(int s)
{
    // waitpid() might overwrite errno, so we save and restore it:
    int saved_errno = errno;

    while(waitpid(-1, NULL, WNOHANG) > 0);

    errno = saved_errno;
}

int main()
{
    
 /*
** 
*/
int clientid=0;
int sockfd, new_fd;  // listen on sock_fd, new connection on new_fd
    struct addrinfo hints, *servinfo, *p;
    struct sockaddr_storage their_addr; // connector's address information
	
    socklen_t sin_size;
    struct sigaction sa;
    int yes=1;
    char s[INET6_ADDRSTRLEN];
    int rv;

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE; // use my IP (in this case the  local machine)

/* server a,b,c addresses*/
		 struct sockaddr_in serverAAddr, serverBAddr, serverCAddr;
    socklen_t serverAAddrLen = sizeof(serverAAddr);     // address references for backend servers A, B and C
    socklen_t serverBAddrLen = sizeof(serverBAddr);
    socklen_t serverCAddrLen = sizeof(serverCAddr);

    bzero((char*)&serverAAddr, sizeof(serverAAddr));
    bzero((char*)&serverBAddr, sizeof(serverBAddr));
    bzero((char*)&serverCAddr, sizeof(serverCAddr));

    serverAAddr.sin_family = AF_INET;  // IPv4 format
    serverBAddr.sin_family = AF_INET;
    serverCAddr.sin_family = AF_INET;

    serverAAddr.sin_addr.s_addr = inet_addr("127.0.0.1");  // loopback address
    serverBAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverCAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    serverAAddr.sin_port = htons(41074);   // port no of server A
    serverBAddr.sin_port = htons(42074);   // port no of server B
    serverCAddr.sin_port = htons(43074);   // port no of server C
    if ((rv = getaddrinfo(NULL, PORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("server: socket");
            continue;
        }

        if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes,
                sizeof(int)) == -1) {
            perror("setsockopt");
            exit(1);
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("server: bind");
            continue;
        }

        break;
    }

    freeaddrinfo(servinfo); // all done with this structure, used an approach as in Beej's

    if (p == NULL)  {
        fprintf(stderr, "server: failed to bind\n");
        exit(1);
    }

    if (listen(sockfd, BACKLOG) == -1) {
        perror("listen");
        exit(1);
    }

    sa.sa_handler = sigchld_handler; // reap all dead processes
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
        perror("sigaction");
        exit(1);
    }

cout<<"Main Server is up and running"<<endl;
/*create an UDP socket to interact and get the dept list*/
		int udpSocket = socket(AF_INET, SOCK_DGRAM, 0); 
		struct sockaddr_in serverAddr;  //udpSocket
		socklen_t serverAddrLen = sizeof(serverAddr);
		bzero((char*)&serverAddr, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    serverAddr.sin_port = htons(44074);  // port no
    bind(udpSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)); 
/* getting all the necessary details from the backend servers */
char suffer[15000];
	string serverADepartments;  // string which stores server A's list
    string serverBDepartments;   //  ''  '' B
    string serverCDepartments;   // "" "" C
	sendto(udpSocket, "deptlist",sizeof("deptlist"), 0, (struct sockaddr*)&serverAAddr, serverAAddrLen);       // send the datagram conisiting of the string "deptlist" to the desired UDP server
    recvfrom(udpSocket, suffer, sizeof(suffer), 0, (struct sockaddr*)&serverAAddr, &serverAAddrLen);     // receive data from a backend server and store it in a buffer
    serverADepartments=suffer;

       
		
		cout <<" Main Server has received the department list from Backend server A using UDP over port 44074 "<<endl;
        cout<<"Server A:  "<<serverADepartments<<endl; 
		 
		 // The above procedure is repeated for server B and C respectively
		sendto(udpSocket, "deptlist",sizeof("deptlist"), 0, (struct sockaddr*)&serverBAddr, serverBAddrLen);
    recvfrom(udpSocket, suffer, sizeof(suffer), 0, (struct sockaddr*)&serverBAddr, &serverBAddrLen);
serverBDepartments = suffer;
		cout << " Main Server has received the department list from Backend server B using UDP over port 44074 "<<endl;
        cout<<"Server B: "<<serverBDepartments<<endl; 
		
		sendto(udpSocket, "deptlist",sizeof("deptlist"), 0, (struct sockaddr*)&serverCAddr, serverCAddrLen);
    recvfrom(udpSocket, suffer, sizeof(suffer), 0, (struct sockaddr*)&serverCAddr, &serverCAddrLen);
    serverCDepartments= suffer;
		cout << "Main Server has received the department list from Backend server C using UDP over port 44074 "<<endl;
        cout<<"Server C: "<<serverCDepartments<<endl; 
		cout << endl; 
		
		/* Finished getting the department lists */ 
        
    while(1) {  // main tcp accept() loop
        sin_size = sizeof their_addr;
        new_fd = accept(sockfd, (struct sockaddr *)&their_addr, &sin_size);
        if (new_fd == -1) {
            perror("accept");
            continue;
        }
		clientid+=1;

        inet_ntop(their_addr.ss_family,
            get_in_addr((struct sockaddr *)&their_addr),
            s, sizeof s);
        

        if (!fork()) { // this is the child process
		string resp;
	/* udp socket*/
		char buffer[1000];
		char duffer[1000];
		char response[1000];
            close(sockfd); // child doesn't need the listener
			while(true){
				memset(buffer, 0, sizeof(buffer));
				   int bytesRead = recv(new_fd, buffer, sizeof(buffer), 0);
				   string msg=buffer;
				   // student or admin split condition, student here
				   if(!(msg.find("admin")!= string::npos)){
				   int space_pos = msg.find(" ");
				   
				   string deptname=msg.substr(0,space_pos);
				   string stuid=msg.substr(space_pos+1);
				   cout<<"Main Server has received the request on Student ";
				   cout<<stuid;
				   cout<<" in ";
				   cout<<deptname;
				   cout<<" from student client "<<clientid<<" using TCP over port 45074"<<endl;
				   //string response1="Thank you for contacting me the main server";
				   //send(new_fd, response1.c_str(),response1.length(),0);
				   
				   /* if dept name found in server A */
				   if(serverADepartments.find(deptname) != string :: npos){  
		    cout<<deptname+" shows up in Server A"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<stuid;
			cout<<" to server A using UDP over port 44074"<<endl;
			string temp=deptname+" "+stuid;
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverAAddr, serverAAddrLen);

        // Receive the  response from the backend server
        recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAAddr, &serverAAddrLen);
		// Display the response to the user
		cout<<"The Main Server has received the searching result(s) of student "+stuid+" from Backend Server A"<<endl;
        cout << " "<< buffer << endl;
		resp=buffer;
				   }
		
		 // if dept name found in server B 
		else if(serverBDepartments.find(deptname) != string :: npos){
		cout<<deptname+" shows up in Server B"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<stuid;
			cout<<" to server B using UDP over port 44074"<<endl;
			string temp=deptname+" "+stuid;
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverBAddr, serverBAddrLen);
			
         recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverBAddr, &serverBAddrLen);
         cout<<"The Main Server has received the searching result(s) of student "+stuid+" from Backend Server B"<<endl;
		 cout <<" "<< buffer << endl;
		 resp=buffer;
	}
	//else if found in C 
	else if(serverCDepartments.find(deptname) != string :: npos){
		cout<<deptname+" shows up in Server C"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<stuid;
			cout<<" to server C using UDP over port 44074"<<endl;
        string temp=deptname+" "+stuid;
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverCAddr, serverCAddrLen);
         recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverCAddr, &serverCAddrLen);
                   cout<<"The Main Server has received the searching result(s) of student "+stuid+" from Backend Server C"<<endl;

		  cout << " " << buffer << endl;
		  resp=buffer;
	}
	
	else{
		resp="department Name "+deptname+" not found";
		cout<<" "<<"Department "<<deptname<<" not found"<<endl;
		cout<<"The Main Server has sent the searching result(s) to the student client "<<clientid<<"using TCP over port 45074"<<endl;
	}
	send(new_fd, resp.c_str(), resp.length(),0);  // send the final result to the student
	
				   }
	
	          /* The response to the student client */
				   //string tcpresponse="Hey Student, I have received "+msg+" from you"; 
					//tcpresponse+="Backend Server's response: "+msg;
				  // int bytessent=send(new_fd, tcpresponse.c_str(), tcpresponse.length(),0);   }       // wierd with sizeof() and length() operators
				   
				   
				   // admin here
				   else{
					   int space_pos = msg.find(" ");
				  // cout<<"Spacepos is"<<space_pos<<endl;
				   string deptname=msg.substr(5,space_pos-5);
				   string stuid=msg.substr(space_pos+1);
				   if(stuid!=""){
				   cout<<"Main Server has received the request on student  ";
				   cout<<stuid;
				   cout<<" in ";
				   cout<<deptname;
				   cout<<" from admin client "<<clientid<<"using TCP over port 45074"<<endl;
				     /* if dept name found in server A */
				   if(serverADepartments.find(deptname) != string :: npos){  
		    cout<<deptname+" shows up in Server A"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<stuid;
			cout<<" to server A using UDP over port 44074"<<endl;
			string temp=deptname+" "+stuid;
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverAAddr, serverAAddrLen);

        // Receive the  response from the backend server
        recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAAddr, &serverAAddrLen);
		// Display the response to the user
		cout<<"The Main Server has received the searching result(s) of student "+stuid+" from Backend Server A"<<endl;
        cout << " "<< buffer << endl;
		resp=buffer;
				   }
		
		 // if dept name found in server B 
		else if(serverBDepartments.find(deptname) != string :: npos){
		cout<<deptname+" shows up in Server B"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<stuid;
			cout<<" to server B using UDP over port 44074"<<endl;
			string temp=deptname+" "+stuid;
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverBAddr, serverBAddrLen);
			
         recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverBAddr, &serverBAddrLen);
         cout<<"The Main Server has received the searching result(s) of student "+stuid+" from Backend Server B"<<endl;
		 cout <<" "<< buffer << endl;
		 resp=buffer;
	}
	//else if found in C 
	else if(serverCDepartments.find(deptname) != string :: npos){
		cout<<deptname+" shows up in Server C"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<stuid;
			cout<<" to server C using UDP over port 44074"<<endl;
        string temp=deptname+" "+stuid;
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverCAddr, serverCAddrLen);
         recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverCAddr, &serverCAddrLen);
                   cout<<"The Main Server has received the searching result(s) of student "+stuid+" from Backend Server C"<<endl;

		  cout << " " << buffer << endl;
		  resp=buffer;
	}
	
	else{
		resp="department Name "+deptname+" not found";
		cout<<"Department "<<deptname<<" not found"<<endl;
		
	}
	cout<<"The Main Server has sent the searching result(s) to the admin client "<<clientid<<"using TCP over port 45074"<<endl;

	send(new_fd, resp.c_str(), resp.length(),0);  // send the final result to the student

				   }
				   else{   // if the admin queried department statistics
					   cout<<"Main Server has received the request on Department ";
				   cout<<deptname;
				   cout<<" from admin client "<<clientid<<"using TCP over port 45074"<<endl;
				   /* if dept name found in server A */
				   if(serverADepartments.find(deptname) != string :: npos){  
		    cout<<deptname+" shows up in Server A"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<deptname;
			cout<<" Statistics to server A using UDP over port 44074"<<endl;
			string temp=deptname+" ";
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverAAddr, serverAAddrLen);

        // Receive the  response from the backend server
        recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverAAddr, &serverAAddrLen);
		// Display the response to the user
		cout<<"The Main Server has received the statistics of "+deptname+" from Backend Server A"<<endl;
        cout << " "<< buffer << endl;
	   resp=buffer;
				   }
				   
				   else if(serverBDepartments.find(deptname) != string :: npos){
		cout<<deptname+" shows up in Server B"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<deptname;
			cout<<" Statistics to server B using UDP over port 44074"<<endl;
			string temp=deptname+" ";
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverBAddr, serverBAddrLen);
			
         recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverBAddr, &serverBAddrLen);
         cout<<"The Main Server has received the statistics of "+deptname+" from Backend Server B"<<endl;
		 cout <<" "<< buffer << endl;
		 resp=buffer;
	}
	
	 else if(serverCDepartments.find(deptname) != string :: npos){
		cout<<deptname+" shows up in Server C"<<endl;
			cout<<"The Main Server has sent request for ";
			cout<<deptname;
			cout<<" to server C using UDP over port 44074"<<endl;
			string temp=deptname+" ";
        sendto(udpSocket, temp.c_str(), temp.length(), 0, (struct sockaddr*)&serverCAddr, serverCAddrLen);
			
         recvfrom(udpSocket, buffer, sizeof(buffer), 0, (struct sockaddr*)&serverCAddr, &serverCAddrLen);
         cout<<"The Main Server has received the statistic(s) of "+deptname+" from Backend Server C"<<endl;
		 cout <<" "<< buffer << endl;
		 resp=buffer;
	}
	else{
		resp="department Name "+deptname+" not found";
		cout<<"Department "<<deptname<<" not found"<<endl;
		}
				   		cout<<"The Main Server has sent the department statistics to the admin client "<<clientid<<"using TCP over port 45074"<<endl;

				   
				  
					   //string response1= "Thank you for contacting me the main server";
					  send(new_fd, resp.c_str(),resp.length(),0); 
				   }
				   //string response="Hey Client, I have received "+msg+" from you"; 
					   
					   
				   }
				    
					
    
			
			
			
			
			
			
			
			
			
				   
			}
            close(new_fd);  // close the socket to free the resoure.
            exit(0);
        }
        
    }

    return 0;

    
    
}



