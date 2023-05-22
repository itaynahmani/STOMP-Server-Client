#pragma once

#include "../include/ConnectionHandler.h"
#include "../include/event.h"
#include <map>
#include <vector>
#include <list>
#include "Summarize.h"
#include "Game.h"
using namespace std;
class Event;

class StompProtocol
{
private:
    string user;
    ConnectionHandler* myCH;
    bool isConnected;
    int subId;
    int receiptId;
    map<string, int> topicToSub;
    map<int, string> SubToTopic;
    map<string, string > recToMsg;
    int pendingReceipt;
    int pendingSubId;
    Summarize sum;

public:
    StompProtocol(ConnectionHandler &CH);
    vector<string> inputProcess(string message);
    void responseProcess(string message);
    vector<string> frameSplit(const string &s);
    vector<string> commandSplit(const string &s, char delimiter);
    string returnHeader(string header, vector<string> lines);
    string getBody(vector<string> parts);
    string eventToStringForSend(Event& event, std::string user);
};