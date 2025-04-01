#pragma once

#include <iostream>
#include <thread>
#include <unordered_map>
#include <nlohmann/json.hpp>

#include <fstream>

#include "SafeQueue.hpp"

using json = nlohmann::json;

class DataParser {
  public:
    DataParser();
    ~DataParser();
    void pushData(json); 
  private:
    std::thread t; 
    SafeQueue<json> data;
    unordered_map<std::string, SafeQueue> ticker;   
    unordered_map<std::string, SafeQueue> book;   
    unordered_map<std::string, SafeQueue> ohlc;   
    unordered_map<std::string, SafeQueue> trade;   

    void processLoop();
    void parseData(std::shared_ptr<json>);
};
