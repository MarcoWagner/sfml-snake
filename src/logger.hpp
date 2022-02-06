#pragma once

#include <functional>
#include <string_view>

namespace Logger {

void warn(std::string_view msg);

inline void warn(bool assertion, std::string_view msg) {
  if (!assertion) {
    warn(msg);
  }
}

using Callback = std::function<void(std::string_view)>;

void set_warn_handler(Callback callback);

} // namespace Logger
