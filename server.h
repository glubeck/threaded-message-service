#pragma once

#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <sstream>
#include <map>
#include <string>

using namespace std;

class Message {

 public:
  Message(string& firstLine, string& name, string& fileName,
	  int& length, string& value, bool needed) {
    
    this->firstLine = firstLine;
    this->name = name;
    this->fileName = fileName;
    this->length = length;
    this->value = value;
    this->needed = needed;
  }
   Message() {

     this->firstLine = "";
     this->name = "";
     this->fileName = "";
     this->length = 0;
     this->value = "";
     this->needed = false;
   }
   //~Message();

   string firstLine;
   string name; 
   string fileName; 
   int length;
   string value;
   bool needed;
   

   string getFileName() {
     return fileName;
   }
   void setFileName(string fileName) {
     this->fileName = fileName;
   }

   int getLength() {
     return length;
   }
   void setLength(int length) {
     this->length = length;
   }

   string getValue() {
     return value;
   }
   void setValue(string value) {
     this->value = value;
   }
   
   bool getNeeded() {
     return needed;
   }
   void setNeeded(bool needed) {
     this->needed = needed;
   }
  
 };

class Server {
public:
    Server();
    ~Server();

    void run();
    
protected:
    virtual void create();
    virtual void close_socket();
    void serve();
    void handle(int);
    string get_request(int);
    bool send_response(int, string);
    vector<string> split(string, char);
    bool isNumber(string);
    bool containsNewline(string);
    vector<string> half(string);
    vector<string> splitCache(string, int);
    Message* parse_request(string);
    string get_value(int, int, string);
    void store(string, int);
    string getList(string);
    string retrieveMessage(string, int);
    int server_;
    int buflen_;
    char* buf_;
    map<string, vector<Message*> > messageMap;

    void storeMessage(Message* message) {

      string name = message->name;
      
      if(messageMap.find(name) == messageMap.end()) {
	//not present in map
	vector<Message*> messages;
	messages.push_back(message);
	messageMap[name]=messages;
      }
      else {
	//name already present in map
	messageMap[name].push_back(message);
      }
    }


};


