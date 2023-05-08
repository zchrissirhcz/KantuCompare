// based on https://github.com/junluan/shadow/blob/master/shadow/util/log.hpp
#ifndef SHADOW_UTIL_LOG_HPP_
#define SHADOW_UTIL_LOG_HPP_

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#if defined(__linux__) || defined(__APPLE__)
#include <cxxabi.h>
#if defined(__ANDROID__) || defined(ANDROID)
#include <android/log.h>
#else
#include <execinfo.h>
#endif
#endif

namespace Shadow {

#define SLOG_INFO LogMessage("INFO", __FILE__, __LINE__)
#define SLOG_WARNING LogMessage("WARNING", __FILE__, __LINE__)
#define SLOG_ERROR LogMessage("ERROR", __FILE__, __LINE__)
#define SLOG_FATAL LogMessage("FATAL", __FILE__, __LINE__)

#define LOG(severity) SLOG_##severity.stream()
#define LOG_IF(severity, condition) \
  if ((condition)) LOG(severity)

#define CHECK_NOTNULL(condition) \
  if ((condition) == nullptr)    \
  LOG(FATAL) << "Check Failed: '" #condition << "' Must be non NULL "

#define CHECK(condition) \
  if (!(condition)) LOG(FATAL) << "Check Failed: " #condition << " "

#define CHECK_OP(condition, val1, val2)                                        \
  if (!((val1)condition(val2)))                                                \
  LOG(FATAL) << "Check Failed: " #val1 " " #condition " " #val2 " (" << (val1) \
             << " vs " << (val2) << ") "

#define CHECK_EQ(val1, val2) CHECK_OP(==, val1, val2)
#define CHECK_NE(val1, val2) CHECK_OP(!=, val1, val2)
#define CHECK_LE(val1, val2) CHECK_OP(<=, val1, val2)
#define CHECK_LT(val1, val2) CHECK_OP(<, val1, val2)
#define CHECK_GE(val1, val2) CHECK_OP(>=, val1, val2)
#define CHECK_GT(val1, val2) CHECK_OP(>, val1, val2)

#if defined(NDEBUG)
#define DLOG(severity) \
  while (false) LOG(severity)
#define DLOG_IF(severity, condition) \
  while (false) LOG_IF(severity, condition)

#define DCHECK(condition) \
  while (false) CHECK(condition)
#define DCHECK_EQ(val1, val2) \
  while (false) CHECK_EQ(val1, val2)
#define DCHECK_NE(val1, val2) \
  while (false) CHECK_NE(val1, val2)
#define DCHECK_LE(val1, val2) \
  while (false) CHECK_LE(val1, val2)
#define DCHECK_LT(val1, val2) \
  while (false) CHECK_LT(val1, val2)
#define DCHECK_GE(val1, val2) \
  while (false) CHECK_GE(val1, val2)
#define DCHECK_GT(val1, val2) \
  while (false) CHECK_GT(val1, val2)

#else
#define DLOG(severity) LOG(severity)
#define DLOG_IF(severity, condition) LOG_IF(severity, condition)

#define DCHECK(condition) CHECK(condition)
#define DCHECK_EQ(val1, val2) CHECK_EQ(val1, val2)
#define DCHECK_NE(val1, val2) CHECK_NE(val1, val2)
#define DCHECK_LE(val1, val2) CHECK_LE(val1, val2)
#define DCHECK_LT(val1, val2) CHECK_LT(val1, val2)
#define DCHECK_GE(val1, val2) CHECK_GE(val1, val2)
#define DCHECK_GT(val1, val2) CHECK_GT(val1, val2)
#endif

class LogMessage {
 public:
  LogMessage(const std::string& severity, const char* file, int line)
      : severity_(severity) {
    std::string file_str(file);
    const auto& file_name = file_str.substr(file_str.find_last_of("/\\") + 1);
    stream_ << severity << ": " << file_name << ":" << line << "] ";
  }
  ~LogMessage() noexcept(false) {
    const auto& msg = stream_.str();
#if defined(__ANDROID__) || defined(ANDROID)
    if (severity_ == "INFO") {
      __android_log_print(ANDROID_LOG_INFO, "native", "%s\n", msg.c_str());
    } else if (severity_ == "WARNING") {
      __android_log_print(ANDROID_LOG_WARN, "native", "%s\n", msg.c_str());
    } else if (severity_ == "ERROR") {
      __android_log_print(ANDROID_LOG_ERROR, "native", "%s\n", msg.c_str());
    } else if (severity_ == "FATAL") {
      __android_log_print(ANDROID_LOG_FATAL, "native", "%s\n", msg.c_str());
    }
#else
    if (severity_ == "INFO") {
      std::cout << msg << std::endl;
    } else {
      std::cerr << msg << std::endl;
    }
#endif
    if (severity_ == "FATAL") {
      const auto& trace = stack_trace(0);
      if (!trace.empty()) {
        std::cerr << trace;
      }
      throw std::runtime_error(msg);
    }
  }

  std::stringstream& stream() { return stream_; }

  LogMessage(const LogMessage&) = delete;
  void operator=(const LogMessage&) = delete;

 private:
  static std::string demangle(const char* symbol) {
    std::string symbol_str(symbol);
#if defined(__linux__) || defined(__APPLE__)
    auto func_start = symbol_str.find("_Z");
    if (func_start != std::string::npos) {
      auto func_end = symbol_str.find_first_of(" +", func_start);
      const auto& func_symbol =
          symbol_str.substr(func_start, func_end - func_start);
      int status = 0;
      size_t length = 0;
      auto demangle_func_symbol =
          abi::__cxa_demangle(func_symbol.c_str(), nullptr, &length, &status);
      if (demangle_func_symbol != nullptr && status == 0 && length > 0) {
        symbol_str = symbol_str.substr(0, func_start) + demangle_func_symbol +
                     symbol_str.substr(func_end);
      }
      free(demangle_func_symbol);
    }
#endif
    return symbol_str;
  }

  static std::string stack_trace(int start_frame, int stack_size = 20) {
    std::stringstream ss;
#if defined(__linux__) || defined(__APPLE__)
#if !defined(__ANDROID__) && !defined(ANDROID)
    std::vector<void*> stack(stack_size, nullptr);
    stack_size = backtrace(stack.data(), stack_size);
    ss << "Stack trace:" << std::endl;
    auto symbols = backtrace_symbols(stack.data(), stack_size);
    if (symbols != nullptr) {
      for (int n = start_frame; n < stack_size; ++n) {
        const auto& demangle_symbol = demangle(symbols[n]);
        if (demangle_symbol.find("LogMessage") == std::string::npos) {
          ss << "  [bt] " << demangle_symbol << std::endl;
        }
      }
    }
    free(symbols);
#endif
#endif
    return ss.str();
  }

  std::string severity_;
  std::stringstream stream_;
};

}  // namespace Shadow

#endif  // SHADOW_UTIL_LOG_HPP_
