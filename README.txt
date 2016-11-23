/* Read me */

a) Name: Rashmi Nalwad

b) Student Id: 2021179341

c) Summary: In this assignment I have implemented how AWS Mapreduce functionality works. Client makes a request to AWS server, AWS server in turn offloads the requests to the backend server's A,B and C which is the Map step. Backend server's computes the values and sends the results back to AWS server. AWS server inturn performs the reduction step on the values received and sends result back to the client.
	
d) Code files are:

	1) client.c: I have 1 client which interacts with AWS server by creating a TCP connection. It first sends the reduction type (sum,min,max,sos) in LowerCase to server.It then sends number of elements that it read from the file to server.It then sends each element, converts it into htonl format and writes line by line into server socket. Client also accepts the final reduced value from the AWS Server.
	2) aws.c : AWS server connects to client using TCP connection and connects to backend server's A,B and C using UDP connections. AWS first reads the reduction type and then reads total number of elements it has to read and then reads data sent by client line by line and stores it into an array. Then it equally divides the data into 3 equal parts and sends 1st set of data to Backend Server A, 2nd set to Backend server B and 3rd set to Backend Server C. It then recives the values from all the backend servers performs reduction operation and sends data back to client.
	3) serverA.c: Backend server's A receives the reduction type and numbers from AWS. Then apply the reduction type on the numbers and sends the computed value back to AWS.
	4) serverB.c:  Backend server's B receives the reduction type and numbers from AWS. Then apply the reduction type on the numbers and sends the computed value back to AWS.
	5) serverC.c : Backend server's C receives the reduction type and numbers from AWS. Then apply the reduction type on the numbers and sends the computed value back to AWS.

e) I have written a make file that directly compiles all the files and creates the executables. Just type "make" command, and all the source files are compiled and required executables are generated. The sequence of executables should be run in the 	order ServerA, ServerB, ServerC, aws and client.

f) Format of messages exchanged:
		i) Reduction type is sent from client to AWS, AWS to backend servers as : <Reduction Type>.
		ii) Client sends Total numbers read from file to server <Total Number>
		ii) All the numbers are sent from client to AWS in the form of <htonl(number)>.
		iii) The total number of elements sent from client to server is divided by 3, the quotient is the of number of elements to be sent to backend server A,B and C. This <Number> number is sent to backend server A,B and C from AWS before sending numbers.
		iv) The computed amount <Number> at backend Server are sent back from A,B and C to AWS.
		v) The reduced amount is sent back from AWS to client as <Number>.

g) Idiosyncracy:
	When all Server's are started and client sends request all 3 phases executes perfectly. But after that AWS will get a interrupt system call error, post which UDP messages sent from backend server's to AWS will get lost and AWS does not receive the messages sent from Backend end Server A,B and C.Basically Phase 3 does not work as expected. This issues exist only in nunki server. Kindly restart all the server's to execute next client request.

h) I have used code for sending and receiving data from "Beej networking guide". I have also referred code from stack	 	overflow when I faced issues. I have marked them appropriately in the source code.


