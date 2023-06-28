/* 
Name: Rahul Kumar Nalubandhu 
Email: nalubanr@oregonstate.edu 
ONID: nalubanr
*/

/*Refrences and citations:
https://cplusplus.com/reference/iomanip/setprecision/
https://courses.cs.washington.edu/courses/cse326/06au/lectures/lect26.pdf
https://stackoverflow.com/questions/53325184/screen-continuously-record-the-buffer-values-and-save-onto-a-file
https://www.geeksforgeeks.org/vectorpush_back-vectorpop_back-c-stl/
https://cplusplus.com/reference/tuple/make_tuple/
*/


/* This is a skeleton code for External Memory Sorting, you can make modifications as long as you meet 
   all question requirements*/  

#include <bits/stdc++.h>
#include "record_class.h"

using namespace std;

//defines how many blocks are available in the Main Memory 
#define buffer_size 22

Records buffers[buffer_size]; //use this class object of size 22 as your main memory

/***You can change return type and arguments as you want.***/

//PASS 1
//Sorting the buffers in main_memory and storing the sorted records into a temporary file (Runs) 
void Sort_Buffer(int current_buffer_size) 
{
    sort(buffers, buffers + current_buffer_size, [](const Records& a, const Records& b) 
    {
        return a.emp_record.eid < b.emp_record.eid;
    });
}

// Function to write records from buffer to a file
void writeRecordsToFile(string temp_file, int current_buffer_size )
{
    fstream output_file;
    output_file.open(temp_file, ios::out);
    // Write all records from the buffer to the file
    for ( int i=0; i < current_buffer_size; i++)
    {
        output_file << std::fixed << buffers[i].emp_record.eid << ',' << buffers[i].emp_record.ename << ',' << buffers[i].emp_record.age << ',' << std::setprecision(0) << buffers[i].emp_record.salary << "\n";
    }
    // Close the output file after writing records
    output_file.close();
}

//PASS 2
//Use main memory to Merge the Runs 
//which are already sorted in 'runs' of the relation Emp.csv 

// Function to merge multiple runs (chunks of sorted records)
void Merge_Runs(int runs)
{
    // cout << "outside merge" << endl; //for debugging
    //Creating the EmpSorted.csv file where we will store our sorted results
    fstream SortOut;
    SortOut.open("EmpSorted.csv", ios::out | ios::app);
    // cout << "opened EmpSorted.csv" << endl; //for debugging
    fstream temp[runs]; 
    string input_file;
    // cout << "reading EmpSorted.csv" << endl;//for debugging
    for (int i = 0; i< runs; i++)
    {   
        // cout << "creating temp.csv" << endl; //for debugging
        input_file = "temp"+to_string(i)+".csv";
        temp[i].open(input_file, ios::in);
    }
    // cout << "created temp.csv" << endl; //for debugging
    // Index to keep track of which temporary file to read from
    int tempFileIndex = 0;
    // Counter to keep track of how many files have been processed
    int processedFilesCount =0;
    int number_of_emp_records = 0;

    // Buffer to store records from the runs
	vector<tuple<int, Records>> newbuffer;
	Records  single_EmpRecord;

    // Load the initial records from each run into the buffer	
	for(int i=0; i<buffer_size-1; i++)
	{
		if(tempFileIndex >= runs)
		{
			tempFileIndex = 0;
		}
		single_EmpRecord = Grab_Emp_Record(temp[tempFileIndex]);
		newbuffer.push_back(make_tuple(tempFileIndex, single_EmpRecord));
		tempFileIndex++;
	}
    // cout<<"buffer created"<<endl; //debugging

    // Keep merging records until no more records are left in the runs
    bool flag = true;
    while(flag)
    {
        // Sort the buffer based on employee ID
        sort(newbuffer.begin(), newbuffer.end(), [](const tuple<int, Records>& a, const tuple<int, Records>& b) 
        {
            return get<1>(a).emp_record.eid < get<1>(b).emp_record.eid;
        });

        // Write the smallest record to the sorted output file
        SortOut << std::fixed << get<1> (newbuffer[0]).emp_record.eid << ',' << get<1>(newbuffer[0]).emp_record.ename << ',' << get<1>(newbuffer[0]).emp_record.age << ',' << std::setprecision(0) << get<1>(newbuffer[0]).emp_record.salary <<"\n";
        // Load the next record from the run that the smallest record came from
        Records single_EmpRecord = Grab_Emp_Record(temp[get<0>(newbuffer[0])]);
        // cout<<"records "<<single_EmpRecord.emp_record.eid<<endl; //for debugging

        processedFilesCount = 0;
        tempFileIndex = get<0>(newbuffer[0]);

        // If the run is empty, skip to the next run
        while (single_EmpRecord.no_values == -1 && processedFilesCount < runs)
        {
        // cout<<"while "<<processedFilesCount<<endl; //for debugging
            tempFileIndex++;
            if(tempFileIndex >= runs)
            {
                tempFileIndex=0;
            }
            processedFilesCount++;
            single_EmpRecord = Grab_Emp_Record(temp[tempFileIndex]);
        }
        // If all runs have been processed, stop merging
        if(processedFilesCount == runs)
        {
            // break; // for debugging
            flag = false;
            for(int i=0; i<runs; i++)
            {
                temp[i].close();
            }
        }
        // Otherwise, replace the smallest record with the next record from the same run
        else
        {
            newbuffer[0] = make_tuple(tempFileIndex,single_EmpRecord);
        }
    }
    // cout << "Till now all block are read" << endl; //for debugging
    // Write any remaining records in the buffer to the sorted output file
    for ( int i =1; i<buffer_size-1; i++)
    {
        SortOut << std::fixed << get<1> (newbuffer[i]).emp_record.eid << ',' << get<1>(newbuffer[i]).emp_record.ename << ',' << get<1>(newbuffer[i]).emp_record.age << ',' << std::setprecision(0) << get<1>(newbuffer[i]).emp_record.salary <<"\n";
    }
    // Close the sorted output file
    SortOut.close();
}

