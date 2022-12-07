#pragma once
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

//class that contains the data for sightings
struct Data {
    string country = "";
    string city = "";
    string state = "";
    string date_time = "";
    string link = "";
    double latitude = 0;
    double longitude = 0;
    double distance = -1;
    int inputOrder = 0;
};

//class for the Hash Table recreation
class HashTable {
    //private hashtable and variable declarations that are only visible within this class
private:
    vector <Data> hashTable;
    double distance(double lat1, double long1, double lat2, double long2);
    int currentNumberBuckets = 10;
    int currentFilledBuckets = 0;
    double currentLoadFactor = 0;
    double loadFactorThreshhold = 0.8;

    //public methods that can be called on the HashTable class
public:
    void insert(Data& data);
    void erase(Data& data);
    void max_load_factor(double maxLoad);
    int bucket_count();
    int filled_bucket_count();
    double load_factor();
    double max_load_factor();
    Data* find(Data& data);
    Data closestLocation();
    HashTable make_HashTable(double& input_latitude, double& input_longitude);
};

double HashTable::distance(double lat1, double long1, double lat2, double long2) {
    double radius = 6371; //earth radius is 6371 km
    double pi = 3.14159265358979323846;
    lat1 = lat1 * pi / 180;
    lat2 = lat2 * pi / 180;
    long1 = long1 * pi / 180;
    long2 = long2 * pi / 180;
    return 2.0 * radius * asin(pow(pow(sin((lat2 - lat1) / 2.0), 2.0) + cos(lat1) * cos(lat2) * pow(sin((long2 - long1) / 2.0), 2.0), 0.5));
}

//read file and insert data into the hashtable, then returns the hashtable
HashTable HashTable::make_HashTable(double& input_latitude, double& input_longitude) {

    //create heap and open file
    HashTable hashTable;
    ifstream file;
    file.open("nuforc_reports.csv");
    string line = "";
    string info = "";
    stringstream ss;
    bool empty_info = false;

    //read file and input data into the HashTable
    while (getline(file, line)) {

        ss << line;
        ss << ",";

        Data data;

        for (int i = 0; i < 7; ++i) {

            getline(ss, info, ',');

            if (info == "") {
                empty_info = true;
                break;
            }

            switch (i) {
            case 0:
                data.country = info;
                break;
            case 1:
                data.city = info;
                break;
            case 2:
                data.state = info;
                break;
            case 3:
                data.date_time = info;
                break;
            case 4:
                data.link = info;
                break;
            case 5:
                data.latitude = stod(info);
                break;
            case 6:
                data.longitude = stod(info);
                break;
            }

        }

        if (!empty_info) {
            //only put data into heap that has no missing info
            data.distance = distance(input_latitude, input_longitude, data.latitude, data.longitude);
            hashTable.insert(data);
        }

        else {
            //clear stream because it may have remnants of the old data being read
            ss.str(std::string());
            empty_info = false;
        }
    }

    //no more need for file so close
    file.close();

    return hashTable;
}

//method for inserting new Data objects into the HashTable
void HashTable::insert(Data& data)
{
    //variable declarations, converting the latitude to a string so I can do a ASCII conversion on it
    string changedLatitude = to_string(data.latitude);
    int sumOfLatitude = 0;
    int index = 0;

    //adds the ascii value of all of the digits within the latitude including the period
    for (int i = 0; i < changedLatitude.length(); i++)
    {
        sumOfLatitude += (int)changedLatitude[i];
    }

    //checks if the vector is empty, will expand it to 10 with stock values if that is the case
    if (hashTable.size() == 0)
    {
        hashTable.resize(10);
    }

    //starts by getting the index which is the remainder of the ASCII interpretation of the latitude divided by the hash table size
    index = sumOfLatitude % hashTable.size();

    //if the index is free, then it will update all of the default values to the new data values
    if (hashTable[index].city == "" && hashTable[index].country == "" && hashTable[index].state == "" && hashTable[index].date_time == ""
        && hashTable[index].latitude == 0 && hashTable[index].longitude == 0 && hashTable[index].distance == -1)
    {
        hashTable[index].country = data.country;
        hashTable[index].city = data.city;
        hashTable[index].state = data.state;
        hashTable[index].date_time = data.date_time;
        hashTable[index].link = data.link;
        hashTable[index].latitude = data.latitude;
        hashTable[index].longitude = data.longitude;
        hashTable[index].distance = data.distance;
        hashTable[index].inputOrder = currentFilledBuckets;

        //increments the buckets to account for the newly filled value
        currentFilledBuckets += 1;
    }
    else
    {
        //I had inspiration from the wikipedia page on quadratic probing in addition to slide 32 of the Hashtable, maps, and sets
        // slides which can be found at the following links: 
        // https://en.wikipedia.org/wiki/Quadratic_probing and https://ufl.instructure.com/courses/460732/pages/module-7-sets-maps-and-hash-tables
        //otherwise the bucket is filled already so it will start it's quadratic probing technique
        for (int i = 0; i < hashTable.size(); i++)
        {
            //the previous index is updated by adding the quadratic index and getting the remainder of that divided by the current hash table size
            index = (index + (int)pow(i, 2)) % hashTable.size();

            //checks to see if the new index is free within the hash table
            if (hashTable[index].city == "" && hashTable[index].country == "" && hashTable[index].state == "" && hashTable[index].date_time == ""
                && hashTable[index].latitude == 0 && hashTable[index].longitude == 0 && hashTable[index].distance == -1)
            {
                hashTable[index].country = data.country;
                hashTable[index].city = data.city;
                hashTable[index].state = data.state;
                hashTable[index].date_time = data.date_time;
                hashTable[index].link = data.link;
                hashTable[index].latitude = data.latitude;
                hashTable[index].longitude = data.longitude;
                hashTable[index].distance = data.distance;
                hashTable[index].inputOrder = currentFilledBuckets;

                //increments the buckets if the free space is found and breaks the quadratic probing loop
                currentFilledBuckets += 1;
                break;
            }
        }
    }

    //The program recalulates the current load factor of the hash table
    currentLoadFactor = ((double)currentFilledBuckets / currentNumberBuckets);

    //in the event that the current load factor reaches the threshold of half of the buckets, it will double the size of the vector and recalculate the current load factor
    //to ensure performance is acceptable
    if (currentLoadFactor >= loadFactorThreshhold)
    {
        //doubles the size of the vector, the new values are assigned to the default value of our data objects
        hashTable.resize(hashTable.size() * 2);

        //updates the current bucket total and the current load factor
        currentNumberBuckets = hashTable.size();
        currentLoadFactor = ((double)currentFilledBuckets / hashTable.size());
    }
};

