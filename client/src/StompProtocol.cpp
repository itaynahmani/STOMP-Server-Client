#include "StompProtocol.h"
#include <vector>
#include <iostream>
#include <string>

class Event;
class Summarize;

using namespace std;

StompProtocol::StompProtocol(ConnectionHandler &CH) : myCH(&CH), subId(0), receiptId(1), topicToSub(), SubToTopic(), recToMsg(), pendingReceipt(0), user(), pendingSubId(),
 isConnected(false),sum()
{
    //topicToReportList = new map<string,vector<string>>;
}

void StompProtocol::responseProcess(string frame)
{
    vector<string> lines = frameSplit(frame);

    if (lines[0] == "CONNECTED")
    {
        isConnected = true;
        cout << ("Login Successful") << endl;
    }
    else if (lines[0] == "ERROR")
    {
        string reason = lines[lines.size()-1];
        cout << (reason) << endl;
        myCH->close();
    }
    else if (lines[0] == "MESSAGE")
    {
        vector<string> parts = frameSplit(frame);
        string game = returnHeader("destination", parts);
        string givenUser = parts[5].substr(6);
        cout << "got the user: "+givenUser << endl;
        string messageBody = getBody(parts);
        Event e = sum.stringToEvent(messageBody,game);
        cout << "after making event"<< endl;
        sum.addEvent(givenUser,game,e);
        cout << "after addind event to sum"<< endl;
    }
    else if (lines[0] == "RECEIPT")
    {
        cout << ("enter receipt") << endl;
        string receipt = returnHeader("receipt-id", lines);

        if (recToMsg.find(receipt) == recToMsg.end())
        {}
        else
        {
            string value = recToMsg.at(receipt);
            if (value == "close")
            {
                cout << ("Logout Successfuly") << endl;
                myCH->setConnection(false);
                myCH->close();
            }
            else
            {
                cout << value.data() << endl;
            }
        }
    }
}

vector<string> StompProtocol::inputProcess(string message)
{
    cout << "entered the proccess" << endl;

    vector<string> frames;

    vector<string> parts = commandSplit(message, ' ');
    string frame = "";
    string command = "";
    if (!parts.empty()){
        command = parts[0];}

    if (command == "login")
    {
        if(!myCH->isConnected()) {
            string hostPort = parts[1]; //login 127.0.0.1:7777 meni g
            cout << hostPort << endl;
            vector<string> splitHP = commandSplit(hostPort, ':');

            string host = splitHP[0];
            short port = stoi(splitHP[1]);
            myCH->setHostAndPort(port,host);
            
             if (!myCH->connect()) {
                    cerr << "Cannot connect to " << host << ":" << port << endl;
                    return {""};
             }
             myCH->setConnection(true);
        }     
        frame = "CONNECT\n""accept-version:1.2\n""host:stomp.cs.bgu.ac.il""\n""login:" + parts[2]+ "\n""passcode:" + parts[3] + "\n""\n";
        user= parts[2];
        cout << frame << endl;

    }
    if(myCH->isConnected()){
     if (command == "join") 
    {
        string topic = parts[1];
        int currId = subId++;
        SubToTopic[currId] = topic;
        topicToSub[topic] = currId;
        pendingReceipt = receiptId;
        pendingSubId = currId;

        recToMsg[to_string(pendingReceipt)] = "Joined channel " + topic;

        // Send a SUBSCRIBE command to the server
        frame = "SUBSCRIBE\n"
                "destination:" +
                topic + "\n"
                        "id:" +
                to_string(currId) + "\n"
                                    "receipt:" +
                to_string(receiptId) + "\n"
                                       "\n"
                                       "\0";
        receiptId++;
    }
    else if (command == "exit")
    {
        string topic = parts[1];
        int currId = -1;
        if (topicToSub.count(topic) > 0)
        {
            currId = topicToSub[topic];
            pendingReceipt = receiptId;
            recToMsg[to_string(pendingReceipt)] = "Exited channel " + topic;
        }
        // Send a UNSUBSCRIBE command to the server
        frame = "UNSUBSCRIBE\n"
                "id:" +
                to_string(currId) + "\n"
                                    "receipt:" +
                to_string(receiptId) + "\n"
                                       "\n"
                                       "\0";
        receiptId++;
    }
    else if (command == "logout")
    {
        topicToSub.clear();
        subId = 0;
        pendingReceipt = receiptId;

        // Send a DISCONNECT command to the server
        frame = "DISCONNECT\n"
                "receipt:" +
                to_string(receiptId) + "\n"
                                       "\n"
                                       "\0";

        recToMsg[to_string(pendingReceipt)] = "close";
    }
    else if (command == "report")
    {
        string json_path = parts[1];
        cout << "before reading json" << endl;
        names_and_events reportedFile = parseEventsFile(json_path);
        cout << "after reading json" << endl;
        for (Event e : reportedFile.events)
        {
            string eventString = eventToStringForSend(e, user);
            cout << "\n"
                 << eventString << "\n"
                 << endl;
            frames.push_back(eventString);
        }
        cout << "going to return"
             << "\n"
             << endl;
        return frames;
    }
    else if (command == "summary"){
        cout << "entered summary command" << endl;
        string game = parts[1];
        string requestUser = parts[2];
        string json_path = parts[3];
        cout << "got the parts, entering summary" << endl;
        sum.summary(game, requestUser, json_path);
        cout << "after summary" << endl;
    }
    }
    else
    {}

    frames.push_back(frame);
    return frames;
}

string StompProtocol::eventToStringForSend(Event &event, string user)
{

    string eventSend = "";
    eventSend = "SEND\n"
                "destination:" +
                event.get_team_a_name() + "_" + event.get_team_b_name() + "\n" + "\n" +
                "user: " + user + "\n" +
                event.toString() + "\n" + "\0";
    return eventSend;
}

vector<string> StompProtocol::commandSplit(const string &s, char delimiter)
{
    vector<string> vec;
    string token;
    istringstream token_stream(s);
    while (getline(token_stream, token, delimiter))
    {
        vec.push_back(token);
    }
    return vec;
}

vector<string> StompProtocol::frameSplit(const string &s)
{
    vector<string> vec;
    istringstream inputStream(s);
    string line;
    while (getline(inputStream, line))
    {
        vec.push_back(line);
    }
    return vec;
}

string StompProtocol::returnHeader(string header, vector<string> lines)
{
    for (string line : lines)
    {
        vector<string> s = commandSplit(line, ':');
        if (s.at(0) == header)
            return s[1];
    }
    return "";
}
string StompProtocol::getBody(vector<string> parts) {
        int index = 0;
        string body = "";
        for (int i = 0; i < parts.size(); i++) {
            if (parts.at(i).find("user:")!=string::npos){
                index = i;
                break;
            }
        }
        for (int i = index; i < parts.size(); i++) {
            body = body + parts[i] + '\n';
        }
        return body;
    }