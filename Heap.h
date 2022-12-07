#pragma once
#include <iostream>
#include <vector>
using namespace std;

//class that holds data for a sighting
struct HeapData {

    string country = "";
    string city = "";
    string state = "";
    string date_time = "";
    string link = "";
    double latitude = 0;
    double longitude = 0;
    double distance = 0;

};

//class for min heap
class Heap {

private:
    double distance(double lat1, double long1, double lat2, double long2);
public:
    vector<HeapData> heap;
    void insert(HeapData& data);
    HeapData peak_top();
    void pop_top();
    void delete_heap();
    int size();
    Heap make_heap(double& input_latitude, double& input_longitude);

};

void Heap::insert(HeapData& data) {

    //if heap is empty then insert data into index 0 and return
    if (heap.size() == 0) {

        heap.push_back(data);
        return;

    }

    //if heap isn't empty, push data to back
    heap.push_back(data);

    //swap position with parent if inserted data is smaller than parent (continue swaps...
    //...until parent has a smaller distance, or inserted data has reached front aka index 0)
    HeapData temp_data;
    int inserted_index = heap.size() - 1;
    int parent_index = (inserted_index - 1) / 2;

    while (inserted_index != 0 && heap[inserted_index].distance < heap[parent_index].distance) {

        //swap inserted data and parent
        temp_data = heap[parent_index];
        heap[parent_index] = heap[inserted_index];
        heap[inserted_index] = temp_data;

        //update inserted data index and index of its new parent
        inserted_index = parent_index;
        parent_index = (inserted_index - 1) / 2;

    }

}

HeapData Heap::peak_top() {

    //if data exists in heap then return top (data w/ smallest distance)
    if (heap.size() > 0) {

        return heap[0];

    }

    //return empty data by default
    HeapData data;
    return data;

}

void Heap::pop_top() {

    //if there is no data in heap then there is nothing to pop
    if (heap.size() == 0) {
        return;
    }

    //if heap has 1 element then simply pop_back
    if (heap.size() == 1) {

        heap.pop_back();
        return;

    }

    //replace top with last element in heap
    int last_index = heap.size() - 1;
    heap[0] = heap[last_index];
    heap.pop_back();
    last_index = 0;

    //while there are still children, swap with smaller child if distance is greater than children's
    int left_child_index = 2 * last_index + 1;
    int right_child_index = 2 * last_index + 2;
    HeapData temp_data;

    while ((left_child_index <= heap.size() - 1 || right_child_index <= heap.size() - 1)) {

        //if distance is not greater than children's then just return
        if (left_child_index <= heap.size() - 1 && right_child_index <= heap.size() - 1) {

            if (heap[last_index].distance <= heap[left_child_index].distance && heap[last_index].distance <= heap[right_child_index].distance) {
                return;
            }

        }

        else if (left_child_index <= heap.size() - 1) {

            if (heap[last_index].distance <= heap[left_child_index].distance) {
                return;
            }

        }

        else if (right_child_index <= heap.size() - 1) {

            if (heap[last_index].distance <= heap[right_child_index].distance) {
                return;
            }

        }

        //case of 2 children
        if (left_child_index <= heap.size() - 1 && right_child_index <= heap.size() - 1) {

            if (heap[last_index].distance > heap[left_child_index].distance && heap[last_index].distance > heap[right_child_index].distance) {

                if (heap[left_child_index].distance < heap[right_child_index].distance) {

                    temp_data = heap[last_index];
                    heap[last_index] = heap[left_child_index];
                    heap[left_child_index] = temp_data;
                    last_index = left_child_index;
                    left_child_index = 2 * last_index + 1;
                    right_child_index = 2 * last_index + 2;

                }
                else {

                    temp_data = heap[last_index];
                    heap[last_index] = heap[right_child_index];
                    heap[right_child_index] = temp_data;
                    last_index = right_child_index;
                    left_child_index = 2 * last_index + 1;
                    right_child_index = 2 * last_index + 2;

                }

            }
            else if (heap[last_index].distance > heap[left_child_index].distance) {

                temp_data = heap[last_index];
                heap[last_index] = heap[left_child_index];
                heap[left_child_index] = temp_data;
                last_index = left_child_index;
                left_child_index = 2 * last_index + 1;
                right_child_index = 2 * last_index + 2;

            }
            else {

                temp_data = heap[last_index];
                heap[last_index] = heap[right_child_index];
                heap[right_child_index] = temp_data;
                last_index = right_child_index;
                left_child_index = 2 * last_index + 1;
                right_child_index = 2 * last_index + 2;

            }

        }

        //case of left child
        else if (left_child_index <= heap.size() - 1) {

            temp_data = heap[last_index];
            heap[last_index] = heap[left_child_index];
            heap[left_child_index] = temp_data;
            last_index = left_child_index;
            left_child_index = 2 * last_index + 1;
            right_child_index = 2 * last_index + 2;

        }

        //case of right child
        else if (right_child_index <= heap.size() - 1) {

            temp_data = heap[last_index];
            heap[last_index] = heap[right_child_index];
            heap[right_child_index] = temp_data;
            last_index = right_child_index;
            left_child_index = 2 * last_index + 1;
            right_child_index = 2 * last_index + 2;

        }

    }

}

void Heap::delete_heap() {

    int heap_size = heap.size();

    for (int i = 0; i < heap_size; ++i) {

        heap.pop_back();

    }

}

int Heap::size() {

    return heap.size();

}

//distance between coordinates in km
double Heap::distance(double lat1, double long1, double lat2, double long2) {
    double radius = 6371; //earth radius is 6371 km
    double pi = 3.14159265358979323846;
    lat1 = lat1 * pi / 180;
    lat2 = lat2 * pi / 180;
    long1 = long1 * pi / 180;
    long2 = long2 * pi / 180;
    return 2.0 * radius * asin(pow(pow(sin((lat2 - lat1) / 2.0), 2.0) + cos(lat1) * cos(lat2) * pow(sin((long2 - long1) / 2.0), 2.0), 0.5));

}

//read file and insert data into heap, then returns heap
Heap Heap::make_heap(double& input_latitude, double& input_longitude) {

    //create heap and open file
    Heap heap;
    ifstream file;
    file.open("nuforc_reports.csv");
    string line = "";
    string info = "";
    stringstream ss;
    bool empty_info = false;

    //read file and input data into heap
    while (getline(file, line)) {

        ss << line;
        ss << ",";

        HeapData data;

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
            heap.insert(data);

        }

        else {
            //clear stream because it may have remnants of the old data being read
            ss.str(std::string());
            empty_info = false;
        }

    }

    //no more need for file so close
    file.close();

    return heap;

}