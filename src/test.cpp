//Christopher Earl
//test.cpp
// contains main() to test various classes
//Version that removed edges from the graph and reroutes

#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <string>

using namespace std;

#include "basicStructures.h"
#include "vertex.h"
#include "path.h"
#include "edge.h"
#include "basicEdgeGroup.h"
#include "journeyInfo.h"
#include "journey.h"
#include "journeyGroup.h"
#include "edgeGroup.h"
#include "graphGroup.h"
#include "ioFunctions.h"
#include "FWGroup.h"
#include "STGroup.hpp"
#include "nashTest.hpp"
#include "debug.h"
#include "options.h"

//returns the index of (the first instance of)"key" in v,
// or -1 if it's not there
template<typename t>
int find_in_vector(const vector<t>& v, t key){
    for(int i = 0; i < v.size(); i++){
        if(v[i] == key)
            return i;
    }
    // if we get here, it's not there
    return -1;
}

const string welcomeHeader =
"\n\
+------------------------------------------------------------------------------+\n\
|                          Shared Shorest Path Project                         |\n\
| Authors: Christopher Earl (2006)                                             |\n\
|          Brooks Emerick (2008)                                               |\n\
|          Sean Quigley (2009)                                                 |\n\
|          Nicole Peterson (2010)                                              |\n\
|          Ronald Fenelus (2011)                                               |\n\
|          Zeal Jagannatha (2011)                                              |\n\
| Mentor:  Sean McCulloch                                                      |\n\
+------------------------------------------------------------------------------+\n";

