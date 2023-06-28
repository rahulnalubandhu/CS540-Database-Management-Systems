/* 
Name: Rahul Kumar Nalubandhu
Email: nalubanr@oregonstate.edu 
ONID: nalubanr
*/

/*Refrences and citations:
https://cplusplus.com/forum/general/268772/ 
https://stackoverflow.com/questions/257091/how-do-i-flush-the-cin-buffer
https://www.educba.com/c-plus-plus-push_back/
https://www.educba.com/c-plus-plus-buffer/
https://www.simplilearn.com/tutorials/cpp-tutorial/int-to-string-cpp#:~:text=In%20C%2B%2B%2C%20you%20can,std%3A%3Ato_string(num)%3B
*/

#include <fstream>
#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <bitset>
#include <cstring>
using namespace std;

class Record
{
public:
    int id, manager_id;
    std::string bio, name;

    Record(vector<std::string> fields)
    {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print()
    {
        cout << "\tID: " << id << "\n";
        cout << "\tNAME: " << name << "\n";
        cout << "\tBIO: " << bio << "\n";
        cout << "\tMANAGER_ID: " << manager_id << "\n";
    }
    // Function to convert a Record object to a comma-separated string representation
    std::string toString()
    {
        // Concatenate the id, name, bio, and manager_id fields with commas as separators
        return std::to_string(id) + "," + name + "," + bio + "," + std::to_string(manager_id);
    }

    // Static function to create a Record object from a comma-separated string representation
    static Record fromString(const std::string &str)
    {
        // Created a vector to store the individual fields of the record
        std::vector<std::string> fields;

        // Created a stringstream object from the input string
        std::stringstream ss(str);

        // Initialize a variable to store each field value (token)
        std::string token;

        // Iterate through the input string, extracting each field value separated by a comma
        while (getline(ss, token, ','))
        {
            // Now Add the extracted field value to the fields vector
            fields.push_back(token);
        }

        // Create and return a Record object using the extracted fields
        return Record(fields);
    }
};

class StorageBufferManager
{

private:
    const int BLOCK_SIZE = 4096;
    std::string file_name;
    int numRecords;
    std::vector<std::string> buffer;
    int buffer_size;
    // int buffer_capacity;

    // Initialize a new block
    std::string initializeBlock()
    {
        std::string block;                                       // Change '\0' to ' '
        // std::cout << "Block length: " << block.length() << "\n"; // for debugging
        return block;
    }

    // Function to write the contents of the buffer to disk and clear the buffer
    void flushBuffer()
    {
        // Print the current size of the buffer for debugging purposes
        // std::cout << "Flushing buffer of size " << buffer_size << "\n";

        // Open the output file for writing in binary mode and append mode
        std::ofstream outfile(file_name, std::ios::out | std::ios::binary | std::ios::app);

        // Iterate through all the blocks in the buffer
        for (const std::string &block : buffer)
        {
            // Initialize the position variable for the current block
            size_t pos = 0;

            // Keep looping until the end of the block is reached
            while (pos < block.size())
            {
                // Find the position of the next newline character in the block
                size_t newline_pos = block.find('\n', pos);

                // If no newline character is found, break the loop
                if (newline_pos == std::string::npos)
                {
                    break;
                }

                // Extract the record string between the current position and the newline position
                std::string record_str = block.substr(pos, newline_pos - pos);

                // Write the record string and a newline character to the output file
                outfile.write(record_str.c_str(), record_str.size());
                outfile.write("\n", 1);

                // Update the position variable to start after the newline character
                pos = newline_pos + 1;
            }
        }

        // Close the output file
        outfile.close();

        // Clear the buffer and reset buffer_size and numRecords variables
        buffer.clear();
        buffer_size = 0;
        numRecords = 0;
    }

    /*functionality: overall for writing out the data i am using only one block or page and writing the data till its max limit is reached in here it is
    4096 and once limit reached flushing out that to disk and writing the remaing in that block and till limit reached and flushing out agin and this is till all the fields in the csv are read*/

    // Function to insert a new record into the buffer
    void insertRecord(Record record)
    {
        // Convert the record object to a string representation
        std::string record_str = record.toString();

        // If there are no records in the buffer yet, create the first block
        if (numRecords == 0)
        {
            // Initialize the first block and add it to the buffer
            buffer.push_back(initializeBlock());

            // Increment the number of records
            numRecords++;
        }

        // Get a reference to the last block in the buffer
        std::string &last_block = buffer.back();
        // cout<< "test1" << endl; // for debugging

        // Calculate the amount of space left in the last block
        int space_left = BLOCK_SIZE - last_block.size();
        // cout << "Buffer size " << space_left << " " << last_block.size() << endl;

        // If there's not enough space in the last block to hold the new record_str, flush the buffer and create a new block
        if (space_left < (record_str.size() + 1))
        {
            // cout << "inside space_left" << endl; // for debugging
            // Flush the buffer to disk
            flushBuffer();

            // Add a new initialized block to the buffer
            buffer.push_back(initializeBlock());
            // cout << "buffer size" << buffer.size() << "\n";

            // Increment the number of records
            numRecords++;

            // Update the reference to the last block
            last_block = buffer.back();
        }

        // Append the record_str and a newline character to the last block
        last_block.append(record_str);
        last_block.append("\n");

        // Increment the buffer_size and numRecords
        buffer_size++;
        numRecords++;
    }

public:
    // The constructor takes a single argument, NewFileName, which represents the name of the file to be managed
    StorageBufferManager(string NewFileName)
        // The member initializer list is used to initialize the file_name, numRecords, and buffer_size data members
        : file_name(NewFileName), numRecords(0), buffer_size(0)
    {
        // Create the file specified by file_name in binary mode
        // This step is necessary to ensure the file exists before any operations are performed on it
        std::ofstream outfile(file_name, std::ios::out | std::ios::binary);

        // Close the output file stream
        // This is done to release any resources held by the output file stream
        outfile.close();
    }

