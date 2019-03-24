#include <stdio.h>
#include <stdlib.h> //Eg. int atoi(const char *str) Conver the string pointed to, by the argument str to an integer (type int).
#include <string.h>
#include <unistd.h>
#include <sys/types.h>  //For next two .h file
#include <sys/socket.h> // include a number of definitions of structures needed for sockects. Eg. defines the sockaddr structure
#include <netinet/in.h> // contains constants and structures needed for internet domain address. Eg. sockaddr_in
#include <netdb.h>
#include <arpa/inet.h>
#include <time.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

string read_file(char * argv){
  string filename = argv;
  ifstream f(filename);
  stringstream buffer;
  buffer << f.rdbuf();
  string s = buffer.str();

  return s;
}

void client_connect(string &xml_input){
  int len = xml_input.length();
  string xml_length = to_string(len);
  string xml_context = xml_length + "\n" + xml_input;
  std::cout << xml_context << std::endl; 

  int status;
  int socket_fd;
  struct addrinfo host_info;
  struct addrinfo *host_info_list;
  const char *hostname = "vcm-8131.vm.duke.edu";
  const char* port_num = "12346";

  memset(&host_info, 0, sizeof(host_info));
  host_info.ai_family   = AF_UNSPEC;
  host_info.ai_socktype = SOCK_STREAM;

  status = getaddrinfo(hostname, port_num, &host_info, &host_info_list);
  if (status != 0) {
    printf("Error: cannot get address info for host\n");
    exit(EXIT_FAILURE);
  }

  socket_fd = socket(host_info_list->ai_family,
		     host_info_list->ai_socktype,
		     host_info_list->ai_protocol);
  if (socket_fd == -1) {
    perror("create socket failed:");
    exit(EXIT_FAILURE);
  }

  status = connect(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
  if (status == -1) {
    perror("connect failed:");
    exit(EXIT_FAILURE);
  }
  std::cout<<"connect to server successfully"<<std::endl;
  


  //const char * test = "test";
  if(send(socket_fd, xml_context.c_str(), xml_context.length(), 0)==-1){
    std::cout<<"error send"<<std::endl;
  }
  
}

int main(int argc, char *argv[]){
  string xml_input = read_file(argv[1]);
  client_connect(xml_input);
  while(1){}
  return EXIT_SUCCESS;
}
