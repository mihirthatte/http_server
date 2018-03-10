#define _BSD_SOURCE
#include<stdio.h>
#include<iostream>
#include <sstream>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<fstream>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include<time.h>

using namespace std;
int main(int argc, char *argv[]){
    timespec ts_beg, ts_end;
    if (argc < 5)  {
        cout<<"usage: Missing client parameters\n";
        exit(1);
    }
    
    struct hostent *server_address;
    server_address = gethostbyname(argv[1]);
    if (server_address == NULL) {
        cout<<"ERROR, no such host\n";
        exit(0);
    }
     

    int port = atoi(argv[2]);
    struct sockaddr_in server_details;
    struct sockaddr_in client_details;

    //clear struct - 
    memset(&server_details, 0, sizeof(server_details));
    
    server_details.sin_family = AF_INET;
    server_details.sin_port = htons(port);
    server_details.sin_addr = *((struct in_addr *)server_address->h_addr);
    memset(&(server_details.sin_zero), '\0', 8);   

	//Set connection type - P for persistent and NP for non
	int connection_type;
	if(strcmp(argv[3], "P") == 0)
		connection_type = 1;
	else
		connection_type = 0;
		
    string filenames[100];
    int num_of_files = 0;
    //start timer -
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_beg);
    for(int i =4; i<argc;i++)
        filenames[num_of_files++] = argv[i];
               
		for(int i =0; i<num_of_files; i++){
			int network_socket;
			network_socket = socket(AF_INET, SOCK_STREAM, 0);
			if (network_socket < 0){
				cout<<"Error opening socket"<<endl;
				return 1;
			}
			if( connect(network_socket, (struct sockaddr*) &server_details,
				sizeof(server_details)) == -1){
				cout<< "Connect client failed"<<endl;
				close(network_socket);
				return 1;
			}
			char request[2048];
			memset(request, 0, 2048);
			strcpy(request, "GET /");
			strcat(request, filenames[i].c_str());
			strcat(request, " HTTP/1.1\r\n");
			if (connection_type ==1) //For persistent connection
				strcat(request, "Connection: Keep-Alive\r\n");
			else
			strcat(request, "\r\n");
			cout<<"*********************************************************************************************"<<endl<<endl;
			cout<<"Request from client -- "<<endl;
			cout<<request<<endl;
			cout<<"*********************************************************************************************"<<endl<<endl;
			
			send(network_socket, request, sizeof(request),0);

			char response[8132];			
			recv(network_socket, &response, sizeof(response), 0);
			cout<<"Response from server"<<endl;
			cout<<"##############################################################################################"<<endl<<endl;
			cout<<response<<endl<<endl;
			cout<<"##############################################################################################"<<endl<<endl;
			close(network_socket);
		}
		//end timer -
		clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
		cout<<"Time taken to transfer "<<num_of_files<<" of files - "<<endl;
		cout << (ts_end.tv_sec - ts_beg.tv_sec) + (ts_end.tv_nsec - ts_beg.tv_nsec);
    return 0;

}
