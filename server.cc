#include <iostream>
#include "server.h"

using namespace std;

Server::Server() {
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
    clientHandler* handler = new clientHandler(buf_);
    this->handler = *handler;
}

Server::~Server() {
    delete buf_;
}

void
Server::run() {
    // create and run the server
    create();
    initializeThreads();
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

        handler.addToQueue(client);
    }
    close_socket();
}

void* handleClients(void* ptr) {

  clientHandler* handler;
  handler = (clientHandler*) ptr;
  handler->handleClients();

}

void Server::initializeThreads() {

  if(threads.size() < 10) {
    for (int i = 0; i < 10; i++) {

      pthread_t* thread = new pthread_t;
      pthread_create(thread, NULL, &handleClients, (void *) &handler);
      threads.push_back(thread);
    }
  }
}
