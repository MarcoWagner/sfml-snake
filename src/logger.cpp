#include "logger.hpp"

#include <functional>
#include <iostream>
#include <string_view>

namespace Logger {

static void warn_handler_default(std::string_view msg) {
  std::cerr << msg << "\n";
}

Callback &warn_handler_singleton() {
  static Callback instance = &warn_handler_default;
  return instance;
}

void set_warn_handler(Callback callback) {
  warn_handler_singleton() = callback;
}

void warn(std::string_view msg) { warn_handler_singleton()(msg); }

} // namespace Logger
