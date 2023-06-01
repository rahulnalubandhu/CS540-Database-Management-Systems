/* 
Name: Rahul Kumar Nalubandhu , Disha Kanavikar
Email: nalubanr@oregonstate.edu , kanavikd@oregonstate.edu
ONID: nalubanr, kanavikd
*/

/*Refrences and citations:
https://courses.cs.washington.edu/courses/cse326/06au/lectures/lect26.pdf
https://stackoverflow.com/questions/53325184/screen-continuously-record-the-buffer-values-and-save-onto-a-file
https://stackoverflow.com/questions/7577825/understanding-merge-sort-optimization-avoiding-copies
https://cplusplus.com/reference/iomanip/setprecision/
*/

/* This is a skeleton code for Optimized Merge Sort, you can make modifications as long as you meet 
   all question requirements*/  

#include <bits/stdc++.h>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

/***You can change return type and arguments as you want.***/

//Sorting the buffers in main_memory and storing the sorted records into a temporary file (runs) 
// Function to sort a buffer of emp records based on eid
void Sort_Emp_Buffer(int current_buffer_size) 
{
    std::sort(std::begin(buffers), std::begin(buffers) + current_buffer_size, [](const Records& a, const Records& b) 
    {
        return a.emp_record.eid < b.emp_record.eid;
    });
}
// Function to sort a buffer of dept records based on managerid
void Sort_Dept_Buffer(int current_buffer_size) 
{
    std::sort(std::begin(buffers), std::begin(buffers) + current_buffer_size, [](const Records& a, const Records& b) 
    {
        return a.dept_record.managerid < b.dept_record.managerid;
    });
}

// Function to write a buffer of emp records to a temporary file
void Write_Emp_Buffer_To_File(string temp_file, int current_buffer_size) 
{
    fstream output_file;
    output_file.open(temp_file, ios::out);
    // Loop through the buffer and write each record to the file
    for (int i = 0; i < current_buffer_size; i++) 
    {
        output_file << buffers[i].emp_record.eid << ',' << buffers[i].emp_record.ename << ',' << buffers[i].emp_record.age << ',' << std::fixed << std::setprecision(0)<< buffers[i].emp_record.salary << "\n";
    }
    output_file.close();
}

// Function to write a buffer of dept records to a temporary file
void Write_Dept_Buffer_To_File(string temp_file, int current_buffer_size) 
{
    fstream output_file;
    output_file.open(temp_file, ios::out);
    // Loop through the buffer and write each record to the file
    for (int i = 0; i < current_buffer_size; i++) 
    {
        output_file << buffers[i].dept_record.did << ',' << buffers[i].dept_record.dname << ',' << std::fixed << std::setprecision(0)<< buffers[i].dept_record.budget << ',' << buffers[i].dept_record.managerid << "\n";
    }
    output_file.close();
}

//Prints out the attributes from empRecord and deptRecord when a join condition is met 
//and puts it in file Join.csv
//Prints out the attributes from empRecord and deptRecord from file Join.csv
void PrintJoin() {
    std::ifstream joinin("Join.csv", std::ios::in);
    if (!joinin.is_open()) 
    {
        std::cout << "Error opening Join.csv\n";
        return;
    }

    std::string line;
    while (std::getline(joinin, line)) 
    {
        std::cout << line << '\n';
    }

    joinin.close();
}


