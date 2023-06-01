/* 
Name: Rahul Kumar Nalubandhu , Disha Kanavikar
Email: nalubanr@oregonstate.edu , kanavikd@oregonstate.edu
ONID: nalubanr, kanavikd
*/

/*Refrences and citations:
https://cplusplus.com/forum/general/268772/ 
https://stackoverflow.com/questions/257091/how-do-i-flush-the-cin-buffer
https://www.simplilearn.com/tutorials/cpp-tutorial/int-to-string-cpp#:~:text=In%20C%2B%2B%2C%20you%20can,std%3A%3Ato_string(num)%3B
https://en.cppreference.com/w/cpp/iterator/make_move_iterator
https://stackoverflow.com/questions/10750057/how-do-i-print-out-the-contents-of-a-vector
https://cplusplus.com/reference/algorithm/lower_bound/
https://en.cppreference.com/w/cpp/io/c/setvbuf
*/


#include <string>
#include <vector>
#include <iostream>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <memory>
#include <unordered_map>
using namespace std;


const int DEGREE = 4;

class Record {
public:
    int id, manager_id;
    string bio, name;

    Record(vector<string> fields) 
    {
        id = stoi(fields[0]);
        name = fields[1];
        bio = fields[2];
        manager_id = stoi(fields[3]);
    }

    void print() 
    {
        cout << "\tID: " << id << "\n";
        cout << "\tName: " << name << "\n";
        cout << "\tBio: " << bio << "\n";
        cout << "\tManager ID: " << manager_id << "\n";
    }
};

class BPlusNode {
public:
    vector<int> keys;
    vector<BPlusNode*> children; 
    bool is_leaf;
    BPlusNode* parent;
    vector<streampos> record_positions;
    BPlusNode(bool is_leaf = false) : is_leaf(is_leaf), parent(nullptr) {}
};

class BPlusTree {
public:
    BPlusNode* root;
    int degree;
    const int BLOCK_SIZE = 4096;
    int numRecords;
    std::vector<std::string> buffer;
    int buffer_size;

     // Initialize a new block
    std::string initializeBlock()
    {
        std::string block;                                       // Change '\0' to ' '
        // std::cout << "Block length: " << block.length() << "\n"; // for debugging
        return block;
    }

    BPlusTree(int degree = DEGREE) : degree(degree), root(nullptr) {}

    // Searches for the leaf node where the given key (id) should be located
    BPlusNode* search(int id) 
    {
        // If the root is null, the tree is empty, so return nullptr
        if (!root) return nullptr;

        // Start at the root of the B+ tree
        BPlusNode* node = root;

        // Traverse the tree until a leaf node is reached
        while (!node->is_leaf) 
        {
            // Find the first key greater than or equal to the given id
            auto iterator = upper_bound(node->keys.begin(), node->keys.end(), id);
            
            // Calculate the index of the found key in the keys vector
            int index = iterator - node->keys.begin();

            // Move to the child node at the calculated index
            node = node->children[index];
        }

        // Return the leaf node where the given key (id) should be located
        return node;
    }

    void insert(int id, streampos record_position) 
    {
        // cout << "this is id : " << id << endl; //for debugging
        // If the root is null, create a new leaf node as the root
        if (!root) 
        {
            root = new BPlusNode(true);
            root->keys.push_back(id);
            root->record_positions.push_back(record_position);
            return;
        }

        // Search for the leaf node where the new key should be inserted
        BPlusNode* leaf = search(id);

        // Find the position where the key should be inserted in the leaf node's keys vector
        auto iterator = lower_bound(leaf->keys.begin(), leaf->keys.end(), id);
        int index = iterator - leaf->keys.begin();

        // Insert the new key and its record position in the appropriate positions within the leaf node
        leaf->keys.insert(iterator, id);
        leaf->record_positions.insert(leaf->record_positions.begin() + index, record_position);

        // If the leaf node is now full, split the node to maintain the B+ tree properties
        if (leaf->keys.size() > 2 * degree - 1) 
        {
            split_node(leaf);
        }
        // cout << "this is at line 101 : " << id << endl; //for debugging
    }


