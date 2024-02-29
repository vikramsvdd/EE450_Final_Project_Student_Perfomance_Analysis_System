1. Vikram Vasudevan

2. 6201-1130-74

3. Platform: Ubuntu 20.04.6

4. In this project, I have implemented a simple network infrastructure, where a student/admin client queries the main server for an academic/departmental information, and the main server satisfies their query by obtaining the info from a collection of backend servers A, B and C.
 
5.student.cpp: Code that acts as the student client

admin.cpp: code corresponding to the admin client.

servermain.cpp : It is the code that drives the main server and implements the main server functionality.

serverA.cpp: It is the code that drives the serverA where the server extracts data from dataA.csv and then provides relevant information to the main server upon its request.

serverB.cpp: It is the code that drives the serverB where the server extracts data from dataB.csv and then provides relevant information to the main server upon its request.

serverC.cpp:It is the code that drives the serverC where the server extracts data from dataC.csv and then provides relevant information to the main server upon its request.

Makefile: a file that compiles the  server/client files into their respective object files and converts them further into an executable file for each cpp file.

6. the message exchange format is as follows: (Here Cs stands for student client, Ca stands for admin client, Sa stands for serverA, and M for main server) The same is the process for serverB and C too!
M:(gets the list from all the servers)
Cs: (the same works if Ca asks for a student info too) Asks the Main server for the academic info corresponding to a student
M: (if a valid department!), proceeds to ask the relevant backend server for the requested info ( A in this case)
M: sends the request to server A
Sa: received request from main server corresponding to <dept name>
Sa: Sent the relevant info (if a valid student, else prompts the main server about the invalidity)
M: forwards it to the student
Cs: displays the info

(the same procedure is followed if the admin asks for a departmental statisics information too!)



7. I have designed my system to handle atmost 1000 characters. Also, I have designed the system to be case-sensitive wrt the handling of the department name (say if in the dataA.txt, there is a department ECE, but if the client inputs ece or eCE, it will fail to recognise that!). Also mainly, the main server gets the department list only once i.e after the startup of the backend servers and its own startup, thus to restart the system you must start the backend servers first and then the main server.

8. References: Beej's Network Programming 
