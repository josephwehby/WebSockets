#pragma once

#include <thread>
#include <unordered_map>
#include <nlohmann/json.hpp>
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

    void processLoop();
    void parseData(std::shared_ptr<json>);
};
