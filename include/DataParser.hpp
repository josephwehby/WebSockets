#include <thread>
#include <unordered_map>
#include "Utils.hpp"

class DataParser {
  public:
    DataParser();
    void pushData(); 
  private:
    void processLoop();
    void parseData();
};
