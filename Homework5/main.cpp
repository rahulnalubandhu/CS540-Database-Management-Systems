/* 
Name: Rahul Kumar Nalubandhu 
Email: nalubanr@oregonstate.edu
ONID: nalubanr
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
    // Create the index
    LinearHashIndex employee_index("EmployeeIndex");
    employee_index.createFromFile("Employee.csv");
    
    // Loop to lookup IDs until user is ready to quit
    string input;

    while(true) {
        try {
            cout << "\n "<< "Enter ID or 'q' to quit : ";

            // user input
            cin >> input;

            if(input == "q"){
                break;
            }

            // Convert string to int and find the record with the given ID
            long int in = stol(input);
            Record emp = employee_index.findRecordById(in);
            emp.print();
        } catch (const std::runtime_error& e) {
            // Print error message if no record was found and prompt again
            cout << e.what() << endl;
        } catch (const std::invalid_argument& e) {
            // Print error message if the input is not a number and prompt again
            cout << "\n Invalid input, please enter a number or 'q' to quit." << endl;
        }
    }
    return 1;
}