    // Splits a given node in the B+ tree to maintain the tree properties
    void split_node(BPlusNode* node) 
    {
        // Create a new node and set its properties (leaf status and parent)
        BPlusNode* new_node = new BPlusNode(node->is_leaf);
        new_node->parent = node->parent;

        // Determine the middle index and middle key for the split
        int mid_index = degree - 1;
        int mid_key = node->keys[mid_index];

        // Move the keys from the middle to the end of the node to the new node
        new_node->keys = vector<int>(node->keys.begin() + mid_index , node->keys.end());
        node->keys.erase(node->keys.begin() + mid_index, node->keys.end());

        // If the node is not a leaf, move the children as well
        if (!node->is_leaf)
        {
            new_node->children = vector<BPlusNode*>(node->children.begin() + mid_index , node->children.end());
            node->children.erase(node->children.begin() + mid_index + 1, node->children.end());

            // Update the parent pointers of the moved children
            for (const auto& child : new_node->children) 
            {
                child->parent = new_node;
            }
        }
        // If the node is a leaf, move the record positions as well
        else 
        {
            new_node->record_positions = vector<streampos>(node->record_positions.begin() + mid_index , node->record_positions.end());
            node->record_positions.erase(node->record_positions.begin() + mid_index + 1, node->record_positions.end());
        }

        // If the node has a parent, update its keys and children with the new node
        if (node->parent) 
        {
            BPlusNode* parent = node->parent;
            auto iterator = upper_bound(parent->keys.begin(), parent->keys.end(), mid_key);
            int index = iterator - parent->keys.begin();
            parent->keys.insert(iterator, mid_key);
            parent->children.insert(parent->children.begin() + index + 1, new_node);

            // If the parent becomes full after the split, call split_node on the parent
            if (parent->keys.size() > 2 * degree - 1) 
            {
                split_node(parent);
            }
        }
        // If the node doesn't have a parent, create a new root with the middle key and the two children
        else 
        {
            root = new BPlusNode();
            root->keys.push_back(mid_key);
            root->children.push_back(node);
            root->children.push_back(new_node);
            node->parent = root;
            new_node->parent = root;
        }
    }

    void flushBuffer()
    {
        std::ofstream datafile("EmployeeData", std::ios::out | std::ios::binary | std::ios::app);
        std::ofstream indexfile("EmployeeIndex", std::ios::out | std::ios::binary | std::ios::app);
 
        for (const std::string &block : buffer)
        {
            size_t pos = 0;
            while (pos < block.size())
            {
                size_t newline_pos = block.find('\n', pos);
                if (newline_pos == std::string::npos)
                {
                    break;
                }
                std::string record_str = block.substr(pos, newline_pos - pos);
                std::string id = record_str.substr(0,8);
                // cout << "record id "<< id << endl; //for debugging
                datafile.seekp(0, ios::end);
                long record_address = datafile.tellp();
                // cout<<"rec add - "<<record_address<<"--size - "<<record_str.size()<<endl; //for debugging
                insert(stoi(id), record_address);
                datafile.write(record_str.c_str(), record_str.size());
                datafile.write("\n", 1);
                pos = newline_pos + 1;
            }
        }
        datafile.close();
        indexfile.close();
        buffer.clear();
        buffer_size = 0;
        numRecords = 0;
    }

