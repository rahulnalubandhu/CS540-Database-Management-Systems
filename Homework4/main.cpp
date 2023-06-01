/*
Skeleton code for storage and buffer management
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

int main(int argc, char* argv[]) 
{
    if (argc < 2) 
    {
        cerr << "Usage: " << argv[0] << " <Employee.csv>" << endl;
        return 1;
    }

    string csvFName(argv[1]);
    BPlusTree bptree;
    bptree.createFromFile(csvFName);
    int id;
    // bptree.seq();
    while (true) 
    {
        cout << "Enter an Employee ID to search for (0 to exit): ";
        cin >> id;
        if (id == 0) break;

        try 
        {
            ifstream data_file("EmployeeData", ios::binary);
            Record record = bptree.findRecordById(id, data_file);
            cout << "Employee record found:\n";
            record.print();
            data_file.close();
        } 
        catch (const runtime_error& e) 
        {
            cerr << "Error: " << e.what() << endl;
        }
    }

    return 0;
}

