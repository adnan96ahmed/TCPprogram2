CIS 3210 A2
 Authors: 
	Adnan Ahmed - 0883865

 Due: Tuesday October 31

 Usage Instructions
 ----------------------------------
 1. 'make' to compile all c programs

 2. Run server using './server <port-num>' where <port-num> is the port number the server listens on.
 	Example: ./server 12051

 3. Run client using './client <server-IP-address>:<port-num> <file> <optional-buffer>'
 	where
 		<server-IP-address>: is the IP address or URL name of the server
 		<port-num> is the port number the server listens on
 		<file> is the name of the file that the client reads and transmits
 		<optional-buffer> is the length of the buffer that is read from the file, Default is 4096 bytes
 	Example: ./client george.socs.uoguelph.ca:12051 file.txt 25

4. Can spawn multiple simultaneous clients using 
	‘./pythonScript.py <num-clients> <server-IP-address>:<port-num> <file> <optional-buffer>'
	where
 		<server-IP-address>: is the IP address or URL name of the server
 		<port-num> is the port number the server listens on
 		<num-clients> is the number of clients to spawn
 		<file> is the name of the file that the client reads and transmits
 		<optional-buffer> is the length of the buffer that is read from the file, Default is 4096 bytes
 	Example: ./pythonScript.py 20 george.socs.uoguelph.ca:12051 file.txt 


Limitations:
—————————————-
- The highest and also the default buffer length size is 4096 bytes, this is because the server file has a fixed size for how many bytes it will receive. 

References:
The main coding files, TCPserver.c and TCPclient.c - 
CIS*3210 Moodle:
https://moodle.socs.uoguelph.ca/

Code for getaddrinfo() - 
http://beej.us/guide/bgnet/output/html/multipage/getaddrinfoman.html

Code for fread() - 
http://www.cplusplus.com/reference/cstdio/fread/

