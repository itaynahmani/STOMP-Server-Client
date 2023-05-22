#include <mutex>
#include "ConnectionHandler.h"
#include "StompProtocol.h"

class InputThread {
private: ConnectionHandler* handler;
         StompProtocol* protocol;
public:

    InputThread(ConnectionHandler& CH,StompProtocol& protocol);
    InputThread(const InputThread& kt);
    InputThread & operator=(const InputThread &kt);
    ~InputThread();
    void run();

};