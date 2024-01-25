/*
** listener.c -- a datagram sockets "server" demo
*/
#include<fstream>
#include<sstream>
#include<vector>
#include<algorithm>
#include<iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<cmath>
#include<cstring>
using namespace std;
#define MYPORT "43074"    // the port users will be connecting to

#define MAXBUFLEN 1000

// I have used Beej's Network Programming and ChatGPT as primary references. 
// Also for detailed comments please visit serverA.cpp , because it has exactly the same implementation as the below code.



// get sockaddr, IPv4 or IPv6:
void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET) {
        return &(((struct sockaddr_in*)sa)->sin_addr);
    }

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

struct StudentRecord {
    string department;
    string studentID;
    vector<int> courseScores;
};
double calculateGPA(const vector<int>& courseScores) {
    double sum = 0.0;
    int count = 0;

    for (int score : courseScores) {
        if (score != -1) {  // Skip "none"
            sum += score;
            count++;
        }
    }

    return (count > 0) ? (sum / count) : 0.0;
}
 
 double calculateDepartmentGpaMean(const vector<StudentRecord>& studentRecords, const string& department) {
    double totalGPA = 0.0;
    int count = 0;

    for (const auto& record : studentRecords) {
        if (record.department == department) {
            double gpa = calculateGPA(record.courseScores);
            totalGPA += gpa;
            count++;
        }
    }

    return (count > 0) ? (totalGPA / count) : 0.0;
}

double calculateDepartmentGpaVariance(const vector<StudentRecord>& studentRecords, const string& department) {
    double mean = calculateDepartmentGpaMean(studentRecords, department);
    double sumSquaredDifferences = 0.0;
    int count = 0;

    for (const auto& record : studentRecords) {
        if (record.department == department) {
            double gpa = calculateGPA(record.courseScores);
            sumSquaredDifferences += pow(gpa - mean, 2);
            count++;
        }
    }

    return (count > 0) ? (sumSquaredDifferences / count) : 0.0;
}

void calculateDepartmentMinMaxGPA(const vector<StudentRecord>& studentRecords, const string& department,
                                   double& departmentMinGPA, double& departmentMaxGPA) {
    vector<double> departmentGPAs;

    for (const auto& record : studentRecords) {
        if (record.department == department) {
            double gpa = calculateGPA(record.courseScores);
            departmentGPAs.push_back(gpa);
        }
    }

    if (!departmentGPAs.empty()) {
        // Sort the GPAs to find min and max
        sort(departmentGPAs.begin(), departmentGPAs.end());
        departmentMinGPA = departmentGPAs.front();
        departmentMaxGPA = departmentGPAs.back();
    } else {
        departmentMinGPA = 0.0;  // Default value if no students in the department
        departmentMaxGPA = 0.0;
    }
}

double calculatePercentageRank(const vector<StudentRecord>& studentRecords, double studentGPA, string dept) {
    int countHigherOrEqual = 0;
    int totalCount = 0;

    for (const auto& record : studentRecords) {
        if (record.department == dept) {  
		//cout<<"Department is "<<dept<<endl;
            double gpa = calculateGPA(record.courseScores);
			//cout<<"GPA is "<<gpa<<endl;
            totalCount++;

            if (gpa <= studentGPA) {
                countHigherOrEqual++;
            }
        }
    }
//cout<<"Total count: "<<totalCount<<endl;
//cout<<"counthigher"<<countHigherOrEqual<<endl;
    return (totalCount == 0) ? 0.0 : (static_cast<double>(countHigherOrEqual) / totalCount) * 100.0;
}


