#ifndef _IO_FUNCTIONS_H_
#define _IO_FUNCTIONS_H_

#include "basicEdgeGroup.h"
#include "graphGroup.h"
#include "journeyInfo.h"

#include <vector>

extern const bool YES;
extern const bool NO;

////
//// Main I/O Functions:
////

// Reads in an integer from cin
int inputInt(const string& prompt);

// Reads in a string from cin
string inputString(const string& prompt);

// Returns the string form of an integer
string str(int);

// Returns the string form of a float
string str(float, int precision = 2);

// Returns the string form of a double
string str(double, int precision = 2);

// Returns the string form of an unsigned
string str(unsigned);

// Returns the string form of a floatWInf
string str(floatWInf);

// Outputs a string to cout. Appends suffix to the end
void output(const string&, const string& suffix = "\n");
// Like output, but doesn't print when reading from a file.
void outputPrompt(const string&, const string& suffix = "\n");

void outputRed(const string&, const string& suffix = "\n");
void outputGreen(const string&, const string& suffix = "\n");

string colorGreen(const string&);
string colorRed(const string&);

// Outputs a string to cout with a particular alignment and width
void outputLeft(const string&, int);
void outputRight(const string&, int);
void outputCenter(const string&, int);

void outputGreenLeft(const string&, int);
void outputGreenRight(const string&, int);
void outputGreenCenter(const string&, int);

void outputRedLeft(const string&, int);
void outputRedRight(const string&, int);
void outputRedCenter(const string&, int);

void pad(const string&, int);

// Reads in a boolean answer from cin.
// Yes/No answer
bool getChoice(string prompt);

int getGraphNumber();
void incrementGraphNumber();

template <typename T>
void outputVector(const vector<T>& v) {
    for(int i = 0; i < v.size(); i++)
        cout << v[i] << " ";
    cout << endl;
}

////
//// Graph I/O Functions:
////

void readGraph(basicEdgeGroup & inputGroup);
void readGraphFromFile(basicEdgeGroup & inputGroup);
//void readGraphs(vector< basicEdgeGroup > & inputGroups);
void exportGraph(basicEdgeGroup& outputGroup);

// Prints a graph to a file.
void dumpGraph(const graphGroup&, const string& fileLabel);

// print a graph
void printGraph(const graphGroup&);

////
//// Journey I/O Functions:
////

void readJourneys(vector< journeyInfo > & journeysInformation, const basicEdgeGroup& g);
void readJourneysFromFile(vector< journeyInfo > & journeysInformation);

// print all info on journeys
void printJourneys(const journeyGroup&);

// print all info on a journey
void printJourney(const journey&);

// print actual paths of a journey
void printJourneyPaths(const journey&);

// print actual paths of a journey
void printJourneyPath(const journey&);

// print shared & single costs && savings of a journey
void printJourneyCosts(const journey&);

// print shared & single costs && saving of a journey
void printJourneyCost(const journey&);

#endif