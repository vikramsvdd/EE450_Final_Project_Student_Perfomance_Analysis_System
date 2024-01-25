# -*- Makefile -*-

#target: dependencies
	#action

all: student admin servermain serverA serverB serverC



student: student.cpp
	g++ -o student student.cpp 
	
	
admin: admin.cpp
	g++ -o admin admin.cpp 


servermain: servermain.cpp
	g++ -o servermain servermain.cpp 


serverA: serverA.cpp
	g++ -o serverA serverA.cpp 
	
serverB: serverB.cpp
	g++ -o serverB serverB.cpp 
	
serverC: serverC.cpp
	g++ -o serverC serverC.cpp 	