string generateDepartmentList(const vector<StudentRecord>& studentRecords) {
    stringstream departmentList;

    // Collect unique department names
    vector<string> uniqueDepartments;
    for (const auto& record : studentRecords) {
        if (find(uniqueDepartments.begin(), uniqueDepartments.end(), record.department) == uniqueDepartments.end()) {
            uniqueDepartments.push_back(record.department);
        }
    }

    // Create a comma-separated string of department names
    for (size_t i = 0; i < uniqueDepartments.size(); ++i) {
        departmentList << uniqueDepartments[i];
        if (i < uniqueDepartments.size() - 1) {
            departmentList << ",";
        }
    }

    return departmentList.str();
}
int main(void)
{
	/* Begin File Handling*/
	ifstream inputFile("dataC.csv"); 

    if (!inputFile.is_open()) {
        cerr << "Error opening file." << endl;
        return 1;
    }

    string line;
    vector<StudentRecord> studentRecords;

    // Read and process each line in the file
    while (getline(inputFile, line)) {
        istringstream ss(line);
        string token;

        // Split the line into tokens
        vector<string> tokens;
        while (getline(ss, token, ',')) {
            tokens.push_back(token);
        }

        // Skip the header line
        if (tokens[0] == "DPT") {
            continue;
        }

        // Process student record
        StudentRecord studentRecord;
        studentRecord.department = tokens[0];
        studentRecord.studentID = tokens[1];

        // Process course scores
        for (size_t i = 2; i < tokens.size(); ++i) {
            if (tokens[i] == "none") {
                studentRecord.courseScores.push_back(-1); // Use -1 to represent "none"
            } else {
                try {
                    // Convert the string to an integer
                    studentRecord.courseScores.push_back(stoi(tokens[i]));
                } catch (const invalid_argument& e) {
                    // Handle the case where conversion fails (e.g., "none")
                    //cerr << "Error converting to integer: " << e.what() << endl;
                    studentRecord.courseScores.push_back(-1); // Use -1 to represent "none"
                }
            }
        }

        // Add the record to the vector
        studentRecords.push_back(studentRecord);
    }

    // Display the processed data (for demonstration purposes)
    /*for (const auto& record : studentRecords) {
        cout << "Department: " << record.department << "\n"
                  << "Student ID: " << record.studentID << "\n"
                  << "Course Scores: ";
        for (int score : record.courseScores) {
            cout << (score == -1 ? "none" : to_string(score)) << " ";
        }
        cout << "\n\n";
    }*/

    inputFile.close();



    
	
	/* End of File Handling */
	/* Begin socket handling */
   int sockfd;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    int numbytes;
    struct sockaddr_storage their_addr;
    char buf[MAXBUFLEN];
    socklen_t addr_len;
    char s[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET; // set to AF_INET to use IPv4
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; // use my IP

    if ((rv = getaddrinfo(NULL, MYPORT, &hints, &servinfo)) != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(rv));
        return 1;
    }

    // loop through all the results and bind to the first we can
    for(p = servinfo; p != NULL; p = p->ai_next) {
        if ((sockfd = socket(p->ai_family, p->ai_socktype,
                p->ai_protocol)) == -1) {
            perror("listener: socket");
            continue;
        }

        if (bind(sockfd, p->ai_addr, p->ai_addrlen) == -1) {
            close(sockfd);
            perror("listener: bind");
            continue;
        }

        break;
    }

    if (p == NULL) {
        fprintf(stderr, "listener: failed to bind socket\n");
        return 2;
    }

    freeaddrinfo(servinfo);
	cout<<"Server C is up and running using UDP over port 43074"<<endl;
	addr_len = sizeof their_addr;
   recvfrom(sockfd, buf,sizeof(buf) , 0,(struct sockaddr *)&their_addr, &addr_len);
   if(strcmp(buf,"deptlist")==0){
     string  departmentList= generateDepartmentList(studentRecords);
			 cout<<"Server C has sent the department list to the Main Server using UDP over port 43074"<<endl;

	//cout<<buf<<endl;
	sendto(sockfd, departmentList.c_str(), departmentList.length(), 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
	
  
	}
	
	
	
	/* the chatting loop starts */
	while(1){
 char buffer[15000];
 bzero(buffer, sizeof(buffer));
	 recvfrom(sockfd, buffer,sizeof(buffer) , 0,(struct sockaddr *)&their_addr, &addr_len);
	 string recep=buffer;
	 int space_pos= recep.find(" ");
	 string deptname=recep.substr(0,space_pos);
	 string stuid=recep.substr(space_pos+1);
	
	 if(stuid!=""){
		 cout<<"Server C has received a student academic record query for Student ";
		 cout<<stuid;
		 cout<<" in Department ";
		 cout<<deptname<<endl;
		 auto it = find_if(studentRecords.begin(), studentRecords.end(),
                           [stuid, deptname](const StudentRecord& record) {
                               return record.studentID == stuid && record.department == deptname;
                           });
		if(it != studentRecords.end()){
			double studentGPA = calculateGPA(it->courseScores);
            double studentPercentageRank = calculatePercentageRank(studentRecords, studentGPA,deptname);
			string gpa=to_string(studentGPA);
			string rank=to_string(studentPercentageRank);
			string response="The Academic record for the student "+stuid+"in Department "+deptname+" is"+"\n"+"Student GPA: "+gpa+"\n"+"Percentage Rank: "+rank+"%";
		   cout<<response<<endl;
		   cout<<"Server C has sent the result(s) to Main Server"<<endl;
		   sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
			
			
		}
		else{
			string response="Student "+stuid+" not found in Department "+deptname;
			cout<<response<<endl;
			cout<<"Server C has sent the result(s) to Main Server"<<endl;
		 sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr *)&their_addr, sizeof(their_addr));
		}
		 
	 }
	 else{
		 cout<<"Server C has received a department academic statistics query for Department";
		 cout<<deptname<<endl;
		 double departmentGpaMean = calculateDepartmentGpaMean(studentRecords, deptname);
    double departmentGpaVariance = calculateDepartmentGpaVariance(studentRecords, deptname);
   double departmentMinGPA, departmentMaxGPA;
    calculateDepartmentMinMaxGPA(studentRecords, deptname, departmentMinGPA, departmentMaxGPA);
	string gpa=to_string(departmentGpaMean);
	string var=to_string(departmentGpaVariance);
	string min=to_string(departmentMinGPA);
	string max= to_string(departmentMaxGPA);
  string response="The academic statistics for department "+deptname+" are"+"\n"+"Department GPA Mean: "+gpa+"\n"+"Department GPA Variance: "+var+"\n"+" Department Max GPA: "+max+"\n"+" Department Min GPA: "+min; 
  cout<<response<<endl;
  cout<<"Server C has sent the result(s) to Main Server"<<endl;
  sendto(sockfd, response.c_str(), response.length(), 0, (struct sockaddr *)&their_addr, sizeof(their_addr));

		 
		 
	 }
	
	
    //close(sockfd);


}
close(sockfd);
}