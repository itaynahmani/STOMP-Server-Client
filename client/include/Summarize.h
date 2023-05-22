#pragma once

#include <list>
#include "event.h"
#include "Game.h"
using namespace std;

class Summarize
{
private:
    map<string, map<string,Game>> GameNameTo_UserMap;
    vector<string> gameList;

public:
    Summarize();
    void summary(string game, string user, string json_path);
    void writeOnFile(string s, string json_path);
    void addEvent(string game,string user,  Event e);
    string returnHeader(string header, vector<string> lines);
    string returnDescription(vector<string> lines);
    Event stringToEvent(string stringEvent, string game);
    vector<string> split(const string &s, char delimiter);
    vector<string> frameSplit(const string &s);
    map<string, string> createGeneralGameUpdates(vector<string> lines);
    map<string, string> createTeamAUpdates(vector<string> lines);
    map<string, string> createTeamBUpdates(vector<string> lines);
};