    // Inserts a record into the data file, updates the index file, and adds it to the B+ tree
    void insertRecord(Record record, ofstream& data_file, ofstream& index_file) 
    {
        // Move the write pointer to the end of the data file and get its position
        data_file.seekp(0, ios::end);
        streampos record_position = data_file.tellp();

        // Writing the record fields to the data file
        data_file.write(reinterpret_cast<const char*>(&record.id), sizeof(record.id));
        data_file.write(record.name.c_str(), record.name.size() + 1);
        data_file.write(record.bio.c_str(), record.bio.size() + 1);
        data_file.write(reinterpret_cast<const char*>(&record.manager_id), sizeof(record.manager_id));
        
        // Writing the record ID and position to the index file
        index_file.write(reinterpret_cast<const char*>(&record.id),sizeof(record.id));
        index_file.write(reinterpret_cast<const char*>(&record_position),sizeof(record_position));

        // Inserting the record ID and position into the B+ tree
        insert(record.id, record_position);

    
        if(numRecords == 0)
        {
            buffer.push_back(initializeBlock());
            numRecords++;
        }
        std::string &last_block = buffer.back();

        // Calculate the space left in the last block of the buffer
        int space_left = BLOCK_SIZE - last_block.size();

        // If there's not enough space left for the record, flush the buffer and add a new block
        if (space_left < (sizeof(record)))
        {
            flushBuffer();   
            buffer.push_back(initializeBlock());
            numRecords++;
            last_block = buffer.back();
        }

        // Append the record to the last block in the buffer
        // last_block.append(record);
        last_block.append("\n");

        // Update the buffer size and the number of records
        buffer_size++;
        numRecords++;
    }

    void createFromFile(string csvFName) 
    {
        ifstream csvFile(csvFName);
        ofstream data_file("EmployeeData", ios::binary);
        ofstream index_file("EmployeeIndex",  ios::binary);

        string line;

        while (getline(csvFile, line)) 
        {
            stringstream ss(line);
            vector<string> fields;
            string field;
            while (getline(ss, field, ',')) 
            {
                fields.push_back(field);
            }
            Record record(fields);
            insertRecord(record, data_file,index_file);
        }
    }
    // i used this to display the sequence of the child nodes just like what data is there in the child nodes
    void seq(){

        int id=11432159;
        // if (!root) return nullptr;
        BPlusNode* node = root;
        while (!node->is_leaf) 
        {
            auto iterator = upper_bound(node->keys.begin(), node->keys.end(), id);
            int index = iterator - node->keys.begin();
        
            for (int i = 0; i < node->keys.size(); i++) {
                cout << node->keys.at(i) << " ";
            }
            // cout<<"line : 73 -  upper bound slection"<<index<<endl; //for debugging

            node = node->children[index];
        }
        // return node;
        //     if (leaf == NULL) {
        //     cout << "No Datayet!" << endl;
        //     return;
        // }
        // while (leaf != NULL) {
        //     for (int i = 0; i < leaf->keys.size(); i++) {
        //         cout << leaf->keys[i] << " ";
        //     }

            // leaf = leaf->keys;
        // }
        // cout << endl;
        }

    // Finds and returns a record in the data file by its ID
    Record findRecordById(int id, ifstream& data_file) 
    {   
        // Search for the leaf node containing the ID in the B+ tree
        BPlusNode* leaf = search(id);
        // cout << "this is in leaf" << leaf << endl; //for debugging

        // Find the position of the ID in the leaf node's keys
        auto iterator = lower_bound(leaf->keys.begin(), leaf->keys.end(), id);
        // cout << "this is in iterator ::" << *iterator << endl; //for debugging 

        int index = iterator - leaf->keys.begin();

        // If the ID is not found, throw an error
        if (iterator == leaf->keys.end() || *iterator != id) 
        {
            // cout<< "this is iterator : " << *iterator << endl; //for debugging
            throw runtime_error("Record not found");
        }

        // Get the record position from the leaf node
        streampos record_position = leaf->record_positions[index];
        data_file.seekg(record_position);
        vector<string> fields(4);
        int temp_id, temp_manager_id;
        data_file.read(reinterpret_cast<char*>(&temp_id), sizeof(int));
        fields[0] = to_string(temp_id);
        getline(data_file, fields[1], '\0');
        getline(data_file, fields[2], '\0');
        data_file.read(reinterpret_cast<char*>(&temp_manager_id), sizeof(int));
        fields[3] = to_string(temp_manager_id);

        // Create and return a Record object from the read fields
        return Record(fields);
    }
};

