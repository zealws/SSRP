#include "nashTest.hpp"
#include "debug.h"

bool nashEquilibrium(graphGroup& g) 
{
    bool result = true;
    
    vector<path> final_paths = g.returnSharedPaths();
    vector<floatWInf> final_costs = g.returnSharedCosts();

    output("searching for a nash equilibrium");
    
    //loop through each journey
    //if a journey can improve start back at journey 0
    //if all journeys can not improve an equilibrium has been found
        
  
     
    
    int j = 0; 
    while(j < g.numJourneys()) 
    {
        path new_path;
        g.removeJourney(j);
        g.addJourneySP(j);
        floatWInf x = g.returnSharedCost(j);
        if( x < final_costs[j]) 
        {
            final_costs[j] = x;
            output("journey " + str(j) + " has a better strategy");
            j = 0;
            output("journey " + str(j) + " has no incentive to move.");
        }
        
        j++;
    }

    output("This solution is a nash equilibrium");
    
    result = true;
    return result;
}