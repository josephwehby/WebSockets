/*
This will handle all the incoming messages from the websocket
*/

#include <mutex>
#include <stack>
#include <stdexcept>

template <typename T>
class SafeQueue {
  public:
    SafeQueue() {}

    SafeQueue(const SafeQueue& other) {
      std::lock_guard<std::mutex> lock(other.m);
      data = other.data; 
    }

    SafeQueue& operator=(const SafeQueue&) = delete;
    
    bool empty() const {
      std::lock_guard<std::mutex> lock(m);
      return data.empty();
    }

    void push(T value) {
      std::lock_guard<std::mutex> lock(m);
      data.push(std::move(value));
    }

    std::shared_ptr<T> pop() {
      std::lock_guard<std::mutex> lock(m);
      
      if (data.empty()) {
        throw std::runtime_error("Stack is empty");
      }

      std::shared_ptr<T> const res = std::make_shared<T>(data.top());
      data.pop();
      return res;
    }

  private:
    std::stack<T> data;
    mutable std::mutex m;
};
