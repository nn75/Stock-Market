//
//  Server.cpp
//
//
//  Created by Yijie Yan on 3/23/19.
//

#include <algorithm>
#include <cstring>
#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <mutex>
#include <netdb.h>
#include <sstream>
#include <stdio.h>
#include <string.h>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <thread>
#include <pthread.h>
#include <exception>
#include <stdexcept>
#include <pqxx/pqxx> 
#include <libxml/parser.h>
#include <libxml/tree.h>



#include "db_operator.h"

#define BUFFER_SIZE 409600
#define THREAD_NUM 1


using namespace pqxx;
using namespace std;
DBOperator db;
class Server{

public:
  int server_fd;


  struct addrinfo * connect_socket();
  void close_connect(int webserver_fd,struct addrinfo*host_info_list);
  void receiveRequest(int client_fd, std::vector <char>& buffer);



};




struct addrinfo * Server::connect_socket(){
    //BEGIN_REF - TCP Example by by Brian Rogers, updated by Rabih Younes in ECE650
    int status;
    const char* hostname=NULL;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *port     = "12320";
    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family   = AF_INET;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags    = AI_PASSIVE;
    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
      std::cerr << "Error: cannot get address info for host" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      exit(EXIT_FAILURE);
    }
    server_fd = socket(host_info_list->ai_family,
		      host_info_list->ai_socktype,
		      host_info_list->ai_protocol);
    if (server_fd == -1) {
      std::cerr << "Error: cannot create socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      exit(EXIT_FAILURE);
    }
    int yes = 1;
    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status = bind(server_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
      std::cerr << "Error: cannot bind socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      exit(EXIT_FAILURE);
    }
    status = listen(server_fd, 100);
    if (status == -1) {
      std::cerr << "Error: cannot listen on socket" << std::endl;
      std::cerr << "  (" << hostname << "," << port << ")" << std::endl;
      exit(EXIT_FAILURE);
    }
    //END_REF
    return host_info_list;
  }

//close socket connection
void Server::close_connect(int webserver_fd,struct addrinfo*host_info_list){
  freeaddrinfo(host_info_list);
  close(webserver_fd);
}




int recv_end(std::vector <char>& buffer, size_t recv_size){
  try{
    std::string len;
    std::string xml(buffer.data());
    std::stringstream ss(xml);
    ss>>len;
    if (recv_size == std::stoi(len) + len.length() + 1){
      //receive end
      return 1;
    }
  }catch(std::exception& e){
    std::cerr << e.what() << std::endl;
    return -1;
  }

  return 0;
}


void paserRoot(std::string raw_xml){
  xmlDocPtr pdoc = NULL;
  xmlNodePtr proot = NULL;
  xmlKeepBlanksDefault(0);
  string xmlString = raw_xml;
  pdoc = xmlParseMemory((const char *)xmlString.c_str(), xmlString.size());
  if (pdoc == NULL)
    {
      perror("error:can't open file!\n");
      return;
    }
  //get the root element
  proot = xmlDocGetRootElement(pdoc);
  if (proot == NULL){
    perror("error: file is empty!\n");
    return;
  }

  if(!xmlStrcmp(proot->name, BAD_CAST("create"))){
    xmlNodePtr pcur = NULL;
    //call create handler
    pcur = proot->xmlChildrenNode;
    while (pcur != NULL){
        if (!xmlStrcmp(pcur->name, BAD_CAST("account"))){
          map<string, string> account_map;
          xmlAttr * attr = pcur->properties;
          while(attr){				
            //printf("\t%s=%s\n",attr->name,attr->children->content);
            string key = (char *) attr->name;
            string value = (char *) attr->children->content;
            account_map.insert(pair<string, string>(key, value));  
            attr = attr->next;
          }
          if(account_map.size()!=2){
            error_handler("Wrong attribute number");//////////////////////////////////////////////////////
          }
          map<string, string>::iterator it;
          it = account_map.find("id");
          string id = "";
          string balance = "";
          if(it != account_map.end()){
            id = it -> second;
          }
          else{
            error_handler("No id");//////////////////////////////////////////////////////////////////////
          }
          it = account_map.find("balance");
          if(it != account_map.end()){
            balance = it -> second;
          }
          else{
            error_handler("No balance");/////////////////////////////////////////////////////////////////
          }
          //handle_create(id, balance);
        }
        pcur = pcur->next;
    }
  }
  if(!xmlStrcmp(proot->name, BAD_CAST("transactions"))){
    cout<< "this is transactions"<<endl;

    //call transaction handler

  }
  //release resource
  xmlFreeDoc(pdoc);
  xmlCleanupParser();
  xmlMemoryDump();
  return;
}

