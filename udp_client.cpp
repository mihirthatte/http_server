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
    
    if (argc < 4)  {
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
    //inet_aton(server_address, &server_details.sin_addr.s_addr);
    server_details.sin_addr = *((struct in_addr *)server_address->h_addr);
    memset(&(server_details.sin_zero), '\0', 8);   

    string filenames[100];
    int num_of_files = 0;
    for(int i =3; i<argc;i++)
        filenames[num_of_files++] = argv[i];
    int network_socket;
    network_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (network_socket < 0){
        cout<<"Error opening socket"<<endl;
        return 1;
    }
    //start timer -
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_beg);
    for(int i =0; i<num_of_files; i++){
        cout<<"i - "<<i<<endl;
        char request[2048];
        memset(request, 0, 2048);
        strcpy(request, "GET /");
        strcat(request, filenames[i].c_str());
        strcat(request, " HTTP/1.1\r\n\r\n");
        cout<<"Request from client -- "<<endl;
        cout<<request<<endl;
        cout<<"******************"<<endl;
        
        socklen_t client_len;
        client_len = sizeof(struct sockaddr_in); 
        int num_bytes;
        num_bytes = sendto(network_socket, request, sizeof(request),0, (struct sockaddr *) &server_details, client_len);
        if(num_bytes < 0){
            cout<<"Send to error"<<endl;
            close(network_socket);
            return 1;
        }
        //Waiting for response from server - 
        char *response;
        response = new char[10000000];
        memset(response, 0, sizeof(response));
        while(1){
            client_len = sizeof(struct sockaddr_in);
            num_bytes = recvfrom(network_socket, response,  sizeof(response),  0, (struct sockaddr *) &server_details, &client_len);
            if(num_bytes<0){
                cout<<"receive error"<<endl;
                return 1;
            }
             if (strcmp(response, "EOF") == 0 || strcmp(response, "HTTP/1.0 404 Not Found\r\n\r\n") == 0){
                    cout<<response<<endl;       
                    break;
            }
            cout<<response;
            memset(response, 0, sizeof(response));
            
        }
        cout<<"File received from the server - "<<endl;
         
    }
    clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts_end);
	cout<<"Time taken to transfer "<<num_of_files<<" of files - "<<endl;
	cout << (ts_end.tv_sec - ts_beg.tv_sec) + (ts_end.tv_nsec - ts_beg.tv_nsec)<<endl;
	close(network_socket);
    return 0;

}
