#include "DataParser.hpp"

DataParser::DataParser() {
  
}

DataParser::~DataParser() {
  if (t.joinable()) {
    t.join();
  }
}

// may need to use move semantics later on to reduce the copies but ill try this for now
void DataParser::pushData(json new_data) {
  data.push(new_data);
}

void DataParser::processLoop() {
  while (true) {
    if (!data.empty()) {
      auto next_item = data.pop();
      DataParser(next_item);
    }
  }
}

void DataParser::parseData(std::shared_ptr<json> item) {
  return;
}
