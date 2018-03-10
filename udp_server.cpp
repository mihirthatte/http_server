#define _BSD_SOURCE
#include<stdio.h>
#include<iostream>
#include <sstream>
#include<string.h>
#include<cstring>
#include<stdlib.h>
#include<unistd.h>
#include<fstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#define PORT 17518

using namespace std;
char* parse_get_request(char *str){

  char *line_sep;
  line_sep = strtok (str,"\n");
  char b[] = "GET";
  char *output = NULL;
  output = strstr (line_sep,b);
  char *html;
  html = strtok(line_sep," ");
  while(html != NULL){
        char b1[]="HTTP";
        char b2[]="GET";
        if(!strstr(html,b1) && !strstr(html,b2)){
          return html;
        }
      html = strtok(NULL," ");
      }
  return NULL;
}

void handle_client(int server_socket, struct sockaddr_in client_details, char *request){
	    cout << "UDP Connection from client " << inet_ntoa(client_details.sin_addr)<< " port - "<<ntohs(client_details.sin_port)<<endl;
        char *file_name;
        cout<<"***********************"<<endl;
        cout<<request<<endl;
        cout<<"**********************"<<endl;
        file_name = parse_get_request(request); 
        cout<<"file name --"<<file_name<<endl;
        //Reading file -
        if (file_name != NULL){
           FILE *fp = fopen(file_name+1, "r");
            if (fp == NULL){
                cout<<"file not found on server"<<endl;
                char not_found[1024];
                memset(not_found, 0, 1024);
                strcpy(not_found, "HTTP/1.0 404 Not Found\r\n\r\n");
                cout<<"Server response -- "<<not_found<<endl;
                int num_bytes;
                num_bytes = sendto(server_socket, not_found, sizeof(not_found),0, (struct sockaddr *) &client_details, sizeof(struct sockaddr_in));
                cout<<"Bytes sent - "<<num_bytes<<endl;
                 if(num_bytes < 0){
                    cout<<"Send to error"<<endl;
                    return ;
                    }
            }
            else{

                char *response;
                response = new char[10000000];
                int file_bytes_read;
                int num_bytes;
                memset(response, 0, sizeof(response));
                while(1){
                    file_bytes_read = fread(response, 1, sizeof(response),fp);
                    if (file_bytes_read == 0)
                        break;
                    num_bytes = sendto(server_socket, response, sizeof(response), 0, (struct sockaddr *) &client_details, sizeof(struct sockaddr_in));
                    if(num_bytes == -1){
                        cout<<"File send to error"<<endl;
                        return;
                    } 
                    cout<<"Number of bytes sent - "<<num_bytes<<endl;
                    memset(response, 0, sizeof(response));
                }
                char eof[20];
                strcpy(eof, "EOF");//End of file indicator
                num_bytes = sendto(server_socket, eof, sizeof(eof), 0, (struct sockaddr *) &client_details, sizeof(struct sockaddr_in));
                
                cout<<"File sent to the client"<<endl;
            }
            
        }
	
    }


int main(int argc, char *argv[]){
    if(argc < 2){
        cout<<"Enter PORT number"<<endl;
        return 1;
    }
    struct sockaddr_in server_details;
    struct sockaddr_in client_details;
    socklen_t fromlen;
    //clear struct - 
    memset(&server_details, 0, sizeof(server_details));
    
    server_details.sin_family = AF_INET;
    server_details.sin_port = htons(atoi(argv[1]));
    server_details.sin_addr.s_addr = INADDR_ANY; //Local IP address
    
    int server_socket;
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);//UDP connection
    if (server_socket < 0){
        cout<<"Error opening socket"<<endl;
        return 1;
    }
        
    //Bind server - 
    if( bind(server_socket, (struct sockaddr*) &server_details, 
        sizeof(server_details)) == -1){
        cout<< "bind failed. Try again using different PORT number"<<endl;
        close(server_socket);
        return 1;
    }

    cout << "UDP Server is running and listening for HTTP request on port "<<atoi(argv[1]) <<endl;
    cout<< "Press ctrl + c to quit the server"<< endl;
    int k =1;
    while(1){
        fromlen = sizeof client_details;
        char request[2048];
        memset(request,0,2048);
        int num_bytes;
        num_bytes = recvfrom(server_socket,request,  2048,  0, (struct sockaddr *)&client_details, &fromlen);
        cout<<"bytes receved from client - "<<num_bytes<<endl;
        if(num_bytes > 0){
            k++;
		    handle_client(server_socket, client_details, request);
        }
    }
    close(server_socket);
    return 0;
}