string error_handler(string type, int id, string sym){
  std::string result;
  std::string account1 = "<error id=' " +std::to_string(id)+"' >";
  std::string sym1 = "<error sym='" + sym + "' " + "id=' " +std::to_string(id)+"' >";
  std::string end = "</error>";
  if(type.compare("account")==0){
    result=account1+"account create error"+end; 
    return result;
  }
  if(type.compare("sym_create")==0){
    result = sym1+"symbol create error"+end;
    return result;
  }
  if(type.compare("sym_attribute")==0){
    result = sym1+"symbol attribute error"+end;
    return result;
  }
  return result;
}


//receive request

void Server::receiveRequest(int client_fd, std::vector <char>& buffer){
  size_t receive_length=0;
  std::cout<<"buffer size is "<<buffer.size()<<std::endl;
  try{
    while(1){
      size_t recvbyte = recv(client_fd,  &buffer.data()[receive_length],BUFFER_SIZE,0);
      if((int)recvbyte==-1){

	perror("first receive error");
      }
      receive_length=receive_length+recvbyte;
      if((int)recvbyte==0){
	break;
      }
      if(recv_end(buffer, receive_length)==1){
	//receive end;
	break;
      }else if (recv_end(buffer, receive_length)== 0){
	//need to resize
	if(receive_length == buffer.size()){
	  buffer.resize(receive_length+BUFFER_SIZE, 0);
	}
      }

    }

  }
  catch(std::bad_alloc& exception){
    std::cerr<<"bad_alloc exception"<<std::endl;
  }
  std::cout<<"Received: "<<std::endl;
  std::cout<<buffer.data()<<std::endl;

  std::string raw_xml(buffer.begin(), buffer.end());

  int pos = raw_xml.find("<");
  std::string xml = raw_xml.substr(pos);
  cout<< "paser xml is " <<endl;
  cout<< xml <<endl;
  paserRoot(xml);

}


void helper(int client_fd, Server server, std::vector <char>& buffer){

  try{
    server.receiveRequest(client_fd, buffer);
  }
  catch(std::exception &e){
    return;
  }
}




void init_db(){
std::string sql;

try{
connection C("dbname=hw4database user=userhw4 password=000000");
if (C.is_open()) {
cout << "Opened database successfully: " << C.dbname() << endl;
} else {
cout << "Can't open database" << endl;
return;
}
/* Create SQL statement */
sql = "CREATE TABLE account(account_id serial PRIMARY KEY, balance INT, SYM1 CHAR(50) );";

 std::string drop_table = "DROP TABLE IF EXISTS account;";

work W(C);


//W.exec(drop_table);
 W.exec( drop_table );
W.exec( sql );
W.commit();
cout << "Table created successfully" << endl;
C.disconnect ();
} catch (const std::exception &e){
cerr << e.what() << std::endl;
return;
}

return;

}








int main(){

  DBOperator db;
  db.init_db();
  Server server;
  struct addrinfo * addrinfo = server.connect_socket();
  size_t server_fd = server.server_fd;
  //  int thread_id = 0;


  while(1){
    try{
      struct sockaddr_in client_addr;
      socklen_t addr_len = sizeof(client_addr);
      int client_fd = accept(server_fd,(struct sockaddr*)&client_addr, &addr_len);

      if (client_fd == -1) {
	perror("Cannot accept connection");
	continue;
      }
      std::vector <char> buffer(1000);
      std::thread th(helper,client_fd, server, std::ref(buffer));
      th.detach();
            //server.receiveRequest(client_fd, buffer);

    }catch(std::exception& e){
      std::cerr << "In main error: " << e.what() << std::endl;
    }
  }

  server.close_connect(server_fd, addrinfo);
  return EXIT_SUCCESS;

}
