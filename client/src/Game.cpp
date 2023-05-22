#include <vector>
#include "Game.h"
#include <string>
#include <map>
#include <vector>
#include <sstream>
#include <fstream>

using namespace std;

Game::Game() : teamAname(), teamBname(), generalUpdates(), team_A_Updates(), team_B_Updates(), gameEventReport()
{
}

void Game::addEvent(Event e)
{
    addGeneralGameUpdates(e);
    addTeam_A_Updates(e);
    addTeam_B_Updates(e);
    addGameEventReportes(e);
}

void Game::addGeneralGameUpdates(Event e)
{
    string ss = "";
    map<string, string> generalUpd = e.get_game_updates();

    for (const auto &update : generalUpd)
    {
        ss = "  " + update.first + ": " + update.second + "\n";
        generalUpdates.push_back(ss);
    }
}
void Game::addTeam_A_Updates(Event e)
{
    teamAname = e.get_team_a_name();
    string ss = "";
    map<string, string> teamAUpd = e.get_team_a_updates();

    for (const auto &update : teamAUpd)
    {
        ss = "  " + update.first + ": " + update.second + "\n";
        //cout << "\n" << "the a update is: ------------------- :" << ss << "\n" << endl;

        team_A_Updates.push_back(ss);
    }
}
void Game::addTeam_B_Updates(Event e)
{
    teamBname = e.get_team_b_name();
    string ss = "";
    map<string, string> teamBUpd = e.get_team_b_updates();

    for (const auto &update : teamBUpd)
    {
        ss = "  " + update.first + ": " + update.second + "\n";
        team_B_Updates.push_back(ss);
    }
}


void Game::addGameEventReportes(Event e)
{

    std::stringstream s;

    s << e.get_time() << "-" << e.get_name() << ":" << "\n";
    s << e.get_discription() << "\n";

    gameEventReport.push_back(s.str());
}
stringstream Game::toString()
{

    cout << "entered the toString" << endl;

    std::stringstream s; 
    s << teamAname << " vs " << teamBname << "\n";
    s << "Game stats:"
      << "\n";
    s << "General stats:"
      << "\n";

    for (const auto &update : generalUpdates)
    {
        s << update;
    }

    s << teamAname << " stats:"
      << "\n";

    for (const auto &update : team_A_Updates)
    {
        s << update;
    }

    s << teamBname << " stats:"
      << "\n";

    for (const auto &update : team_B_Updates)
    {
        s << update;
    }

    s << "Game event reports:"
      << "\n";

    for (const auto &update : gameEventReport)
    {
        s << update;
    }

     return s;
}