//Use main memory to Merge and Join tuples 
//which are already sorted in 'runs' of the relations Dept and Emp 
void Merge_Join_Runs(int emp_file_count, int dept_file_count) 
{
    std::fstream joinout;
    joinout.open("Join.csv", std::ios::out);

    std::vector<std::fstream> emp_temp_files(emp_file_count);
    std::vector<std::fstream> dept_temp_files(dept_file_count);

    for (int i = 0; i < emp_file_count; i++)
        emp_temp_files[i].open("emp_temp" + std::to_string(i) + ".csv", std::ios::in);
    for (int i = 0; i < dept_file_count; i++)
        dept_temp_files[i].open("dept_temp" + std::to_string(i) + ".csv", std::ios::in);

    std::vector<Records> emp_records(emp_file_count);
    std::vector<Records> dept_records(dept_file_count);

    // Read the first record from each employee temporary file
    for (int i = 0; i < emp_file_count; i++) 
    {
        emp_records[i] = Grab_Emp_Record(emp_temp_files[i]);
    }

    // Read the first record from each department temporary file
    for (int i = 0; i < dept_file_count; i++) 
    {
        dept_records[i] = Grab_Dept_Record(dept_temp_files[i]);
    }

    // Merge and join the sorted runs
    while (true) 
    {
        int min_emp_index = -1;
        int min_dept_index = -1;
        int min_emp_eid = INT_MAX;
        int min_dept_managerid = INT_MAX;

        // Find the minimum employee eid and department managerID
        for (int i = 0; i < emp_file_count; i++) 
        {
            if (emp_records[i].no_values != -1 && emp_records[i].emp_record.eid < min_emp_eid) {
                min_emp_eid = emp_records[i].emp_record.eid;
                min_emp_index = i;
            }
        }

        for (int i = 0; i < dept_file_count; i++) 
        {
            if (dept_records[i].no_values != -1 && dept_records[i].dept_record.managerid < min_dept_managerid) {
                min_dept_managerid = dept_records[i].dept_record.managerid;
                min_dept_index = i;
            }
        }

        // Check if no more records to process
        if (min_emp_index == -1 || min_dept_index == -1)
            break;

        // If employee EID is less than department manager ID, move to the next employee record
        if (min_emp_eid < min_dept_managerid) 
        {
            emp_records[min_emp_index] = Grab_Emp_Record(emp_temp_files[min_emp_index]);
        }
        // If department manager ID is less than employee EID, move to the next department record
        else if (min_dept_managerid < min_emp_eid) 
        {
            dept_records[min_dept_index] = Grab_Dept_Record(dept_temp_files[min_dept_index]);
        }
        // If employee EID and department manager ID match, write to the output file
        else 
        {
            do 
            {
                if (!dept_records[min_dept_index].dept_record.dname.empty()) 
                {
                    joinout << dept_records[min_dept_index].dept_record.did << ',' << dept_records[min_dept_index].dept_record.dname << ','
                            << std::fixed << std::setprecision(0)<< dept_records[min_dept_index].dept_record.budget << ',' << dept_records[min_dept_index].dept_record.managerid << ','
                            << emp_records[min_emp_index].emp_record.eid << ',' << emp_records[min_emp_index].emp_record.ename << ','
                            << emp_records[min_emp_index].emp_record.age << ',' << emp_records[min_emp_index].emp_record.salary << '\n';
                }
                emp_records[min_emp_index] = Grab_Emp_Record(emp_temp_files[min_emp_index]);
                dept_records[min_dept_index] = Grab_Dept_Record(dept_temp_files[min_dept_index]);

                // Check if there are more department records with the same manager ID
                while (dept_records[min_dept_index].dept_record.managerid == min_dept_managerid) 
                {
                    if (!dept_records[min_dept_index].dept_record.dname.empty()) 
                    {
                        joinout << dept_records[min_dept_index].dept_record.did << ',' << dept_records[min_dept_index].dept_record.dname << ','
                                << std::fixed << std::setprecision(0)<< dept_records[min_dept_index].dept_record.budget << ',' << dept_records[min_dept_index].dept_record.managerid << ','
                                << emp_records[min_emp_index].emp_record.eid << ',' << emp_records[min_emp_index].emp_record.ename << ','
                                << emp_records[min_emp_index].emp_record.age << ',' << emp_records[min_emp_index].emp_record.salary << '\n';
                    }
                    dept_records[min_dept_index] = Grab_Dept_Record(dept_temp_files[min_dept_index]);

                    if (dept_records[min_dept_index].no_values == -1)
                        break;
                }
            } while (emp_records[min_emp_index].emp_record.eid == min_emp_eid);
        }
    }

    // Close the temporary files
    for (int i = 0; i < emp_file_count; i++)
        emp_temp_files[i].close();
    for (int i = 0; i < dept_file_count; i++)
        dept_temp_files[i].close();

    // Close the output file
    joinout.close();
}

void deltempfiles(int emp_file_count, int dept_file_count) 
{
    for (int i = 0; i < emp_file_count; i++)
        remove(("emp_temp" + to_string(i) + ".csv").c_str());
    for (int i = 0; i < dept_file_count; i++)
        remove(("dept_temp" + to_string(i) + ".csv").c_str());
}

int main() {

    // Open two file streams to read from Emp.csv and Dept.csv
    fstream empin;
    fstream deptin;
    empin.open("Emp.csv", ios::in);
    deptin.open("Dept.csv", ios::in);

    // If either of the files can't be opened, print an error message and return 1
    if (!empin.is_open() || !deptin.is_open()) {
    std::cout << "Error opening file(s).\n";
    return 1;
    }

    // Initialize counters for emp and dept files and buffer size
    int emp_file_count = 0, dept_file_count = 0;
    int current_buffer_size = 0;

    // Read emp records into the buffer. Once the buffer is full, sort the buffer,
    // write it to a temporary file, and then reset the buffer
    while ((buffers[current_buffer_size] = Grab_Emp_Record(empin)).no_values != -1) {
        current_buffer_size++;
        if (current_buffer_size == buffer_size) {
            Sort_Emp_Buffer(current_buffer_size);
            Write_Emp_Buffer_To_File("emp_temp" + to_string(emp_file_count) + ".csv", current_buffer_size);
            current_buffer_size = 0;
            emp_file_count++;
        }
    }

    // If there are any remaining emp records in the buffer after reading all the records,
    // sort the buffer, write it to a temporary file
    if (current_buffer_size > 0) {
        Sort_Emp_Buffer(current_buffer_size);
        Write_Emp_Buffer_To_File("emp_temp" + to_string(emp_file_count) + ".csv", current_buffer_size);
        emp_file_count++;
    }

    current_buffer_size = 0;

    // Do the same for dept records as was done for emp records
    while ((buffers[current_buffer_size] = Grab_Dept_Record(deptin)).no_values != -1) {
        current_buffer_size++;
        if (current_buffer_size == buffer_size) {
            Sort_Dept_Buffer(current_buffer_size);
            Write_Dept_Buffer_To_File("dept_temp" + to_string(dept_file_count) + ".csv", current_buffer_size);
            current_buffer_size = 0;
            dept_file_count++;
        }
    }

    if (current_buffer_size > 0) {
        Sort_Dept_Buffer(current_buffer_size);
        Write_Dept_Buffer_To_File("dept_temp" + to_string(dept_file_count) + ".csv", current_buffer_size);
        dept_file_count++;
    }

    // Close both input file streams
    empin.close();
    deptin.close();

    // Merge and join the runs of the Dept and Emp relations
    Merge_Join_Runs(emp_file_count, dept_file_count);

    // Print the join results
    PrintJoin();

    // Delete the temporary files after the join
    deltempfiles(emp_file_count, dept_file_count);

    return 0;
}
