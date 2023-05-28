//  Project Identifier: AD48FB4835AF347EB0CA8009E24C3B13F8519882
//
//  project2A.cpp
//  EECS281 Project 2a
//
//  Created by Lydia Kim on 2/1/23.
//

#include "project2A.hpp"
#include <iostream>
#include <string>
#include <getopt.h>
#include <queue>
#include <vector>
#include <algorithm>
#include "xcode_redirect.hpp"
#include "P2random.h"

using namespace std;

class Galaxy {
public:
    //TO DO: MAKE DEPLOYMENT CONSTRUCTOR
    
    void getMode(int argc, char *argv[]);
    void readData();
    //bool battleChecking(size_t planetID);
    int battles(int& si, int& je);
    //having a separate function for printing verbose?
    void printMedian();
    void printGeneralEval();
    void printWatcher();
    
    
    //only one deployment is needed
    class Deployment {
    public:
        int ID;
        int timestamp;
        int generalNumber;
        int forceAmount;
        mutable int troopAmount;
    }; //Deployment struct
    
    //Sith Functor
    class SithComparator {
    public:
        bool operator() (Deployment a, Deployment b) {
            if (a.forceAmount == b.forceAmount) {
                return (a.ID > b.ID);
            }
            return (a.forceAmount < b.forceAmount);
        }
    };

    //Jedi Functor
    class JediComparator {
    public:
        bool operator() (Deployment a, Deployment b) {
            if (a.forceAmount == b.forceAmount) {
                return (a.ID > b.ID);
            }
            return (a.forceAmount > b.forceAmount);
        }
    };
    
    enum class State : char{initial, seenOne, seenBoth, maybeBetter};
    
    class Planet {
    public:
        priority_queue<Deployment, vector<Deployment>, SithComparator> sith;
        priority_queue<Deployment, vector<Deployment>, JediComparator> jedi;
        
        //heap for the median
        priority_queue<int, vector<int>, greater<int>> upper;
        priority_queue<int, vector<int>, less<int>> lower;
                
        int battles = 0;
        
        Deployment attackJedi;
        Deployment attackSith;
        Deployment attackJediMaybe;
        
        Deployment ambushJedi;
        Deployment ambushSith;
        Deployment ambushSithMaybe;
        
        State attack = State::initial;
        State ambush = State::initial;
    }; //planet class
    
    class General {
    public:
        int jediDeployed;
        int sithDeployed;
        int troopDied;
    }; // General struct
    
    int numBattles = 0;
    //command line options
    bool verboseMode = false;
    bool generalMode = false;
    bool medianMode = false;
    bool watcherMode = false;
    
private:
    //reading in input
    string mode = "";
    int numGenerals;
    int numPlanets;
    int randomSeed;
    int numDeployments;
    int arrivalRate;
    
    int generalID;
    int planetID;
    
    int timestamp;
    int currentTimestamp = 0;
    
    int count;
    vector<Planet> planets;
    vector<General> generals;
    
}; // galaxy class


void Galaxy::getMode(int argc, char *argv[]) {
    opterr = false;
    int gotopt = 0, option_index = 0;
    option long_opts[] = {
        { "verbose", no_argument, nullptr, 'v' },
        { "median", no_argument, nullptr, 'm' },
        { "general-eval", no_argument, nullptr, 'g' },
        { "watcher", no_argument, nullptr, 'w' },
        { "help", no_argument, nullptr, 'h' },
        { nullptr, 0, nullptr, '\0' },
    }; //long_options[]
    
    while((gotopt = getopt_long(argc, argv, "vmgwh", long_opts, &option_index)) != -1) {
        switch (gotopt) {
            case 'v':
                verboseMode = true;
                break;
            case 'm':
                medianMode = true;
                break;
            case 'g':
                generalMode = true;
                break;
            case 'w':
                watcherMode = true;
                break;
            case 'h':
                exit(0);
            default:
                cerr << "Error: invalid option \n";
                exit(1);
                
        } //switch ..choice
    } //while
    
} // getMode


int Galaxy::battles(int& je, int &si) {
    int numTroopsLost = 0;
    
    //sith is greater than jedi
    if (si > je) {
        si -= je;
        numTroopsLost = 2 * je;
        je = 0;
    }
    //jedi is greater than sith
    else if (si < je) {
        je-= si;
        numTroopsLost = 2 * si;
        si = 0;
    }
    //sith and jedi are the same
    else {
        numTroopsLost = 2 * si;
        je = 0;
        si = 0;
    }
    
    return numTroopsLost;
}