// Function to print the sorted records from the buffer
void PrintSorted(int current_buffer_size)
{
    // Iterate over each record in the buffer up to the current_buffer_size
    for (int i = 0; i < current_buffer_size; i++)
	{
        // Print the index (i) and each attribute of the employee record.
        // The attributes printed are Employee ID (eid), Employee name (ename), age and salary.
		cout << std::fixed << i << " " << buffers[i].emp_record.eid  << " " << buffers[i].emp_record.ename << " " << buffers[i].emp_record.age << " " << std::setprecision(0) << buffers[i].emp_record.salary << endl;
	}
}

// Function to delete temporary files
void deltempfiles(int temp)
{
    // Loop through the number of temporary files
    for(int i=0; i<temp; i++)
    {
        // Generate the file name for each temporary file
        std::string input_file = "temp"+std::to_string(i)+".csv";

        // Attempt to remove the file. 
        // The function 'remove' returns 0 if the file is successfully deleted.
        if(remove(input_file.c_str()) == 0)
        {
            // Print a message to indicate that the file has been successfully removed.
            // std::cout << "files has been removed" << std::endl;
        }
        else
        {
            // If the file could not be deleted, print an error message.
            // std::cout << "files are not removed" << std::endl;
        }
    }
}

int main() {
    //Open file streams to read and write
    //Opening out the Emp.csv relation that we want to Sort
    fstream empin;
    empin.open("Emp.csv", ios::in);
    // cout << "Emp.csv is opened" << endl; //for debugging
    // flags check when relations are done being read
	bool flag = true;
	int current_buffer_size = 0;
	int temp_file_count = 0;

    string temp_file_name;

    // Read records from input file, sort them and write to temporary files
    while (flag)
    {
        Records emp_record = Grab_Emp_Record(empin);
        // If end of file is reached, sort the remaining records, write them to file and break the loop
        if(emp_record.no_values == -1)
        {
            cout << "End of the file is reached and sorting the remaing records" << endl;
            flag = false;
            Sort_Buffer(current_buffer_size);
            PrintSorted(current_buffer_size);
            temp_file_name = "temp"+to_string(temp_file_count)+".csv";
            writeRecordsToFile(temp_file_name, current_buffer_size);
            temp_file_count++;
        }
        // If there's space in the buffer, add the record to it
        // Otherwise, sort the full buffer, write it to a temporary file and clear it for the next batch
        if(current_buffer_size + 1 <= buffer_size)
        {
            buffers[current_buffer_size] = emp_record;
            current_buffer_size += 1;
        }
        else
        {
            // If the buffer is full, sort the buffer
            cout << "Main Memory is full. sort and store in a temporary file" << endl;
            Sort_Buffer(buffer_size);
            PrintSorted(buffer_size);
            // Write the sorted buffer to a temporary file
            temp_file_name = "temp"+to_string(temp_file_count)+".csv";
            writeRecordsToFile(temp_file_name, buffer_size);
            temp_file_count++;
            current_buffer_size = 0;
            buffers[current_buffer_size] = emp_record;
            current_buffer_size +=1;

        }
    }
    empin.close();

    // cout << "written " << endl; // for debugging

    //2. Use Merge_Runs() to Sort the runs of Emp relations 
    Merge_Runs(temp_file_count);

    //Please delete the temporary files (runs) after you've sorted the Emp.csv
    deltempfiles(temp_file_count);
    
    return 0;
}