    // Read csv file (Employee.csv) and add records to the (EmployeeRelation)
    void createFromFile(string csvFName)
    {
        // Open the input file stream to read the CSV file
        std::ifstream infile(csvFName);
        // Declare a string variable to hold each line read from the file
        std::string line;
        // Declare an integer variable to keep track of the current line number
        int lineNumber = 1;

        // Read each line from the input file until the end of the file is reached
        while (std::getline(infile, line))
        {
            // for debugging purposes
            // cout << "Processing line " << lineNumber << ": " << line << endl;

            // Increment the line number for the next iteration
            lineNumber++;

            // Declare a vector of strings to hold the individual fields in the current line
            std::vector<std::string> fields;

            // Create a stringstream from the current line to tokenize the fields
            std::stringstream ss(line);
            // Declare a string variable to hold the tokenized field
            std::string token;

            // Tokenize the fields in the line, using a comma as the delimiter
            while (std::getline(ss, token, ','))
            {
                // for debugging purposes
                // cout << "inner while buffer size: " << buffer.size() << "\n";

                // Add the tokenized field to the vector of fields
                fields.push_back(token);
            }

            // Create a Record object from the vector of fields
            Record record(fields);

            // Insert the record into the buffer
            insertRecord(record);

            //  for debugging purposes
            // cout << "Record insertion completed" << endl;
        }

        // Close the input file stream
        infile.close();

        // If there are records remaining in the buffer, flush the buffer to write the records to the file
        if (buffer_size > 0)
        {
            flushBuffer();
        }
    }


    /* Functionality : here in find recordbyid it is writing the data  into one block and once the block limit is reached which is 4096 
    then it is traversing through that block cheking if the given manager id if it is found or not if id is not found then its flushing the block and writing the 
    next data and searching in that block if found its sending the data so here iam just utilising 1 page of the given limit 3 pages*/
    
    Record findRecordById(int id)
    {
        // Herewe are Setting the buffer size to 4096 bytes
        const size_t BUFFER_SIZE = 4096;
        // Create a buffer to hold the contents of the file
        char buffer[BUFFER_SIZE];

        // Open the file for reading in binary mode
        std::ifstream infile(file_name, std::ios::in | std::ios::binary);
        // Create a string to hold the current line being read
        std::string current_line;

        // Continue reading the file until the end is reached
        while (!infile.eof())
        {
            // Read BUFFER_SIZE bytes from the file and store it in the buffer
            infile.read(buffer, BUFFER_SIZE);
            // Print the content of the buffer
            // std::cout << buffer << std::endl;

            // Get the number of bytes actually read from the file
            std::streamsize bytesRead = infile.gcount();
            // Initialize a variable to store the starting index of a line within the buffer
            size_t line_start = 0;

            // Iterate over the characters in the buffer
            for (size_t i = 0; i < bytesRead; ++i)
            {
                // Check if the current character is a newline or if we reached the end of the buffer
                if (buffer[i] == '\n' || i == bytesRead - 1)
                {
                    // Add the current line's content from the buffer to the current_line string
                    current_line.append(buffer + line_start, buffer + i + 1);
                    // cout << "current line(inner): " << current_line << endl;

                    // If the current character is a newline, process the line
                    if (buffer[i] == '\n')
                    {
                        // Convert the current_line string to a Record object
                        Record record = Record::fromString(current_line);
                        // Check if the record's ID matches the given ID
                        if (record.id == id)
                        {
                            // If the record is found, close the file and return the record
                            infile.close();
                            return record;
                        }
                        // Clear the current_line string for the next line
                        current_line.clear();
                    }

                    // Update the line_start index to the next character after the newline
                    line_start = i + 1;
                }
            }
            // Print the value of line_start
            // std::cout << line_start << std::endl;

            // If there's remaining content in the buffer after the last newline, append it to the current_line string
            if (line_start < bytesRead)
            {
                current_line.append(buffer + line_start, buffer + bytesRead);
            }
            // cout << "current line: " << current_line << endl;
        }

        // Close the file
        infile.close();
        // If the record is not found, throw an error
        throw std::runtime_error("Record not found");
    }
};
