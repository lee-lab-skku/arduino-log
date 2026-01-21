#include "ArduinoLog.hpp"
#include <Arduino.h>
#include <bitset>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdio.h>
#include <unity.h>

using namespace fakeit;
std::stringstream output_;
::Logging Log;
void reset_output() {
  output_.str(std::string());
  output_.clear();
}

std::string decimal_to_binary(int n) {

  std::string binary;
  while (n != 0) {
    binary = (n % 2 == 0 ? "0" : "1") + binary;
    n /= 2;
  }
  return binary;
}

void TEST_ASSERT_EQUAL_STRING_STREAM(std::stringstream const &expected_output,
                                     std::stringstream const &actual_output) {

  TEST_ASSERT_EQUAL_STRING(expected_output.str().c_str(),
                           actual_output.str().c_str());
}

void set_up_logging_captures() {
  When(OverloadedMethod(ArduinoFake(Serial), println, size_t(void)))
      .AlwaysDo([&]() -> int {
        output_ << "\n";
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(char)))
      .AlwaysDo([&](const char x) -> int {
        output_ << x;
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const char[])))
      .AlwaysDo([&](const char x[]) -> int {
        output_ << x;
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(const String &)))
      .AlwaysDo([&](const String &x) -> int {
        output_ << x.c_str();
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned char, int)))
      .AlwaysDo([&](unsigned char x, int y) -> int {
        if (y == 2) {
          output_ << decimal_to_binary(x);
        } else {
          output_ << std::setbase(y) << (unsigned long)x;
        }
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int)))
      .AlwaysDo([&](unsigned long x, int y) -> int {
        output_ << std::fixed << std::setprecision(y) << x;
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(int, int)))
      .AlwaysDo([&](int x, int y) -> int {
        if (y == 2) {
          output_ << decimal_to_binary(x);
        } else {
          output_ << std::setbase(y) << x;
        }
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned int, int)))
      .AlwaysDo([&](unsigned int x, int y) -> int {
        if (y == 2) {
          output_ << decimal_to_binary(x);
        } else {
          output_ << std::setbase(y) << x;
        }
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(double, int)))
      .AlwaysDo([&](double x, int y) -> int {
        output_ << std::fixed << std::setprecision(y) << x;
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(long, int)))
      .AlwaysDo([&](long x, int y) -> int {
        if (y == 2) {
          output_ << std::bitset<sizeof(long)>(x).to_string();
        } else {
          output_ << std::setbase(y) << x;
        }
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print, size_t(unsigned long, int)))
      .AlwaysDo([&](unsigned long x, int y) -> int {
        if (y == 2) {
          output_ << std::bitset<sizeof(double)>(x).to_string();
        } else {
          output_ << std::setbase(y) << x;
        }
        return 1;
      });
  When(OverloadedMethod(ArduinoFake(Serial), print,
                        size_t(const __FlashStringHelper *ifsh)))
      .AlwaysDo([&](const __FlashStringHelper *x) -> int {
        auto message = reinterpret_cast<const char *>(x);
        output_ << message;
        return 1;
      });

  When(Method(ArduinoFake(Serial), flush)).AlwaysReturn();
}

