#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cstdlib>
#include <iomanip>
#include <map>
#include <random>
#include <vector>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Window/VideoMode.hpp>
#include <SFML/Window.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <chrono>
#include "Heap.h"
#include "HashTable.h"

using namespace std;
using namespace sf;




// used to map latitude and longitude to the map used
pair<double, double> plotCoords(double lat, double lon, unsigned int width, unsigned int height) {
    pair<double, double> coords;


    double ycoordFactorNEG = 724 / 180; // y factor for negative longitudes
    double ycoordFactorPOS = 718 / 180; // y factor for positive logitudes
    double xcoordFactor = 1;

    // find x factor for latitude depending on value
    if (lat >= 60) { // 60-90
        xcoordFactor = 80.0 / 15; // height in pixels / # of degrees
    }
    else if (lat >= 45) { // 45-59
        xcoordFactor = 75.0 / 15;
    }
    else if (lat >= 30) { // 30-44
        xcoordFactor = 75.0 / 15;
    }
    else if (lat >= 15) { // 15-29
        xcoordFactor = 65.0 / 15;
    }
    else if (lat >= 0) { // 0-14
        xcoordFactor = 60.0 / 15;
    }
    else if (lat >= -15) { // -15 to -1
        xcoordFactor = 60.0 / 15;
    }
    else if (lat >= -30) { // 0 to -30
        xcoordFactor = 65.0 / 15;
    }
    else if (lat >= -45) { // -31 to -45
        xcoordFactor = 65.0 / 15;
    }
    else if (lat >= -60) { // -46 to -60
        xcoordFactor = 75.0 / 15;
    }
    else if (lat >= -90) { // -61 to -90
        xcoordFactor = 100.0 / 15;
    }
    if (lat < 0) {
        coords = make_pair((578 - (lat * xcoordFactor)), 724 + (lon * ycoordFactorNEG));
    }
    else {
        coords = make_pair(578 - (lat * xcoordFactor), 724 + (lon * ycoordFactorNEG));
    }
    return coords;
}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1440, 900+500), "window");

    window.setTitle("hot ufos near you!");

    sf::Vector2u size = window.getSize();
    unsigned int width = size.x;
    unsigned int height = size.y;

    // create map
    sf::Texture texture;
    texture.loadFromFile("worldMap.png");

    sf::Sprite sprite;
    sprite.setTexture(texture);

    sprite.setPosition(sf::Vector2f(0, 0));

    // create ufo
    sf::Texture ufoTexture;
    ufoTexture.loadFromFile("ufo.png");
    ufoTexture.setSmooth(true);

    sf::Sprite ufo;
    ufo.setTexture(ufoTexture);

    ufo.scale(sf::Vector2f(0.5, 0.5));
    ufo.setPosition(1000, 1100);
    ufo.setOrigin(ufo.getLocalBounds().width/2, ufo.getLocalBounds().height/2);
    

    // making heap/hash table buttons
    sf::Font font;
    sf::Text heapText;
    heapText.setStyle(sf::Text::Bold);
    if (font.loadFromFile("arial.ttf")) {
        heapText.setFont(font);
    }
    heapText.setCharacterSize(27);
    heapText.setFillColor(sf::Color::Black);
    heapText.setString("Heap");
    heapText.setPosition(173, 1207);

    sf::Text hashText;
    hashText.setStyle(sf::Text::Bold);
    if (font.loadFromFile("arial.ttf")) {
        hashText.setFont(font);
    }
    hashText.setCharacterSize(27);
    hashText.setFillColor(sf::Color::Black);
    hashText.setString("Hash Table");
    hashText.setPosition(319, 1207);

    sf::RectangleShape heapButton(sf::Vector2f(150, 75));
    heapButton.setFillColor(Color(179, 243, 186));
    heapButton.setPosition(135, 1190);

    heapButton.setOutlineColor(Color(149, 192, 150));
    heapButton.setOutlineThickness(3);

    sf::RectangleShape hashButton(sf::Vector2f(150, 75));
    hashButton.setFillColor(Color(179, 243, 186));
    hashButton.setPosition(315, 1190);

    hashButton.setOutlineColor(Color(149, 192, 150));
    hashButton.setOutlineThickness(3);

    // redo button
    sf::RectangleShape redoButton(sf::Vector2f(150, 50));
    redoButton.setFillColor(Color(255, 203, 203));
    redoButton.setPosition(223, 1283);
    redoButton.setOutlineColor(Color(240, 128, 128));
    redoButton.setOutlineThickness(3);

    sf::Text redoText;
    redoText.setStyle(sf::Text::Bold);
    redoText.setFont(font);
    redoText.setCharacterSize(27);
    redoText.setFillColor(sf::Color::Black);
    redoText.setString("Redo");
    redoText.setPosition(265, 1290);

    // adding space background
    sf::Texture spaceTexture;
    spaceTexture.loadFromFile("space.jpg");
    spaceTexture.setSmooth(true);
    sf::Sprite space;
    space.setTexture(spaceTexture);
    space.setPosition(sf::Vector2f(0, height*.5));
    space.setColor(sf::Color(100, 100, 100));

    // creating you are here sprite
    sf::Texture youarehereText;
    youarehereText.loadFromFile("youarehere.png");
    youarehereText.setSmooth(true);
    sf::Sprite youarehere;
    youarehere.setTexture(youarehereText);
    youarehere.scale(0.15, 0.15);
    youarehere.scale(0.01, 0.01);
    
    sf::Event event;

    bool ufoFly = false;

    string userLongitude = "";
    string userLatitude = "";
    
    // creating text sprites for input
    sf::String input;
    sf::Text latText;
    if (font.loadFromFile("arial.ttf")) {
        latText.setFont(font);
    }
    latText.setCharacterSize(32);
    latText.setFillColor(sf::Color::White);

    sf::Text longText;
    if (font.loadFromFile("arial.ttf")) {
        longText.setFont(font);
    }
    longText.setCharacterSize(32);
    longText.setFillColor(sf::Color::White);

    string type = "latitude";

    // creating text areas
    sf::RectangleShape rectangle(sf::Vector2f(400, 425));
    rectangle.setFillColor(Color(160, 160, 160));
    rectangle.setPosition(100, 925);
    rectangle.setOutlineColor(Color(179, 243, 186));
    rectangle.setOutlineThickness(3);

    sf::RectangleShape rectangle2(sf::Vector2f(760, 425));
    rectangle2.setFillColor(Color(160, 160, 160));
    rectangle2.setPosition(590, 925);
    rectangle2.setOutlineColor(Color(179, 243, 186));
    rectangle2.setOutlineThickness(3);

    sf::RectangleShape textbox1(sf::Vector2f(200, 50));
    textbox1.setFillColor(Color(175, 175, 175));
    textbox1.setPosition(200, 975);

    sf::RectangleShape textbox2(sf::Vector2f(200, 50));
    textbox2.setFillColor(Color(175, 175, 175));
    textbox2.setPosition(200, 975);

    textbox1.setOutlineColor(Color::Black);
    textbox2.setOutlineColor(Color::Black);
    textbox1.setOutlineThickness(3);
    textbox2.setOutlineThickness(3);

    // creating text
    string title = "Nearest UFO to You";
    string enterLat = "Latitude: ";
    string enterLong = "Longitude: ";

    sf::Text titleText;
    if (font.loadFromFile("arial.ttf")) {
        titleText.setFont(font);
    }
    titleText.setCharacterSize(32);
    titleText.setFillColor(sf::Color::Black);
    titleText.setString(title);

    sf::Text latEnter;
    if (font.loadFromFile("arial.ttf")) {
        latEnter.setFont(font);
    }
    latEnter.setCharacterSize(32);
    latEnter.setFillColor(sf::Color::Black);
    latEnter.setString(enterLat);

    sf::Text longEnter;
    if (font.loadFromFile("arial.ttf")) {
        longEnter.setFont(font);
    }
    longEnter.setCharacterSize(32);
    longEnter.setFillColor(sf::Color::Black);
    longEnter.setString(enterLong);
    
    int num = 0;

    // initialize coords
    pair<double, double> coords = plotCoords(0, 0, 0, 0);
    pair<double, double> yourcoords = plotCoords(0, 0, 0, 0);

    bool foundCoords = false;
    
    // create result text sprites
    sf::Text distanceText;
    distanceText.setFont(font);
    distanceText.setCharacterSize(32);
    distanceText.setFillColor(sf::Color::Black);
    sf::Text ufoLatText;
    ufoLatText.setFont(font);
    ufoLatText.setCharacterSize(32);
    ufoLatText.setFillColor(sf::Color::Black);
    sf::Text ufoLongText;
    ufoLongText.setFont(font);
    ufoLongText.setCharacterSize(32);
    ufoLongText.setFillColor(sf::Color::Black);
    sf::Text cityText;
    cityText.setFont(font);
    cityText.setCharacterSize(32);
    cityText.setFillColor(sf::Color::Black);
    sf::Text stateText;
    stateText.setFont(font);
    stateText.setCharacterSize(32);
    stateText.setFillColor(sf::Color::Black);
    sf::Text countryText;
    countryText.setFont(font);
    countryText.setCharacterSize(32);
    countryText.setFillColor(sf::Color::Black);
    sf::Text datetimeText;
    datetimeText.setFont(font);
    datetimeText.setCharacterSize(32);
    datetimeText.setFillColor(sf::Color::Black);
    sf::Text linkText;
    linkText.setFont(font);
    linkText.setCharacterSize(24);
    linkText.setFillColor(sf::Color::Black);
    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(32);
    timeText.setFillColor(sf::Color::Black);

    sf::Text loadingText;
    loadingText.setFont(font);
    loadingText.setCharacterSize(32);
    loadingText.setFillColor(sf::Color::White);
    loadingText.setString("Loading...");

    bool showResults = false;
    bool hashButtonClicked = false;
    bool heapButtonClicked = false;

    pair<double, double> userCoordinates;

    float heapTime = 0;
    float hashTime = 0;

    while (window.isOpen()) {
        while (window.pollEvent(event)) {
            switch (event.type)
            {
            case sf::Event::Closed: // if window is closed
                window.close();
                break;

            case sf::Event::KeyPressed:
                
                if (event.key.code == Keyboard::Return) { // if return key pressed
                    if (num == 0) { // switch to entering longitude instead of latitude
                        type = "longitude";
                        num++;
                    }
                    else if (num == 1) { // accept no more text
                        type = "none";
                        foundCoords = true;
                        
                        num++;
                    }
                }
                break;

            case sf::Event::MouseButtonPressed:
                if (event.mouseButton.button == sf::Mouse::Left) { // if left mouse click

                    // if clicked redo button
                    if (event.mouseButton.x >= 223 && event.mouseButton.x <= 223 + 150 && event.mouseButton.y >= 1283 && event.mouseButton.y <= 1283 + 50) { // redo
                        heapButtonClicked = false;
                        hashButtonClicked = false;
                        userLatitude = "";
                        userLongitude = "";
                        enterLat = "";
                        enterLong = "";
                        showResults = false;
                        ufoFly = false;
                        // result ufo and youarehere
                        num = 0;
                        type = "latitude";
                        latText.setString("");
                        longText.setString("");
                        heapTime = 0;
                        hashTime = 0;
                        youarehere.scale(0.01, 0.01);
                        ufo.setScale(sf::Vector2f(0.5, 0.5));
                        ufo.setPosition(1000, 1100);
                        hashButton.setFillColor(Color(179, 243, 186));
                        heapButton.setFillColor(Color(179, 243, 186));
                        

                    }
                    
                    // if clicked heap button
                    if (event.mouseButton.x >= 135 && event.mouseButton.x <= 135 + 150 && event.mouseButton.y >= 1190 && event.mouseButton.y <= 1190 + 75) { // heap
                        // get heap values with user input
                        heapButtonClicked = true;
                        heapButton.setFillColor(Color(149, 192, 150));
                        hashButton.setFillColor(Color(179, 243, 186));
                        cout << "here" << endl;
                        window.draw(hashButton);
                        window.draw(heapButton);

                        window.draw(hashText);
                        window.draw(heapText);

                        loadingText.setPosition(140, 1185);
                        window.draw(loadingText);

                        window.display();
                        
                        // time here
                        auto heapStart = std::chrono::steady_clock::now();
                        
                        Heap heap = heap.make_heap(userCoordinates.first, userCoordinates.second);
                        

                        //prints top data in heap
                        
                        double latitude = heap.peak_top().latitude;
                        double longitude = heap.peak_top().longitude;
                        coords = plotCoords(latitude, longitude, width, height);
                        double distance = heap.peak_top().distance;

                        // get report info
                        distanceText.setString("Distance to UFO: " + to_string(distance) + " km");
                        ufoLatText.setString("Latitude: " + to_string(latitude) + "°");
                        ufoLongText.setString("Longitude: " + to_string(longitude) + "°");
                        cityText.setString("City: " + heap.peak_top().city);
                        stateText.setString("State: " + heap.peak_top().state);
                        countryText.setString("Country: " + heap.peak_top().country);
                        datetimeText.setString("Date & Time: " + heap.peak_top().date_time);
                        linkText.setString("Link: " + heap.peak_top().link);
                        
                        auto heapEnd = std::chrono::steady_clock::now();
                        std::chrono::duration<double> heap_elapsed_seconds = heapEnd - heapStart;
                        heapTime = heap_elapsed_seconds.count();
                        heapTime = ceil(heapTime * 1000.0) / 1000.0;
                        cout << "heap time: " << heapTime << endl;
                        // to here

                        
                        ufoFly = true;
                        
                    }
                    // if clicked hash table button
                    if (event.mouseButton.x >= 315 && event.mouseButton.x <= 315 + 150 && event.mouseButton.y >= 1190 && event.mouseButton.y <= 1190 + 75) { // hash table
                        // get hash values

                        hashButtonClicked = true;
                        hashButton.setFillColor(Color(149, 192, 150));
                        heapButton.setFillColor(Color(179, 243, 186));

                        window.draw(hashButton);
                        window.draw(heapButton);

                        window.draw(hashText);
                        window.draw(heapText);

                        loadingText.setPosition(325, 1180);
                        window.draw(loadingText);

                        window.display();
                        
                        auto hashStart = std::chrono::steady_clock::now();
                        
                        HashTable hashTable = hashTable.make_HashTable(userCoordinates.first, userCoordinates.second);

                        
                        double latitude = hashTable.closestLocation().latitude;
                        double longitude = hashTable.closestLocation().longitude;
                        coords = plotCoords(latitude, longitude, width, height);
                        double distance = hashTable.closestLocation().distance;

                        // get report info
                        distanceText.setString("Distance to UFO: " + to_string(distance) + " km");
                        ufoLatText.setString("Latitude: " + to_string(latitude) + "°");
                        ufoLongText.setString("Longitude: " + to_string(longitude) + "°");
                        cityText.setString("City: " + hashTable.closestLocation().city);
                        stateText.setString("State: " + hashTable.closestLocation().state);
                        countryText.setString("Country: " + hashTable.closestLocation().country);
                        datetimeText.setString("Date & Time: " + hashTable.closestLocation().date_time);
                        linkText.setString("Link: " + hashTable.closestLocation().link);
                        
                        auto hashEnd = std::chrono::steady_clock::now();
                        std::chrono::duration<double> hash_elapsed_seconds = hashEnd - hashStart;
                        hashTime = hash_elapsed_seconds.count();
                        hashTime = ceil(hashTime * 1000.0) / 1000.0;
                        cout << "hash table time: " << hashTime << endl;


                        // get report info
                        /*
                        distanceText.setString("Distance to UFO: " + distance + " km");
                        ufoLatText.setString("Latitude: " + latitude + '\370');
                        ufoLongText.setString("Longitude: " + longitude + '\370');
                        cityText.setString("City: " + city);
                        stateText.setString("State: " + state);
                        countryText.setString("Country: " + country);
                        datetimeText.setString("Date: " + date_time);
                        linkText.setString("Link: " + link);
                        */

                        ufoFly = true;
                    }
                    

                }
                break;

            case sf::Event::TextEntered:
                // accept text input for user latitude & longitude
                if (event.text.unicode == 8) {
                    if (type == "latitude") {
                        if (userLatitude.length() >= 1) {
                            userLatitude = userLatitude.substr(0, userLatitude.length() - 1);
                            latText.setString(userLatitude);
                        }
                    }
                    if (type == "longitude") {
                        if (userLongitude.length() >= 1); {
                            userLongitude = userLongitude.substr(0, userLongitude.length() - 1);
                            longText.setString(userLongitude);
                        }
                    }
                }
                else if (event.text.unicode < 128) {
                    if (type == "latitude") {
                        userLatitude += static_cast<char>(event.text.unicode);
                        latText.setString(userLatitude);
                    }
                    if (type == "longitude") {
                        userLongitude += static_cast<char>(event.text.unicode);
                        longText.setString(userLongitude);
                    }
                    
                }
                break;

            default:
                break;
            }


            // clear the window
            window.clear(sf::Color::Black);

            // if user finishes entering coords
            if (foundCoords == true) {
                
                double yourlatitude;
                stringstream(userLatitude) >> yourlatitude;
                
                
                double yourlongitude;
                stringstream(userLongitude) >> yourlongitude;

                userCoordinates = make_pair(yourlatitude, yourlongitude);
                
                yourcoords = plotCoords(yourlatitude, yourlongitude, width, height);
                
                // adjust you are here sprite
                youarehere.scale(100, 100);
                youarehere.setOrigin(youarehere.getLocalBounds().width / 2, youarehere.getLocalBounds().height);
                youarehere.setPosition(yourcoords.second, yourcoords.first+5);
                window.draw(youarehere);
                
                foundCoords = false;
                
            }
            // move ufo to its coords
            if (ufoFly) {                
                ufo.setScale(sf::Vector2f(0.15, 0.15)); 
                ufo.setPosition(coords.second, coords.first);

                ufoFly = false;
                showResults = true;
            }
            
            // draw and place sprites
            window.draw(space);
            window.draw(sprite);
            window.draw(youarehere);
            window.draw(ufo);
            

            window.draw(rectangle);
            textbox1.setPosition(280, 995);
            window.draw(textbox1);
            textbox2.setPosition(280, 1095);
            window.draw(textbox2);
            latText.setPosition(290, 995);
            window.draw(latText);
            longText.setPosition(290, 1095);
            window.draw(longText);


            longEnter.setPosition(110, 1095);
            window.draw(longEnter);
            latEnter.setPosition(110, 995);
            window.draw(latEnter);

            titleText.setPosition(150, 930);
            window.draw(titleText);

            
            window.draw(hashButton);
            window.draw(heapButton);

            window.draw(hashText);
            window.draw(heapText);

            window.draw(redoButton);
            window.draw(redoText);

            // if results have been caclulated, show results
            if (showResults) {
                window.draw(rectangle2);

                distanceText.setPosition(600, 925);
                //distanceText.setString("Distance: ");
                window.draw(distanceText);

                ufoLatText.setPosition(600, 975);
                //ufoLatText.setString("Latitude: ");
                window.draw(ufoLatText);

                ufoLongText.setPosition(940, 975);
                //ufoLongText.setString("Longitude: ");
                window.draw(ufoLongText);

                cityText.setPosition(600, 1025);
                //cityText.setString("City: ");
                window.draw(cityText);

                stateText.setPosition(600, 1075);
                //stateText.setString("State: ");
                window.draw(stateText);

                countryText.setPosition(600, 1125);
                //countryText.setString("Country: ");
                window.draw(countryText);

                datetimeText.setPosition(600, 1175);
                //datetimeText.setString("Date & Time: ");
                window.draw(datetimeText);

                linkText.setPosition(600, 1225);
                //linkText.setString("Link: ");
                window.draw(linkText);


                if (heapButtonClicked) {
                    timeText.setPosition(600, 1265);
                    timeText.setString("Heap Elapsed Time: " + to_string(heapTime).substr(0, 5) + "s");
                    window.draw(timeText);
                }

                if (hashButtonClicked) {
                    timeText.setPosition(600, 1305);
                    timeText.setString("Hash Table Elapsed Time: " + to_string(hashTime).substr(0, 5) + "s");
                    window.draw(timeText);
                }

                // if heap and hash true
                if (heapButtonClicked && hashButtonClicked) {
                    // if hash is faster
                    if (heapTime > hashTime) {
                        float timeDiff = heapTime - hashTime;
                        timeDiff = ceil(timeDiff * 1000.0) / 1000.0;
                        timeText.setPosition(1080, 1305);
                        timeText.setString("(fastest by " + to_string(timeDiff).substr(0, to_string(timeDiff).length() - 3) + "s)");
                        timeText.setCharacterSize(29);
                        timeText.setStyle(sf::Text::Bold);
                        window.draw(timeText);
                        timeText.setStyle(sf::Text::Regular);
                        timeText.setCharacterSize(32);

                    }
                    // if heap is faster
                    else if (hashTime > heapTime) {
                        float timeDiff = hashTime - heapTime;
                        timeDiff = ceil(timeDiff * 1000.0) / 1000.0;
                        timeText.setPosition(1000, 1265);
                        timeText.setString("(fastest by " + to_string(timeDiff).substr(0, to_string(timeDiff).length() - 3) + "s)");
                        timeText.setCharacterSize(29);
                        timeText.setStyle(sf::Text::Bold);
                        window.draw(timeText);
                        timeText.setStyle(sf::Text::Regular);
                        timeText.setCharacterSize(32);
                    }
                }

            }

            window.display();


        }
    }

    return 0;
}
