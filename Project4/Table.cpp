//
//  Table.cpp
//  CS32_Project4
//
//  Created by Rahul Reddy on 5/31/21.
//

#include <cctype>
#include <functional>
#include "Table.h"
using namespace std;

//check if this parses strings correctly 
bool StringParser::getNextField(std::string& fieldText)
{
    m_start = m_text.find_first_not_of(" \t\r\n", m_start);
    if (m_start == std::string::npos)
    {
        m_start = m_text.size();
        fieldText = "";
        return false;
    }
    if (m_text[m_start] != '\'')
    {
        size_t end = m_text.find_first_of(" \t\r\n", m_start+1);
        fieldText = m_text.substr(m_start, end-m_start);
        m_start = end;
        return true;
    }
    fieldText = "";
    for (;;)
    {
        m_start++;
        size_t end = m_text.find('\'', m_start);
        fieldText += m_text.substr(m_start, end-m_start);
        m_start = (end != std::string::npos ? end+1 : m_text.size());
        if (m_start == m_text.size()  ||  m_text[m_start] != '\'')
            break;
        fieldText += '\'';
    }
    return true;
}

Table::Table(std::string keyColumn, const std::vector<std::string>& columns) {

    hashTable.resize(1000); //initialize the items, so each list is initialized?
    
    //validate the input columns/key column
    
    //DONE
    //the keyColumn might not be any of the column names.
    for (int i = 0; i < columns.size(); ++i) {
        if (columns[i] == keyColumn) {
            isGood = true;
            break;
        }
        else { //if (columns[i] != keyColumn)
            isGood = false;
        }
    
    }
    
    //DONE
    //the columns vector might be empty
    if (columns.empty()) {
        isGood = false;
    }
    
    //DONE
    //does the columns vector contains empty strings?
    for (int i = 0; i < columns.size(); ++i) {
        if (columns[i] == "")
            isGood = false;
    }
    
    //DONE?
    //the columns vector contains duplicate strings
    if (!columns.empty()) {
        for (int i = 0; i < columns.size() - 1; ++i) {
            for (int j = i+1; j < columns.size(); ++j) {
                if (columns[i] == columns[j]) {
                    isGood = false;
                    break;
                }
            }
        }
    }
    
    //store the key column and column names for later
    m_keyColumn = keyColumn;
    m_columns = columns;
    
    //find the index of the key
    for (int i = 0; i < m_columns.size(); ++i) {
        if (m_keyColumn == m_columns[i])
            m_keyIndex = i;
    }
}

//Return true if the table was successfully constructed, and false otherwise.
bool Table::good() const {
    return isGood; //if good is true, then return true. Otherwise, return false
}


Table::~Table()
{
    //no need to have a destructor if I do NOT dynamically allocate any memory
}


//hash function (Private)
int Table::hashFunction(std::string keyValue) const
{
    //The header <functional> defines a hash class template that you can use to hash strings:
	//string s("Hello there");
	//unsigned int h = std::hash<std::string>()(s);
 
    size_t h = hash<string>()(keyValue); 
    
    int index = h % 1000;
    return index;
}

bool Table::insert(const std::string& recordString)
{
    if (!isGood)
        return false;
        
    StringParser recParser(recordString);
    
    //we want to extract one element using the getNextField function to put it in one of the columns of the table
    string s;
    vector<string> newVector;
    
    //check if stringParses accounts for delimiters, single quotes, etc
    for (int i = 0; i < recordString.size(); ++i)
    {
        if (recParser.getNextField(s))
            newVector.push_back(s);
        else
            break;
    }
    
    //checking if the recordString has the same number of "fields" as the "m_columns" vector has elements
    if (newVector.size() != m_columns.size())
        return false; //here to compile
       
    //use hash function
    int newIndex = hashFunction(newVector[m_keyIndex]);
    
    //add 'newVector' to the list at the newIndex (conceptually)
    //you can just add everything to the list (no need to care about duplicates)
    hashTable.at(newIndex).push_back(newVector);

    return true; 
}