void runPastHeuristics() {
    graphGroup mainGraph;
    vector< journeyInfo > listsOfJourneys;
    basicEdgeGroup basicGraph;
    vector< vector< floatWInf > > minSavings;
    vector< vector< floatWInf > > maxSavings;
    vector< vector< floatWInf > > averageSavings;
    string fileName;

    output(welcomeHeader);

    // Some simple questions before we start
    bool printGraphInfo = getChoice("Print graph?");

    bool show_reroutings = getChoice("Print journey reroutings?");

    // Generate the graph and journeys
    readGraph(basicGraph);
    readJourneys(listsOfJourneys, basicGraph);

    // Set the journey numbers for each journey:
    // Journey i's journey number is i
    for(int i = 0; i < listsOfJourneys.size(); i++)
        listsOfJourneys[i].setJourneyNum(i);
    mainGraph.set(basicGraph, listsOfJourneys);

    FWGroup FloydPaths;

    mainGraph.set(basicGraph, listsOfJourneys);
    vector<int> journeysNum;

    for(int j = 0; j < listsOfJourneys.size(); j++)
        journeysNum.push_back(listsOfJourneys[j].journeyNum());

    FloydPaths.set(journeysNum, mainGraph);
    // FloydPaths[i].print_cost_grid();

    if(debug)
        output("Running tests...");

    if(printGraphInfo)
        printGraph(mainGraph);

    //route each journey by it's FW path
    for(int j = 0; j < listsOfJourneys.size(); j++){
        mainGraph.addJourney(listsOfJourneys[j].journeyNum(),
                             FloydPaths.returnPath(listsOfJourneys[j].source(),
                                                   listsOfJourneys[j].destination()));
    }

    int num_passes = inputInt("How many times to run through the edges?");
    int journey_threshold =
        inputInt("How many journeys on the edge before we delete the edge?");
    bool coalition_one =
        getChoice("Do you want to change the graph after one coalition edge improves?");
    bool outsider_one =
        getChoice("Do you want to change the graph after one outsider edge improves?");

    for(int cur_pass = 0; cur_pass < num_passes; cur_pass++){

        if(show_reroutings){
            // then show the current journeys paths
            if(debug)
                output("About to rip some stuff up...");
            for(int cur_printing = 0;
                cur_printing < mainGraph.numJourneys();
                cur_printing++)
                printJourney(mainGraph.getJourney(cur_printing));
        }


        //for each edge...
        for(int j = 0; j < mainGraph.returnN(); j++) {
            for(int k = 0; k < mainGraph.returnN(); k++) {
                // if edge (j,k) has > 1 journey on it
                if(mainGraph.numJourneysUsing(j,k) >= journey_threshold ){
                    //then delete the edge- first remove the journeys using it
                    if(debug){
                        output("removing journeys from edge ("
                                + str(j) + "," + str(k) + ")");
                    }

                    vector<int> journeys_rerouted; // list of journeys we rip up
                    vector<path> saved_paths; // paths of the journeys we rip up
                    vector<floatWInf> saved_costs; // costs of the paths we rip up
                    saved_paths = mainGraph.returnSharedPaths();
                    saved_costs = mainGraph.returnSharedCosts();
                    for(int cur_journey = 0;
                        cur_journey < mainGraph.numJourneys();
                        cur_journey++){
                        if(mainGraph.isJourneyIn(j,k,cur_journey)){ // then rip it up

                            mainGraph.removeJourney(listsOfJourneys[cur_journey].journeyNum());
                            journeys_rerouted.push_back(listsOfJourneys[cur_journey].journeyNum());
                            if(debug)
                                output("Ripping up " + str(cur_journey));
                        }
                    }
                    //remove the edge from the graph
                    floatWInf saved_edge_cost = mainGraph.totalEdgeCost(j,k);
                    mainGraph.update_edge_cost(j,k,infinity);

                    //now reroute everyone that was displaced
                    mainGraph.addJourneysSP(journeys_rerouted);
                    bool displaced_improvement = false; // do any dispalced journeys
                    //get better?
                    int displaced_improvement_num = 0;
                    bool outside_improvement = false;    //do any outsiders get better?
                    int outsider_improvement_num = 0;

                    float sad_rat_num = 0;
                    float sad_rat_den = 0;
                    //now see if anyone else wants to move
                    for(int cur_journey = 0; cur_journey < mainGraph.numJourneys();
                        cur_journey++){
                        int my_journey_num = listsOfJourneys[cur_journey].journeyNum();
                        if(find_in_vector(journeys_rerouted,
                                            listsOfJourneys[cur_journey].journeyNum())
                            == -1){
                            //then it's not there

                            mainGraph.removeJourney(my_journey_num);
                            mainGraph.addJourneySP(my_journey_num);
                            if(mainGraph.returnSharedCost(my_journey_num) < saved_costs[my_journey_num])
                                // an outsider got better
                                outsider_improvement_num++;
                        }
                        else{ // then this is a displaced journey
                            if (mainGraph.returnSharedCost(my_journey_num)
                                < saved_costs[my_journey_num])
                                // a displaced journey got better
                                displaced_improvement_num++ ;
                        }
                        floatWInf sad_difference = saved_costs[cur_journey] -
                            mainGraph.returnSharedCost(my_journey_num);
                        if(sad_difference > 0) // cost got better
                            sad_rat_den += sad_difference.value();
                        else  //cost got worse
                            sad_rat_num += -1* sad_difference.value();
                    }  // end loop through all journeys to update the outsiders
                    //and check for improvement

                    //put the edge back
                    mainGraph.update_edge_cost(j,k,saved_edge_cost);
                    if(show_reroutings){
                        // then show the current journeys paths
                        for(int cur_printing = 0; cur_printing < mainGraph.numJourneys(); cur_printing++)
                            printJourney(mainGraph.getJourney(cur_printing));
                    }
                    // did enough displaced journeys improve?
                    if(coalition_one && displaced_improvement_num >0)
                        displaced_improvement = true;
                    if(!coalition_one && displaced_improvement_num > journeys_rerouted.size()/2)
                        displaced_improvement = true;

                    //did enough outsider journeys improve?
                    if(outsider_one && outsider_improvement_num > 0)
                        outside_improvement = true;
                    if(!outsider_one && outsider_improvement_num > (mainGraph.numJourneys() - journeys_rerouted.size())/2)
                        outside_improvement = true;
                    if(displaced_improvement && outside_improvement){
                        // then we keep the new journeys
                        if(debug){
                            output("Keeping the new paths");
                            output("New sad rat : " + str(sad_rat_num*1.0/sad_rat_den));
                            output("New paths:");

                            for(int cur_journey = 0; cur_journey < mainGraph.numJourneys();
                                cur_journey++)
                                printJourney(mainGraph.getJourney(listsOfJourneys[cur_journey].journeyNum()));
                        }
                    }
                    else{ // put the old paths back
                        if(debug){
                            output("No improvement- replacing old paths");
                        }
                        mainGraph.removeAllJourneys();
                        mainGraph.addJourneys(saved_paths);
                    }
                } // end "if >1 journey on this edge"
            }
        } // end "for all edges"
    } // end # of passes

    for(int j = 0; j < mainGraph.numJourneys(); j++) {
        output("Journey " + str(j) + ":");
        printJourney(mainGraph.getJourney(j));
    }

    floatWInf final_total_cost = 0;
    for(int j = 0; j < mainGraph.numJourneys(); j++)
        final_total_cost += mainGraph.returnSharedCost(j);
    output("Final total cost: " + str(final_total_cost));

    //preequilibrium
    dumpGraph(mainGraph);

    // this checks if the final solution is a Nash Equilibrium
    bool nash_equilibrium = nashEquilibrium(mainGraph);

    //equlibrium
    dumpGraph(mainGraph);

    //print spanning tree
    /*
        * currently buggy, not in use
    STGroup st;
    st.findMinSpanningTree(mainGraph.returnGraph());
    vector<journeyInfo> jiplaceholder;
    graphGroup spanningTree = graphGroup(st.returnMinSpanningTree(), jiplaceholder);
    dumpGraph(spanningTree);
    */

    /*output(string("This solution is ")
            + (nash_equilibrium ? "" : "not ")
            + "a Nash Equilibrium.");
            */

    return;
}

int main(int argc, char* argv[]) {

    if(argc > 1) {
        readFromFile = true;
        inFile = new ifstream(argv[1]);
    }
    else {
        readFromFile = false;
        inFile = &cin;
    }

    runPastHeuristics();

    if(inFile != &cin)
        delete inFile;

    return 0;
}