void Galaxy::readData() {
    string comment;
    //int timestamp;
    int rebalancing = 0;
    string team;
    int force;
    int troop;
    string junkBeginning;
    int prevTimestamp = 0;
    char junk;
    
    stringstream ss;
    
    getline(cin, comment);
    cin >> junkBeginning;
    cin >> mode;
    cin >> junkBeginning;
    cin >> numGenerals;
    generals.resize(numGenerals);
//    if (generalMode == true) {
//        generals.resize(numGenerals);
//    }
    cin >> junkBeginning;
    cin >> numPlanets;
    planets.resize(numPlanets);
    
    cout << "Deploying troops...\n";
    
    if (mode == "PR") {
        cin >> junkBeginning;
        cin >> randomSeed;
        cin >> junkBeginning;
        cin >> numDeployments;
        cin >> junkBeginning;
        cin >> arrivalRate;
        
        P2random::PR_init(ss, randomSeed, static_cast<unsigned int>(numGenerals), static_cast<unsigned int>(numPlanets), static_cast<unsigned int>(numDeployments), static_cast<unsigned int>(arrivalRate));
    }
    
    istream &inputStream = mode == "PR" ? ss : cin;
        
    while (inputStream >> timestamp >> team >> junk >> generalID >> junk >> planetID >> junk >> force >> junk >> troop) {
        
        //the first time stamp does not need to start at 0
                
        //WORK ON THIS PART!!!!!!!!
        if (timestamp != currentTimestamp && (medianMode)) {
            printMedian();
        }
        
        //DL input error checking
        if (mode == "DL") {
            //1. generalID is in range
            if (generalID >= numGenerals) {
                cerr << "Invalid general ID \n";
                exit(1);
            }
            //2. planetID is in range
            else if (planetID >= numPlanets) {
                cerr << "Invalid planet ID \n";
                exit(1);
            }
            //3. force is greater than 0
            else if (force <= 0) {
                cerr << "Invalid force sensitivity level \n";
                exit(1);
            }
            //4. numTroops is greater than 0
            else if (troop <= 0) {
                cerr << "Invalid number of troops \n";
                exit(1);
            }
            //5. timestamps are non-decreasing
            else if (prevTimestamp > timestamp) {
                cerr << "Invalid decreasing timestamp \n";
                exit(1);
            }
            prevTimestamp = timestamp;
        } // DL error checking
        
        //creating Sith object
        if (team == "SITH") {
            Deployment sithTemp;
            sithTemp.timestamp = timestamp;
            sithTemp.forceAmount = force;
            sithTemp.generalNumber = generalID;
            sithTemp.troopAmount = troop;
            sithTemp.ID = count++;
            
            generals[generalID].sithDeployed += sithTemp.troopAmount;
            planets[planetID].sith.push(sithTemp);
            
            if (watcherMode) {
                //ATTACK = Jedi arrived first = cannot be at initial state
                if (planets[planetID].attack == State::seenOne) {
                    if (sithTemp.forceAmount >= planets[planetID].attackJedi.forceAmount) {
                        planets[planetID].attack = State::seenBoth;
                        planets[planetID].attackSith = sithTemp;
                    }
                }
                else if (planets[planetID].attack == State::seenBoth) {
                    if (sithTemp.forceAmount > planets[planetID].attackSith.forceAmount) {
                        planets[planetID].attackSith = sithTemp;
                    }
                }
                else if (planets[planetID].attack == State::maybeBetter) {
                    if ((sithTemp.forceAmount - planets[planetID].attackJediMaybe.forceAmount) > (planets[planetID].attackSith.forceAmount - planets[planetID].attackJedi.forceAmount)) {
                        planets[planetID].attackSith = sithTemp;
                        planets[planetID].attackJedi = planets[planetID].attackJediMaybe;
                        planets[planetID].attack = State::seenBoth;
                    }
                }
                //AMBUSH
                if (planets[planetID].ambush == State::initial) {
                    planets[planetID].ambush = State::seenOne;
                    planets[planetID].ambushSith = sithTemp;
                }
                else if (planets[planetID].ambush == State::seenOne) {
                    if (sithTemp.forceAmount > planets[planetID].ambushSith.forceAmount) {
                        planets[planetID].ambushSith = sithTemp;
                    }
                }
                else if (planets[planetID].ambush == State::seenBoth) {
                    if (sithTemp.forceAmount > planets[planetID].ambushSith.forceAmount) {
                        planets[planetID].ambush = State::maybeBetter;
                        planets[planetID].ambushSithMaybe = sithTemp;
                    }
                }
                else if (planets[planetID].ambush == State::maybeBetter) {
                    if (sithTemp.forceAmount > planets[planetID].ambushSithMaybe.forceAmount) {
                        planets[planetID].ambushSithMaybe = sithTemp;
                    }
                }
            } //sith watcher mode
            
        } //creating sith
        
        //creating Jedi object
        else { // creating Jedi object
            Deployment jediTemp;
            jediTemp.timestamp = timestamp;
            jediTemp.forceAmount = force;
            jediTemp.generalNumber = generalID;
            jediTemp.troopAmount = troop;
            jediTemp.ID = count++;
            //potential***********************************vvvv
            generals[generalID].jediDeployed += jediTemp.troopAmount;
            planets[planetID].jedi.push(jediTemp);
            
            if (watcherMode) {
                //ATTACK = Jedi arrives first
                //the first jedi is seen so it moves to the next state
                if (planets[planetID].attack == State::initial) {
                    planets[planetID].attack = State::seenOne;
                    planets[planetID].attackJedi = jediTemp;
                }
                //the seenOne state: lower force seen so it becomes the new one
                else if (planets[planetID].attack == State::seenOne) {
                    if (jediTemp.forceAmount < planets[planetID].attackJedi.forceAmount) {
                        planets[planetID].attackJedi = jediTemp;
                    }
                }
                //at the seenBoth state but there is a maybe Jedi
                else if (planets[planetID].attack == State::seenBoth) {
                    if (jediTemp.forceAmount < planets[planetID].attackJedi.forceAmount) {
                        planets[planetID].attack = State::maybeBetter;
                        planets[planetID].attackJediMaybe = jediTemp;
                    }
                }
                else if (planets[planetID].attack == State::maybeBetter) {
                    if (jediTemp.forceAmount < planets[planetID].attackJediMaybe.forceAmount) {
                        planets[planetID].attackJediMaybe = jediTemp;
                    }
                }
                
                
                //AMBUSH = Jedi arrives after Sith = cannot be at the initial state
                if (planets[planetID].ambush == State::seenOne) {
                    if (jediTemp.forceAmount <= planets[planetID].ambushSith.forceAmount) {
                        planets[planetID].ambush = State::seenBoth;
                        planets[planetID].ambushJedi = jediTemp;
                    }
                }
                else if (planets[planetID].ambush == State::seenBoth) {
                    if (jediTemp.forceAmount < planets[planetID].ambushJedi.forceAmount) {
                        planets[planetID].ambushJedi = jediTemp;
                    }
                }
                //calculating which is the better fight
                else if (planets[planetID].ambush == State::maybeBetter) {
                    if ((planets[planetID].ambushSithMaybe.forceAmount - jediTemp.forceAmount) > (planets[planetID].ambushSith.forceAmount - planets[planetID].ambushJedi.forceAmount)) {
                        planets[planetID].ambushSith = planets[planetID].ambushSithMaybe;
                        planets[planetID].ambushJedi = jediTemp;
                        planets[planetID].ambush = State::seenBoth;
                    }
                }
            } // Jedi watcher mode
        } // else statement for Jedi
        
        
        
        while ((planets[planetID].jedi.size() > 0 && planets[planetID].sith.size() > 0) && (planets[planetID].jedi.top().forceAmount <= planets[planetID].sith.top().forceAmount) ) {
            //calculating casualties
            int numTroopsLost = battles(planets[planetID].jedi.top().troopAmount, planets[planetID].sith.top().troopAmount);
            numBattles++;
            
            //general mode
            if (generalMode) {
                generals[(planets[planetID].jedi.top().generalNumber)].troopDied += (numTroopsLost/2);
                generals[(planets[planetID].sith.top().generalNumber)].troopDied += (numTroopsLost/2);
            }
            
            //verbose mode
            if (verboseMode) {
                cout << "General " << planets[planetID].sith.top().generalNumber << "'s battalion attacked General " << planets[planetID].jedi.top().generalNumber << "'s battalion on planet " << planetID << ". " << numTroopsLost << " troops were lost.\n";
            }
            
            //median mode
            if (medianMode) {
                if (planets[planetID].lower.size() == 0) {
                    planets[planetID].lower.push(numTroopsLost);
                } //when it is empty
                else {
                    
                    if (planets[planetID].lower.top() >= numTroopsLost) {
                        planets[planetID].lower.push(numTroopsLost);
                            //rebalancing
                        if (planets[planetID].lower.size() > planets[planetID].upper.size()) {
                            rebalancing = planets[planetID].lower.top();
                            planets[planetID].lower.pop();
                            planets[planetID].upper.push(rebalancing);
                        }
                    }
                    //numTroopsLost >
                    else {
                        planets[planetID].upper.push(numTroopsLost);
                        if (planets[planetID].upper.size() > planets[planetID].lower.size()) {
                            rebalancing = planets[planetID].upper.top();
                            planets[planetID].upper.pop();
                            planets[planetID].lower.push(rebalancing);
                        }
                    }
                } // if greater than or equal to 1
                planets[planetID].battles++;
            }
            
            //popping off of the pq
            //there is more sith
            if(planets[planetID].sith.top().troopAmount > planets[planetID].jedi.top().troopAmount) {
                planets[planetID].jedi.pop();
            }
            //there is more jedi
            else if (planets[planetID].sith.top().troopAmount < planets[planetID].jedi.top().troopAmount) {
                planets[planetID].sith.pop();
            }
            //sith and jedi have equal amount
            else {
                planets[planetID].sith.pop();
                planets[planetID].jedi.pop();
            }
        } //while loop for battle checking
        
        currentTimestamp = timestamp;
    } //while loop reading in deployment
} //readData function