void Table::find(std::string key, std::vector<std::vector<std::string>>& records) const
{
    /*
    Replace the value in the second parameter with a vector containing as many elements as there are records in the table whose key field is a string equal to the value of the first parameter. Each of those elements is one of the matching records, represented as a vector whose elements are the fields of that record. The records placed in the vector don't have to be in any particular order. For example, for the table t built in the example shown for the insert function, the assertions in the following should succeed:
     */
     
    if (!isGood)
        return;
       
    //clear the 'records' vector (2nd param), so we can insert stuff into it
    records.clear();
    
    int keyIndex_find = hashFunction(key); //change variable name of 'keyIndex'
    
    list<vector<string>> keyList = hashTable[keyIndex_find];
    list<vector<string>>::iterator it = keyList.begin();
    
    for ( ; it != keyList.end(); ++it) 
        records.push_back(*it);
}

int Table::select(std::string query, std::vector<std::vector<std::string>>& records) const
{
    if (!isGood) {
        records.clear();
        return -1;
    }
        
        
    //check if query is valid
    
    records.clear();
    
    StringParser queryParser(query);
    
    //we want to extract one element using the getNextField function to put it in one of the columns of the table
    string s;
    vector<string> queryVector;
    
    //check if stringParses accounts for delimiters, single quotes, etc
    for (int i = 0; i < query.size(); ++i)
    {
        if (queryParser.getNextField(s))
            queryVector.push_back(s);
        else
            break;
    }
    
    //checking if the queryVector has the same number of "fields" as the "m_columns" vector has elements
    if (queryVector.size() != 3) {
        records.clear();
        return -1; //invalid size of query
    }
        
    bool firstElemInColumns = false;
    bool keyIndexMatchesQuery = false;
    int queryColumn = 0;
    
    for (int i = 0; i < m_columns.size(); ++i)
    {
        //assuming the data will be passed in the correct order
        if (m_columns[i] == queryVector[0]) {
            queryColumn = i; //used in the numerical comparison
            firstElemInColumns = true;

            if (m_keyIndex == i)
                keyIndexMatchesQuery = true;
                
            break;
        }
    }
    
    //if the first element of the query is NOT in the columns vector, then this query is not valid
    if (!firstElemInColumns) {
        records.clear();
        return -1;
    }
    
    //check the 2nd queryVector element (operator)
    
    
    string op = queryVector[1];
    
    //checks if the string is a alphabetical character
    for (int i = 0; i < op.length(); ++i) {
        if (isalpha(op[i])) {
            op[i] = toupper(op[i]);
        }
    }
    
    
    //operators: <, <=, >, >=, !=, ==, and =: STRING comparisons
    if (op == "<" || op == "<=" || op ==  ">" || op == ">=" || op == "!=" || op == "==" || op == "=") {
    
        //case for "==" and "="
        if (op == "==" || op == "=") {
            if (keyIndexMatchesQuery) {
                find(queryVector[2], records);
                return 0;
            }
            
            else { //keyIndexMatchesQuery = false
                for (int i = 0; i < hashTable.size(); ++i) { //vector
                    if (hashTable[i].empty())
                        continue;
                        
                    //otherwise, the hash table is not empty:
                    list<vector<string>> keyList = hashTable[i];
                    list<vector<string>>::iterator it = keyList.begin();
                    
                    while (it != keyList.end()) { //list
                    
                        vector<string> keyVector = *it;
                        //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                            if (keyVector[queryColumn] == queryVector[2])
                                records.push_back(keyVector);
                        //}
                        ++it;
                    }
                }
                return 0;
            }
            
        }
        
        //case for "<"
        else if (op == "<") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (keyVector[queryColumn] < queryVector[2]) { //only difference is changing the operator
                            records.push_back(keyVector);
                        }
                    //}
                    ++it;
                }
            }
            return 0;
        }
        
        //case for "<="
        else if (op == "<=") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (keyVector[queryColumn] <= queryVector[2]) { //only difference is changing the operator
                            records.push_back(keyVector);
                        }
                    //}
                    ++it;
                }
            }
            return 0;
        }
        
        //case for ">"
        else if (op == ">") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (keyVector[queryColumn] > queryVector[2]) { //only difference is changing the operator
                            records.push_back(keyVector);
                        }
                    //}
                    ++it;
                }
            }
            return 0;
        }
        
        //case for ">="
        else if (op == ">=") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (keyVector[queryColumn] >= queryVector[2]) { //only difference is changing the operator
                            records.push_back(keyVector);
                        }
                    //}
                    ++it;
                }
            }
            return 0;
        }
        
        //case for "!="
        else if (op == "!=") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (keyVector[queryColumn] != queryVector[2]) { //only difference is changing the operator
                            records.push_back(keyVector);
                        }
                    //}
                    ++it;
                }
            }
            return 0;
        }
        
        else {
            records.clear();
            return -1;
        }
    }
        
    //LT, LE, GT, GE, NE, EQ: NUMERICAL comparisons
    else if (op == "LT" || op == "LE" || op ==  "GT" || op == "GE" || op == "NE" || op == "EQ") {
        
        //numerical comparison: almost the same implementation as the string comparison
        //need to convert value in the query from a string to a number (done by stringToDouble function)
        //take data in the hash table and convert it to a number (run stringToDouble function on both query and each element in the vector of the hashTable that we are pushing back to records)
        //insert a few if statements in the stringComparison
        //after going through every single data entry, add to a counter if the data is not convertible to a number (return value in the spec)
        
        
        double q;
        
        //if the 3rd element of the queryVector is not a double, return -1
        if (!stringToDouble(queryVector[2], q)) {
            records.resize(0);
            return -1;
        }
        
        int numOfBadEntries = 0;
        //now we check the entries in the hash table to see if they have valid numerical data
        
        double d;
        
        //case for "<"
        if (op == "LT") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (stringToDouble(keyVector[queryColumn], d)) {
                            if (d < q) { //only difference is changing the operator
                                records.push_back(keyVector);
                            }
                        }
                        
                        else
                            ++numOfBadEntries;
                    //}
                    ++it;
                }
            }
            return numOfBadEntries;
        }
            
        //case for "<="
        else if (op == "LE") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (stringToDouble(keyVector[queryColumn], d)) {
                            if (d <= q) { //only difference is changing the operator
                                records.push_back(keyVector);
                            }
                        }
                        
                        else
                            ++numOfBadEntries;
                    //}
                    ++it;
                }
            }
            return numOfBadEntries;
        }
        
        //case for ">"
        else if (op == "GT") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (stringToDouble(keyVector[queryColumn], d)) {
                            if (d > q) { //only difference is changing the operator
                                records.push_back(keyVector);
                            }
                        }
                        
                        else
                            ++numOfBadEntries;
                    //}
                    ++it;
                }
            }
            return numOfBadEntries;
        }
        
        //case for ">="
        else if (op == "GE") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (stringToDouble(keyVector[queryColumn], d)) {
                            if (d >= q) { //only difference is changing the operator
                                records.push_back(keyVector);
                            }
                        }
                        
                        else
                            ++numOfBadEntries;
                    //}
                    ++it;
                }
            }
            return numOfBadEntries;
        }
        
        //case for "!="
        else if (op == "NE") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (stringToDouble(keyVector[queryColumn], d)) {
                            if (d != q) { //only difference is changing the operator
                                records.push_back(keyVector);
                            }
                        }
                        
                        else
                            ++numOfBadEntries;
                    //}
                    ++it;
                }
            }
            return numOfBadEntries;
        }
        
        //case for "=="
        else if (op == "EQ") {
            for (int i = 0; i < hashTable.size(); ++i) { //vector
                if (hashTable[i].empty())
                    continue;
                        
                //otherwise, the hash table is not empty:
                list<vector<string>> keyList = hashTable[i];
                list<vector<string>>::iterator it = keyList.begin();
                    
                while (it != keyList.end()) { //list
                    vector<string> keyVector = *it;
                    //for (int k = 0; k < keyVector.size(); ++k) { //vector of strings
                        if (stringToDouble(keyVector[queryColumn], d)) {
                            if (d == q) { //only difference is changing the operator
                                records.push_back(keyVector);
                            }
                        }
                        
                        else
                            ++numOfBadEntries;
                    //}
                    ++it;
                }
            }
            return numOfBadEntries;
        }
        
        else {
            records.clear();
            return -1;
        }
        
    }
    
    //operator element of query is not valid
    else {
        records.clear();
        return -1;
    }

}