//method to return the current size of the HashTable vector which is the number of buckets it holds
int HashTable::bucket_count()
{
    return currentNumberBuckets;
}

int HashTable::filled_bucket_count()
{
    return currentFilledBuckets;
}

//method to return the current load factor based on the number of filled buckets out of the total buckets
double HashTable::load_factor()
{
    return currentLoadFactor;
}

//method to return the current load factor threshold
double HashTable::max_load_factor()
{
    return loadFactorThreshhold;
}

//method to allow setting a new load factor threshhold
void HashTable::max_load_factor(double maxLoad)
{
    loadFactorThreshhold = maxLoad;
}

//method to return a pointer to a data object to get the contents of it
Data* HashTable::find(Data& data)
{
    //it will traverse the entire vector to look for the object
    for (int i = 0; i < hashTable.size(); i++)
    {
        //checks to make sure all of the data properties match and will return the pointer if it is the case
        if (hashTable[i].city == data.city && hashTable[i].country == data.country && hashTable[i].state == data.state && hashTable[i].date_time == data.date_time
            && hashTable[i].latitude == data.latitude && hashTable[i].longitude == data.longitude && hashTable[i].distance == data.distance)
        {
            return &hashTable[i];
        }
    }

    //otherwise the data entity is not in the vector Hash Table
    return nullptr;
}

//method to find the closest data for a UFO to the input latitude and longitude
Data HashTable::closestLocation()
{
    //arbitrary values
    double minDistance = -2;
    Data outputData;

    //finds the first index in the hash table that isn't default and assings the distance and return data to it
    for (int i = 0; i < hashTable.size(); i++)
    {
        if (hashTable[i].distance != -1)
        {
            minDistance = hashTable[i].distance;
            outputData = hashTable[i];
            break;
        }
    }

    //going over the entire hash table, the program searches for the closest distance
    for (int i = 0; i < hashTable.size(); i++)
    {
        //if the data isn't a default distance and the distance at the index is smaller than the current minimum, it will update
        if (hashTable[i].distance <= minDistance && hashTable[i].distance != -1)
        {
            if (hashTable[i].distance == minDistance)
            {

                if (hashTable[i].inputOrder < outputData.inputOrder)
                {
                    minDistance = hashTable[i].distance;
                    outputData = hashTable[i];
                }
            }
            else
            {
                minDistance = hashTable[i].distance;
                outputData = hashTable[i];
            }
        }
    }

    //returns the closest distance
    return outputData;
}

//erase method to find the data object in the hash table and resets it to a default object status 
void HashTable::erase(Data& data)
{
    //goes through the entire hash table to try and find the correct data point
    for (int i = 0; i < hashTable.size(); i++)
    {
        //if the data point is within the hash table, it will wipe the data
        if (hashTable[i].city == data.city && hashTable[i].country == data.country && hashTable[i].state == data.state && hashTable[i].date_time == data.date_time
            && hashTable[i].latitude == data.latitude && hashTable[i].longitude == data.longitude && hashTable[i].distance == data.distance)
        {
            hashTable[i].country = "";
            hashTable[i].city = "";
            hashTable[i].state = "";
            hashTable[i].date_time = "";
            hashTable[i].link = "";
            hashTable[i].latitude = 0;
            hashTable[i].longitude = 0;
            hashTable[i].distance = 0;

            //decrements the buckets as a new slot has freed and breaks the loop
            currentFilledBuckets -= 1;
            break;
        }
    }
}