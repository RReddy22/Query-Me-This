//
//  Table.hpp
//  CS32_Project4
//
//  Created by Rahul Reddy on 5/31/21.
//

#ifndef Table_hpp
#define Table_hpp

#include <cstdlib>
#include <list>
#include <vector>
#include <iostream>
#include <iterator>
#include <string>

class StringParser
{
  public:
    StringParser(std::string text = "")
    {
        setString(text);
    }

    void setString(std::string text)
    {
        m_text = text;
        m_start = 0;
    }

    bool getNextField(std::string& field);

  private:
    std::string m_text;
    size_t m_start;
};

class Table
	{
	  public:
	    Table(std::string keyColumn, const std::vector<std::string>& columns);
	    ~Table();
	    bool good() const;
	    bool insert(const std::string& recordString);
	    void find(std::string key, std::vector<std::vector<std::string>>& records) const;
	    int select(std::string query, std::vector<std::vector<std::string>>& records) const;

	      // We prevent a Table object from being copied or assigned by
	      // making the copy constructor and assignment operator unavailable.
	    Table(const Table&) = delete;
	    Table& operator=(const Table&) = delete;
	  private:
	    std::vector<std::list<std::vector<std::string>>> hashTable;
        //std::list<std::vector<std::string>> itemList; 
        std::string m_keyColumn;
        std::vector<std::string> m_columns; //check if this should be a pointer
        bool isGood = true;
        int hashFunction(std::string keyValue) const; //possible 2nd parameter = std::vector<std::string> recordVector ?
        int m_keyIndex;
        //enum Ops { <, <=, >, >=, !=, ==, =, LT, LE, GT, GE, NE, EQ };
        bool stringToDouble(std::string s, double& d) const {
            char* end;
            d = std::strtof(s.c_str(), &end);
            return end == s.c_str() + s.size()  &&  !s.empty();
        }
	};


#endif /* Table_hpp */
