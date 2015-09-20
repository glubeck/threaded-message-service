#include "client.h"

Client::Client() {
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Client::~Client() {
}

void Client::run() {
    // connect to the server and run echo program
    create();
    echo();
}

void Client::create() {
}

void Client::close_socket() {
}

void Client::echo() {
    string line;
    cout << "%";

    // loop to handle user interface
    while (getline(cin,line)) {

      string cache = "";
      bool valid = false;
        // append a newline
        line += "\n";
        
	//parse request
	vector<string> elements = parse_request(line);

	if(elements[0] == "send" && elements.size() == 3) {
	  valid = true;
	  cout << "- Type your message. End with a blank line -" << endl;
	  getline(cin,line);
	  cache += line + "\n";
	  while(line != "") {
	    getline(cin,line);
	    cache += line + "\n";
	  }

	  cache = cache.substr(0, cache.size()-1);

	  stringstream cacheLength;
	  cacheLength << cache.length();
	  cache.insert(0, "put " + elements[1] + " " + elements[2] + " " + cacheLength.str() + "\n");
 
	}
    
	if(elements[0] == "list" && elements.size() == 2) {
	  valid = true;
	  cache = elements[0] + " " + elements[1] + "\n";

	}

	if(elements[0] == "reset" && elements.size() == 1) {
	  valid = true;
	  cache = elements[0] + "\n";

	}

	if(elements[0] == "read" && elements.size() == 3 && isNumber(elements[2])) {
	  valid = true;
	  cache = "get " + elements[1] + " " + elements[2] + "\n";
	}

	if(elements[0] == "quit" && elements.size() == 1) {
	  exit (EXIT_SUCCESS);
	}

	if(valid) {
	  // send request
	  bool success = send_request(cache);
	  //break if an error occurred
	  if (not success) {
	    cout << "did not send" << endl;
	    break;
	  }
	  // get a response
	  success = get_response();
	  // break if an error occurred
	  if (not success) {
	    cout << "did not get response";
            break;
	  }
	}
	cout << "%";
    }
    close_socket();
}

bool Client::send_request(string request) {
    // prepare to send request
    const char* ptr = request.c_str();
    int nleft = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0) {
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

bool Client::get_response() {
    string response = "";
    // read until we get a newline
    while (response.find("\n") == string::npos) {
        int nread = recv(server_,buf_,1024,0);
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
        response.append(buf_,nread);
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
    cout << response;
    return true;
}

vector<string> Client::parse_request(string line) {

  stringstream ss(line);
  string item;
  vector<string> elems;
  while(getline(ss, item, ' ')) {
    elems.push_back(item);
  }
  if(elems.back().length() > 0)
    elems.back() = elems.back().substr(0, elems.back().size()-1);
  return elems;
}

bool Client::isNumber(string str) {

  for (size_t i = 0; i < str.length(); i++)
    if (!isdigit(str[i]))
      return false;

  return true;
}

