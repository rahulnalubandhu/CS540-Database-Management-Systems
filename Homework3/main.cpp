/*
Skeleton code for storage and buffer management
*/

/* 
Name: Rahul Kumar Nalubandhu , Disha Kanavikar
Email: nalubanr@oregonstate.edu , kanavikd@oregonstate.edu
ONID: nalubanr, kanavikd
*/


#include <string>
#include <ios>
#include <fstream>
#include <vector>
#include <string>
#include <string.h>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include "classes.h"
using namespace std;


int main(int argc, char* const argv[]) {

    if (argc != 2) {
        cerr << "Usage: " << argv[0] << " <Employee.csv>" << endl;
        return 1;
    }

    string inputFileName = argv[1];
    // Create the EmployeeRelation file from Employee.csv
    StorageBufferManager manager("EmployeeRelation");
    // cout << "Creating from file: " << inputFileName << endl; here cheking if the correct file  is being called or not // for debugging
    manager.createFromFile(inputFileName);
    
    // Loop to lookup IDs until user is ready to quit
    string input;
    cout << "Enter employee ID to search (or type 'q' to quit): ";
    while (cin >> input && input != "q") {
        try {
            int id = stoi(input); // Convert the string input to an integer
            Record foundRecord = manager.findRecordById(id);
            cout << "Employee found:" << endl;
            foundRecord.print();
        } catch (const runtime_error& e) {
            cout << "Error: " << e.what() << endl;
        } catch (const invalid_argument& e) {
            cout << "Invalid input. Please enter a valid employee ID or type 'q' to quit." << endl;
        }
        cout << "Enter employee ID to search (or type 'q' to quit): ";
    }

    return 0;
}