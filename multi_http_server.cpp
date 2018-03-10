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
#include <pthread.h>
using namespace std;

struct thread_argc{
	int socketfd;
	struct sockaddr_in *client_details;
};

int connection_type = 0;

char* parse_get_request(char *str){
  
  char *line_sep, *line_sep2;
  line_sep = strtok (str,"\n");
  line_sep2 = strtok (NULL, "\n");
   if(strcmp(line_sep2,"Connection: Keep-Alive\r")==0){
    connection_type = 1;
  }
  char *html;
  char *html1;
  html = strtok(line_sep," ");
  bool flag = false;
  while(html != NULL){
        char b1[]="HTTP";
        char b2[]="GET";
        if(!strstr(html,b1) && !strstr(html,b2)){
          flag = true;
          html1 = html;
        }
      html = strtok(NULL," ");
      }
  if(flag)
    return html1;
  return NULL;
}

void* handle_client(void *client_argc){
	//struct sockaddr_in client_details, int client_socket
	struct thread_argc *args = (struct thread_argc *)client_argc;
	int client_socket = args->socketfd;
	struct sockaddr_in *client_details = args->client_details;
	cout << "Connection from client " << inet_ntoa(client_details->sin_addr)<< " port - "<<ntohs(client_details->sin_port)<<endl;
        char client_buffer[2048];
        memset(client_buffer,0,2049);
        read(client_socket, client_buffer,2047);
        
        char *file_name;
        cout<<"***********************"<<endl;
        cout<<client_buffer<<endl;
        cout<<"**********************"<<endl;
        file_name = parse_get_request(client_buffer); 
        cout<<"file name --"<<file_name<<endl;


        //Reading file -
        if (file_name != NULL){
            ifstream file(file_name+1);//"To eliminate first '/' character
            if (file.fail()){
                char not_found[2048] = "HTTP/1.0 404 Not Found\r\n<html><body><h1>File not found </h1></body></html>\r\n";
                send(client_socket, not_found, sizeof(not_found),0);
                close(client_socket);
            }
            else{
				
				file.seekg(0, std::ios::end);
                int length = file.tellg();
                cout<<"File size -- "<<length<<endl;
                file.seekg(0, std::ios::beg);
                char buffer[length];
                memset(buffer, 0, length);
                file.read(buffer, length);
                
                char response[2048+length];
                memset(response, 0, sizeof(response));
                strcpy(response,"HTTP/1.0 200 OK\r\nContent-Type: text/html\r\n\r\n");
                strcat(response,buffer);
                
                int numSent;
                numSent = send(client_socket, response, sizeof(response),0);
                cout<<"Data sent ---" <<numSent<<endl;
                if (connection_type == 0)//Non-persistent connection
                    close(client_socket);
                if (connection_type == 1) //Persistent connection
                { 
					cout<<"persistent conn "<<endl;
					//Don't close the socket;
				}			
                cout<<"File sent to the client"<<endl;

            }
            file.close();
        }
	
}

int main(int argc, char *argv[]){
    if(argc < 2){
        cout<<"Enter PORT number"<<endl;
        return 1;
    }
    struct sockaddr_in server_details;
    struct sockaddr_in client_details;

    //clear struct - 
    memset(&server_details, 0, sizeof(server_details));
    
    server_details.sin_family = AF_INET;
    server_details.sin_port = htons(atoi(argv[1]));
    server_details.sin_addr.s_addr = INADDR_ANY; //Local IP address
    
    int server_socket;
    server_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
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
    if (listen(server_socket, 5) == -1){
        cout<<"listen failed "<<endl;
            return 1;
    }
    cout << "Server is running and listening for HTTP request on port "<<atoi(argv[1]) <<endl;
    cout<< "Press ctrl + c to quit the server"<< endl;
	pthread_t client_threads[100];
	int thread_pointer = 0;
    int delegate_socket;
    while(thread_pointer < 100){
        socklen_t client_len;
        client_len = sizeof(client_details);
        
        delegate_socket = accept(server_socket, (struct sockaddr *) &client_details, &client_len);
        if (delegate_socket < 0){
            cout<<"accept failed "<<endl;
            close(server_socket); 
            return 1;
        }
		
		struct thread_argc *client_argc = (struct thread_argc *)malloc(sizeof(thread_argc));
		client_argc->socketfd = delegate_socket;
		client_argc->client_details = &client_details;
		
		pthread_create(&client_threads[thread_pointer++],NULL, handle_client, client_argc);
		//handle_client(client_details, delegate_socket);
        //close(delegate_socket);
    
    }
    close(server_socket);
    return 0;
}