void Galaxy::printMedian() {
    for (int i = 0; i < numPlanets; ++i) {
        int median = 0;
        if (planets[i].lower.size() > 0 || planets[i].battles > 0) {
            //odd number and the median is in the upper
            if (planets[i].upper.size() < planets[i].lower.size()) {
                median = planets[i].lower.top();
            }
            //odd number and the median is in the lower
            else if (planets[i].upper.size() > planets[i].lower.size()) {
                median = planets[i].upper.top();
            }
            else {
                median = (planets[i].lower.top() + planets[i].upper.top())/2;
            }
            cout << "Median troops lost on planet " << i << " at time " << currentTimestamp << " is " << median << ".\n";
            
            planets[i].battles = 0;
        } //if statement
    } //for loop through the planets
} //printMedian

void Galaxy::printGeneralEval() {
    cout << "---General Evaluation---\n";
    for (int i = 0; i < numGenerals; ++i) {
        cout<< "General " << i << " deployed " << generals[i].jediDeployed << " Jedi troops and " << generals[i].sithDeployed << " Sith troops, and " << (generals[i].jediDeployed + generals[i].sithDeployed - generals[i].troopDied) << "/" << (generals[i].jediDeployed + generals[i].sithDeployed) << " troops survived.\n";
    }
}

void Galaxy::printWatcher() {
    cout << "---Movie Watcher---\n";
    for (int i = 0; i < numPlanets; ++i) {
        //ambush
        if (planets[i].ambush == State::maybeBetter || planets[i].ambush == State::seenBoth) {
            cout << "A movie watcher would enjoy an ambush on planet " << i << " with Sith at time " << planets[i].ambushSith.timestamp << " and Jedi at time " << planets[i].ambushJedi.timestamp << " with a force difference of " << (planets[i].ambushSith.forceAmount - planets[i].ambushJedi.forceAmount) << ".\n";
        } //ambush if statement
        else {
            cout << "A movie watcher would not see an interesting ambush on planet " << i << ".\n";
        } //ambush else statement
        
        //attack
        if (planets[i].attack == State::maybeBetter || planets[i].attack == State::seenBoth) {
            cout << "A movie watcher would enjoy an attack on planet " << i << " with Jedi at time " << planets[i].attackJedi.timestamp << " and Sith at time " << planets[i].attackSith.timestamp << " with a force difference of " << (planets[i].attackSith.forceAmount - planets[i].attackJedi.forceAmount) << ".\n";
        }//attack if statement
        else {
            cout << "A movie watcher would not see an interesting attack on planet " << i << ".\n";
        } //attack else statement
    } //for loop going through every planet
} //printWatcher function



int main(int argc, char *argv[]) {
    ios_base::sync_with_stdio(false);
    xcode_redirect(argc, argv);
    
    Galaxy match;
    match.getMode(argc, argv);
    match.readData();
    //^^prints the verbose
    
    //printing out the median
    if (match.medianMode) {
        match.printMedian();
    }
    
    //end of day summary
    cout << "---End of Day---\n";
    cout << "Battles: " << match.numBattles << "\n";
    
    //print out for if general true
    if (match.generalMode) {
        match.printGeneralEval();
    }
    
    //print out for if watch
    if (match.watcherMode) {
        match.printWatcher();
    }

    return 0;
} //main
