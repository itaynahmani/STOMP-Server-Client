#include "event.h"
#include <vector>
#include "Summarize.h"
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>
#include <json.hpp>
using json = nlohmann::json;

using namespace std;

Summarize::Summarize() : GameNameTo_UserMap()                         
{}

void Summarize::summary(string game, string user, string filename)
{

    Game requestedGame = (GameNameTo_UserMap[game])[user];
    std::stringstream s;
    std::cout << "before tostring" << std::endl;
    s = requestedGame.toString();
    std::cout << s.str() << std::endl;
     std::cout << "before writeOnJson" << std::endl;
    writeOnFile(s.str(), filename);
}

void Summarize::writeOnFile(string s, string file_name)
{
    ofstream myFile;
    myFile.open(file_name);
    if(myFile){
        myFile << s;
        myFile.close();
    }
    else{
        ofstream newFile(file_name);
        newFile << s;
        newFile.close();
    }
}

void Summarize::addEvent(string user,string game, Event newEvent)
{
    if (!count(gameList.begin(),gameList.end(),game)){//First Apperance of this Game
        std::cout << "first condition" << std::endl;
           gameList.push_back(game);
            map<string,Game> newMap;
            GameNameTo_UserMap[game]=newMap;  
            Game newGame;
            newGame.addEvent(newEvent);
            GameNameTo_UserMap[game][user]=newGame;
    }
    else if(GameNameTo_UserMap[game].find(user)==GameNameTo_UserMap[game].end()){// First Apperance of this Game-User  
            Game newGame;
            newGame.addEvent(newEvent);
            GameNameTo_UserMap[game][user]=newGame;
    }
    else{
        GameNameTo_UserMap[game][user].addEvent(newEvent);
    }
}

Event Summarize::stringToEvent(string stringEvent, string game)
{
    cout << "-------------------------------------- " << endl;
    cout << stringEvent << endl;
    cout << "-------------------------------------- " << endl;
    vector<string> lines = frameSplit(stringEvent);
    vector<string> teamNames = split(game, '_');
    string teamAname = teamNames.at(0);
    string teamBname = teamNames.at(1);
    cout << "got team names: "+ teamAname+", "+teamBname << endl;
    string eventName = returnHeader("event name", lines).substr(1);
    cout << "got event name: "+ eventName << endl;
    string time = returnHeader("time", lines).substr(1);
    cout << "got time: "+time << endl;
     string description = lines[lines.size()-2];
    cout << "got descripition: " + description << endl;
    map<string, string> generalGameUpdates = createGeneralGameUpdates(lines);
    map<string, string> teamAUpdates = createTeamAUpdates(lines);
    map<string, string> teamBUpdates = createTeamBUpdates(lines);
    cout << "maps made" << endl;
    Event e(teamAname, teamBname, eventName, stoi(time), generalGameUpdates, teamAUpdates, teamBUpdates, description);
    return e;
}

std::vector<std::string> Summarize::split(const std::string &s, char delimiter)
{
    std::vector<std::string> result;
    std::size_t start = 0, end = s.find(delimiter);
    while (end != std::string::npos)
    {
        result.push_back(s.substr(start, end - start));
        start = end + 1;
        end = s.find(delimiter, start);
    }
    result.push_back(s.substr(start, end));
    return result;
}
vector<string> Summarize::frameSplit(const string &s)
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

string Summarize::returnHeader(string header, vector<string> lines)
{
    for (string line : lines)
    {
        vector<string> s = split(line,':');
        if (s.at(0) == header)
            return s[1];
    }
    return "";
}
string Summarize::returnDescription(vector<string> lines)
{
    int index;
    string des="";
    for (int i=0;i<lines.size();i++)
    {
        vector<string> s = split(lines[i],':');
        if (s.at(0) == "description")
            index=i+2;
            break;
    }
    for (int i = index; i < lines.size(); i++) {
           des = des + lines[i] + '\n';
        }
    return des;
}
map<string, string> Summarize::createGeneralGameUpdates(vector<string> lines)
{

    int start = 0, end = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines.at(i) == "general game updates:")
            start = i + 1;
        if (lines.at(i) == "team a updates:")
        {
            end = i;
            break;
        }
    }
    map<string, string> generalUpdates;
    for (int i = start; i < end; i++)
    {
        vector<string> update = split(lines.at(i), ':');
        string updateName = update.at(0) + "";
        string updateDescription = update.at(1);
        generalUpdates[updateName] = updateDescription;
    }
    return generalUpdates;
}

map<string, string> Summarize::createTeamAUpdates(vector<string> lines)
{

    int start = 0, end = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines.at(i) == "team a updates:")
            start = i + 1;
        if (lines.at(i) == "team b updates:")
        {
            end = i;
            break;
        }
    }
    map<string, string> teamAupdates;
    for (int i = start; i < end; i++)
    {
        vector<string> update = split(lines.at(i), ':');
        string updateName = update.at(0) + "";
        string updateDescription = update.at(1);
        teamAupdates[updateName] = updateDescription;
    }
    return teamAupdates;
}

map<string, string> Summarize::createTeamBUpdates(vector<string> lines)
{

    int start = 0, end = 0;
    for (int i = 0; i < lines.size(); i++)
    {
        if (lines.at(i) == "team b updates:")
            start = i + 1;
        if (lines.at(i) == "description:")
        {
            end = i;
            break;
        }
    }
    map<string, string> teamBupdates;
    for (int i = start; i < end; i++)
    {
        vector<string> update = split(lines.at(i),':');
        string updateName = update.at(0)+ "";
        string updateDescription = update.at(1);
        teamBupdates[updateName] = updateDescription;
    }
    return teamBupdates;
}