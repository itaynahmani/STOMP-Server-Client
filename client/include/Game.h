#pragma once

#include <list>
#include "event.h"
using namespace std;

class Game
{
private:
    string teamAname;
    string teamBname;
    vector<string> generalUpdates;
    vector<string> team_A_Updates;
    vector<string> team_B_Updates;
    vector<string> gameEventReport;

public:
    Game();
    void addEvent(Event event);
    void addGeneralGameUpdates(Event e);
    void addTeam_A_Updates(Event e);
    void addTeam_B_Updates(Event e);
    void addGameEventReportes(Event e);
    stringstream toString();
};