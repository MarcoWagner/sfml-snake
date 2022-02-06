#pragma once

#include "logger.hpp"

#include <chrono>
#include <iostream>

class IntervalRunner {
  using clock = std::chrono::high_resolution_clock;
  using time_point = clock::time_point;

  clock::duration m_interval{};
  time_point m_last_update{};

public:
  class FPS {
    const clock::duration m_interval{};

  public:
    constexpr explicit FPS(int fps)
        : m_interval(clock::duration{std::chrono::seconds{1}} / fps) {}
    constexpr operator clock::duration() const { return m_interval; }
  };

  IntervalRunner(clock::duration interval) : m_interval(interval) {}

  virtual ~IntervalRunner() = default;

  void set_interval(clock::duration interval) { m_interval = interval; }

  clock::duration get_interval() const { return m_interval; }

  void tick() {
    const auto start_time = clock::now();
    const auto past_time = start_time - m_last_update;
    if (past_time < m_interval) {
      return;
    }

    Logger::warn(m_last_update == time_point{} || past_time < m_interval * 2,
                 "running to slow, frames dropped!");

    run_frame();

    m_last_update = start_time;

    Logger::warn(clock::now() - m_last_update < m_interval,
                 "running to slow, frames will probably getting dropped!");
  }

  virtual void run_frame() = 0;
};