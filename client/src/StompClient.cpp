#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <iostream>
#include "StompProtocol.h"
#include <ConnectionHandler.h>
#include <stdlib.h>
#include "InputThread.h"
#include <thread>

class InputThread;

using namespace std;

int main (int argc, char *argv[]) {
     std::cout << "entered client" << std::endl;
    ConnectionHandler *connectionHandler = new ConnectionHandler();
	StompProtocol *protocol = new StompProtocol(*connectionHandler);
	InputThread inputThread(*connectionHandler, *protocol);
	thread inThread(&InputThread::run, &inputThread);

	while(1){
        if(connectionHandler->isConnected()){
            string response;
            if (!connectionHandler->getFrameAscii(response, '\0')) {
                std::cout << "Disconnected. Exiting...\n" << std::endl;
                break;
            }
            protocol->responseProcess(response);
        }
    }
    inThread.join();
    delete(connectionHandler);
    delete(protocol);
}
