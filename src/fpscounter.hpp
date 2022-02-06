
#include <chrono>

class FPSCounter {
  using clock = std::chrono::steady_clock;
  using time_point = clock::time_point;

  time_point start_time{};
  int counter_running{};
  int counter_last_second{};

public:
  void tick() {
    const auto current_time = clock::now();
    if (current_time - start_time > std::chrono::seconds{1}) {
      counter_last_second = counter_running;
      counter_running = 0;
      start_time = current_time;
    }
    ++counter_running;
  }

  int get() const { return counter_last_second; }
};
