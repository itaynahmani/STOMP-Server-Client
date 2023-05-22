#include "InputThread.h"
#include "ConnectionHandler.h"
#include "StompProtocol.h"
#include <vector>
class ConnectionHandler;


InputThread::InputThread(ConnectionHandler &hand, StompProtocol &protocol):  handler(&hand),protocol(&protocol){}


void InputThread::run() {
    while(1) {
        const short bufsize = 1024;
        char buf[bufsize];
        std::cout << "waiting for input" << std::endl;
        std::cin.getline(buf, bufsize);
        std::string line(buf);
        std::cout << "got input" << std::endl;
        vector<string> frames = protocol->inputProcess(line);
        for(int i=0;i<frames.size();i++){
            string frame = frames[i];
            if(frame!=""){
                if(!handler->sendFrameAscii(frame,'\0')){
                    std::cout << "Disconnected. Exiting...\n" << std::endl;
                    break;
                }
            }
            else{
                // check if to send error
                cout << "else" << endl;
            }
        }
    }
}

InputThread::InputThread(const InputThread &inThread): handler(inThread.handler),protocol(inThread.protocol) {}
InputThread& InputThread::operator=(const InputThread &inThread){
    handler=inThread.handler;
    protocol=inThread.protocol;
    return  *this;
}
InputThread::~InputThread() {
    delete(handler);
    delete(protocol);
}