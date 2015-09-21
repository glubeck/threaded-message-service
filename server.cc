#include <iostream>
#include "server.h"

using namespace std;

Server::Server() {
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Server::~Server() {
    delete buf_;
}

void
Server::run() {
    // create and run the server
    create();
    serve();
}

void
Server::create() {
}

void
Server::close_socket() {
}

void
Server::serve() {
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);

      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {

        handle(client);
    }
    close_socket();
}

void
Server::handle(int client) {
    // loop to handle all requests
  
 

   string cache = "";

  while (1) {
    bool valid = false;
    string request = get_request(client);
    bool success;
    if (request.empty())
      break;
    
    cache = request;
    bool needed = false;
    
    //split the message from the cache
    vector<string> halves = half(request);
   
    //split the message into parts
    vector<string> elements = split(halves[0], ' ');
    
    if(elements[0] == "put" && elements.size() == 4) {
      valid = true;
      store(cache, client);
      success = send_response(client, "OK\n");
    }

    if(elements[0] == "list" && elements.size() == 2) {
      valid = true;
      string list = getList(elements[1]);
      success = send_response(client, list);
    }

    if(elements[0] == "get" && elements.size() == 3) {
      valid = true;
      string message = retrieveMessage(elements[1], atoi(elements[2].c_str()));

      if(message == "name not found")
	success = send_response(client, "error name not found\n");

      if(message == "invalid index")
	success = send_response(client, "error index invalid\n");
      
      success = send_response(client, message);
    }

    if(elements[0] == "reset" && elements.size() == 1) {
      valid = true;
      messageMap.clear();
      success = send_response(client, "OK\n");
    }
    
    if(!valid) {
      success = send_response(client, "error invalid input\n");
    }
    
  }
  close(client);
}

string Server::get_value(int client, int messageLength, string cache) {

  int left = messageLength - cache.length();  
  string cache2 = cache;

  while(left > 0) {

    int nread = recv(client,buf_,1024,0);
    
    left -= nread;

    if (nread < 0) {
      if (errno == EINTR)
	// the socket call was interrupted -- try again
	continue;
      else
	// an error occurred, so break out
	break;
    } else if (nread == 0) {
      // the socket is closed
      break;
    }

    cache2.append(buf_,nread);
  }
  return cache2;
}

void Server::store(string cache, int client) {

  //parse the store request
  Message *message = parse_request(cache);
  
  //if more bytes are needed, get more bytes from the buffer
  if(message->needed) {

    message->value = get_value(client, message->length, message->value);

  }
  
  //store the message
  storeMessage(message);
   
}

string
Server::get_request(int client) {
    string request = "";
    // read until we get a newline
    while (request.find("\n") == string::npos) {
        int nread = recv(client,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        request.append(buf_,nread);
    }
    // a better server would cut off anything after the newline and
    // save it in a cache
    return request;
}

bool
Server::send_response(int client, string response) {
    // prepare to send response
    const char* ptr = response.c_str();
    int nleft = response.length();
    int nwritten;
    // loop to be sure it is all sent

    if(response == "invalid input\n") {
      perror("invalid input");
      return false;
    }
    
    while (nleft) {
        if ((nwritten = send(client, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}



Message* Server::parse_request(string request) {

  bool needed;
  int byteNum;
  Message *dummy = new Message();
  string text = "";
  if(containsNewline(request)) {

    //split the message from the cache
    vector<string> halves = half(request);
   
    //split the message into three parts
    vector<string> elements = split(halves[0], ' ');
    
    //if syntax is valid
    if(elements[0] == "put" && elements.size() == 4) {
      
      //if fourth argument is number
      if(isNumber(elements[3])) {
	
	//convert to int
	byteNum = atoi(elements[3].c_str());
	
	//determine whether or not more bytes are needed
	if(byteNum > request.length()) {
	  needed = true;
	}
	if(byteNum <= request.length()) {
	  needed = false;
	}
	
	//store in message variable
	Message *message = new Message(halves[0], elements[1], elements[2], byteNum,
	  			       halves[1], needed);
	
	return message;

      }
      //if anything goes wrong, return a dummy message with bogus input
      return dummy;
    }
    return dummy;
  }
  return dummy;
}


vector<string> Server::split(string s, char delim) {
  stringstream ss(s);
  string item;
  vector<string> elems;
  while(getline(ss, item, delim)) {
    elems.push_back(item);
  }
  return elems;
}

vector<string> Server::half(string str) {
  string first = "";
  string second = str;
  vector<string> halves;
  for (size_t i = 0; i < str.length(); i++) {
    if(str[i] != '\n') {
      first += str[i];
      second.erase(0,1);
    }
    if(str[i] == '\n') {
      halves.push_back(first);
      second.erase(0,1);
      halves.push_back(second);
      break;
    }
  }
  return halves;
}



vector<string> Server::splitCache(string cache, int byteNum) {


  string first = "";
  string second = cache;
  vector<string> halves;
  
  for(size_t i = 0; i < byteNum; i++) {

    first+=cache[i];
    second.erase(0,1);
  }
  halves.push_back(first);
  halves.push_back(second);
  return halves;
}

bool Server::isNumber(string str) {

  for (size_t i = 0; i < str.length(); i++) 
    if (!isdigit(str[i])) 
	return false;
	
  return true;
}

bool Server::containsNewline(string str) {

  for (size_t i = 0; i < str.length(); i++) {
    if (str[i] == '\n') {
      return true;
    }
  }
  return false;
}

string Server::getList(string name) {

  vector<Message*> messages = messageMap[name];

  stringstream size;
  size << messages.size();
  
  string response = "list " + size.str() + "\n";

  for(int i = 0; i < messages.size(); i++) {

    stringstream index;
    index << i+1;
    
    response += index.str() + " " + messages[i]->fileName + "\n";

  }

  return response;
  
}

string Server::retrieveMessage(string name, int index) {

  if(messageMap.find(name) == messageMap.end())
    return "name not found";
  
  string response = "message ";
  vector<Message*> messages = messageMap[name];

  if(index > messages.size())
    return "invalid index";
  
  Message* message = messages[index-1];

  stringstream messageLength;
  messageLength << message->length;
  
  response += message->fileName + " " + messageLength.str() + "\n" + message->value;

  return response;

}