void setUp(void) {
  ArduinoFakeReset();
  set_up_logging_captures();
  Logging::setLevel(LOG_LEVEL_TRACE);
  Logging::setOutput(&Serial);
}
void test_int_values() {
  reset_output();
  int int_value1 = 173;
  int int_value2 = 65536;
  Log.info("Log as Info with integer values : %d, %d", int_value1,
             int_value2);
  std::stringstream expected_output;
  expected_output << "Log as Info with integer values : 173, 65536\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_int_hex_values() {
  reset_output();
  int int_value1 = 152;
  int int_value2 = 65010;
  Log.info(F("Log as Info with hex values     : %x, %X"), int_value1,
             int_value1);
  Log.info("Log as Info with hex values     : %x, %X", int_value2,
             int_value2);
  std::stringstream expected_output;
  expected_output << "Log as Info with hex values     : 98, 0x0098\n"
                  << "Log as Info with hex values     : fdf2, 0xfdf2\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_int_binary_values() {
  reset_output();
  int int_value1 = 2218;
  int int_value2 = 17814;
  Log.info(F("Log as Info with binary values  : %b, %B"), int_value1,
             int_value2);
  std::stringstream expected_output;
  expected_output << "Log as Info with binary values  : 100010101010, "
                     "0b100010110010110\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}
void test_long_values() {
  reset_output();
  long long_value1 = 34359738368;
  long long_value2 = 274877906944;
  Log.info(F("Log as Info with long values    : %l, %l"), long_value1,
             long_value2);
  std::stringstream expected_output;
  expected_output << "Log as Info with long values    : "
                     "34359738368, 274877906944\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_bool_values() {
  reset_output();
  bool true_value = true;
  bool false_value = false;
  Log.info("Log as Info with bool values    : %t, %T", true_value,
             true_value);
  Log.info("Log as Info with bool values    : %t, %T", false_value,
             false_value);
  std::stringstream expected_output;
  expected_output << "Log as Info with bool values    : T, true\n";
  expected_output << "Log as Info with bool values    : F, false\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_char_string_values() {
  reset_output();
  const char *charArray = "this is a string";
  Log.info(F("Log as Info with string value   : %s"), charArray);
  std::stringstream expected_output;
  expected_output << "Log as Info with string value   : this is a string\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_flash_string_values() {
  reset_output();
  //__FlashStringHelper cannot be declared outside a function
  const __FlashStringHelper *flashCharArray2 = F("this is a string");

  const char flashCharArray1[] PROGMEM = "this is a string";
  Log.info("Log as Info with Flash string value   : %S", flashCharArray1);
  std::stringstream expected_output;
  expected_output
      << "Log as Info with Flash string value   : this is a string\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_string_values() {
  reset_output();
  String stringValue1 = "this is a string";
  Log.info("Log as Info with string value   : %s", stringValue1.c_str());
  std::stringstream expected_output;
  expected_output << "Log as Info with string value   : this is a string\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_float_values() {
  reset_output();
  float float_value = 12.34;
  Log.info(F("Log as Info with float value   : %F"), float_value);
  Log.info("Log as Info with float value   : %F", float_value);
  std::stringstream expected_output;
  expected_output << "Log as Info with float value   : 12.34\n"
                  << "Log as Info with float value   : 12.34\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_double_values() {
  reset_output();
  double double_value = 1234.56789;
  // Log.info(F("%D"), double_value);
  Log.info(F("Log as Info with double value   : %D"), double_value);
  Log.info("Log as Info with double value   : %D", double_value);
  std::stringstream expected_output;
  expected_output << "Log as Info with double value   : 1234.57\n"
                  << "Log as Info with double value   : 1234.57\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_mixed_values() {
  reset_output();
  int int_value1 = 15826;
  int int_value2 = 31477;
  long long_value1 = 274877906944;
  long long_value2 = 68719476743;
  bool true_value = true;
  bool false_value = false;
  Log.info(F("Log as Debug with mixed values  : %d, %d, %l, %l, %t, %T"),
             int_value1, int_value2, long_value1, long_value2, true_value,
             false_value);
  std::stringstream expected_output;
  expected_output
      << "Log as Debug with mixed values  : 15826, 31477, 274877906944, "
         "68719476743, T, false\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_log_levels() {
  reset_output();
  bool true_value = true;
  bool false_value = false;
  Log.trace("Log as Trace with bool value    : %T", true_value);
  Log.warning("Log as Warning with bool value  : %T", false_value);
  Log.error("Log as Error with bool value    : %T", true_value);
  Log.critical("Log as Critical with bool value    : %T", false_value);
  Log.debug(F("Log as Debug with bool value   : %T"), true_value);
  std::stringstream expected_output;
  expected_output << "Log as Trace with bool value    : true\n"
                     "Log as Warning with bool value  : false\n"
                     "Log as Error with bool value    : true\n"
                     "Log as Critical with bool value    : false\n"
                     "Log as Debug with bool value   : true\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void printTimestamp(Print *_logOutput) {
  char c[12];
  int m = sprintf(c, "%10lu ", millis());
  _logOutput->print(c);
}

void test_prefix() {
  reset_output();
  When(Method(ArduinoFake(), millis)).Return(1026);
  Log.setPrefix(printTimestamp); // set timestamp as prefix
  Log.info(F("Log with prefix"));
  Log.clearPrefix(); // clear prefix
  std::stringstream expected_output;
  expected_output << "      1026 Log with prefix\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_log_level() {
  reset_output();
  Log.info("Level: %L");
  Log.debug("Level: %L");
  Log.warning("Level: %L");
  Log.error("Level: %L");
  Log.critical("Level: %L");
  Log.trace("Level: %L");
  std::stringstream expected_output;
  expected_output << "Level: INFO\n"
                     "Level: DBUG\n"
                     "Level: WARN\n"
                     "Level: ERRO\n"
                     "Level: CRIT\n"
                     "Level: TRCE\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_threshold_level() {
  reset_output();
  Log.info("Threshold: %v");
  std::stringstream expected_output;
  expected_output << "Threshold: TRCE\n"; // Log is configured with LOG_LEVEL_TRACE
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_module_name() {
  reset_output();
  Logging::setLevel(LOG_LEVEL_INFO);
  ::Logging ModuleLog("TestModule");
  ModuleLog.info("Module: %n");
  // Debug is below INFO threshold, so it won't print
  std::stringstream expected_output;
  expected_output << "Module: TestModule\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_millis() {
  reset_output();
  When(Method(ArduinoFake(), millis)).Return(12345);
  Log.info("Timestamp: %m ms");
  std::stringstream expected_output;
  expected_output << "Timestamp: 12345 ms\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_formatted_time() {
  reset_output();
  When(Method(ArduinoFake(), millis)).Return(3723456); // 1:02:03.456
  Log.info("Time: %M");
  std::stringstream expected_output;
  expected_output << "Time: 01:02:03.456\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_formatted_time_edge_cases() {
  reset_output();
  When(Method(ArduinoFake(), millis)).Return(0);
  Log.info("Time: %M");
  When(Method(ArduinoFake(), millis)).Return(9); // 00:00:00.009
  Log.info("Time: %M");
  When(Method(ArduinoFake(), millis)).Return(99); // 00:00:00.099
  Log.info("Time: %M");
  When(Method(ArduinoFake(), millis)).Return(36005001); // 10:00:05.001
  Log.info("Time: %M");
  std::stringstream expected_output;
  expected_output << "Time: 00:00:00.000\n"
                     "Time: 00:00:00.009\n"
                     "Time: 00:00:00.099\n"
                     "Time: 10:00:05.001\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

void test_internal_free_ram() {
  reset_output();
  Log.info("RAM: %r bytes");
  // On native platform, this should print a simulated value, not N/A
  std::string output_str = output_.str();
  // Just verify it contains "RAM: " and "bytes" and a number
  TEST_ASSERT_TRUE(output_str.find("RAM: ") != std::string::npos);
  TEST_ASSERT_TRUE(output_str.find(" bytes\n") != std::string::npos);
  // Should not be N/A on native platform with our implementation
  TEST_ASSERT_TRUE(output_str.find("N/A") == std::string::npos);
}

void test_combined_internal_variables() {
  reset_output();
  When(Method(ArduinoFake(), millis)).Return(5432);
  Log.info("[%L] Time=%m, Formatted=%M");
  Log.warning("Level: %L, Threshold: %v");
  std::stringstream expected_output;
  expected_output << "[INFO] Time=5432, Formatted=00:00:05.432\n"
                     "[WARN] Level: WARN, Threshold: TRCE\n";
  TEST_ASSERT_EQUAL_STRING_STREAM(expected_output, output_);
}

int main(int argc, char **argv) {
  UNITY_BEGIN();
  RUN_TEST(test_int_values);
  RUN_TEST(test_int_hex_values);
  RUN_TEST(test_int_binary_values);
  RUN_TEST(test_long_values);
  RUN_TEST(test_bool_values);
  RUN_TEST(test_char_string_values);
  RUN_TEST(test_flash_string_values);
  RUN_TEST(test_string_values);
  RUN_TEST(test_float_values);
  RUN_TEST(test_double_values);
  RUN_TEST(test_mixed_values);
  RUN_TEST(test_log_levels);
  RUN_TEST(test_prefix);
  RUN_TEST(test_internal_log_level);
  RUN_TEST(test_internal_threshold_level);
  RUN_TEST(test_internal_module_name);
  RUN_TEST(test_internal_millis);
  RUN_TEST(test_internal_formatted_time);
  RUN_TEST(test_internal_formatted_time_edge_cases);
  RUN_TEST(test_internal_free_ram);
  // RUN_TEST(test_combined_internal_variables);
  UNITY_END();
}
