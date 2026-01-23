#pragma once
#include "Arduino.h"

#include <inttypes.h>
#include <stdarg.h>

// *************************************************************************
//  Uncomment line below to fully disable logging, and reduce project size
// ************************************************************************
//#define DISABLE_LOGGING

#define LOG_LEVEL_SILENT   0
#define LOG_LEVEL_CRITICAL 1
#define LOG_LEVEL_ERROR    2
#define LOG_LEVEL_WARNING  3
#define LOG_LEVEL_INFO     4
#define LOG_LEVEL_DEBUG    5
#define LOG_LEVEL_TRACE    6

#define LEVEL_ABBREV_CRITICAL "CRIT"
#define LEVEL_ABBREV_ERROR    "ERRO"
#define LEVEL_ABBREV_WARNING  "WARN"
#define LEVEL_ABBREV_INFO     "INFO"
#define LEVEL_ABBREV_DEBUG    "DBUG"
#define LEVEL_ABBREV_TRACE    "TRCE"

/**
 * ArduinoLog is a minimalistic framework to help the programmer output log statements to an output of choice, 
 * fashioned after extensive logging libraries such as log4cpp ,log4j and log4net. In case of problems with an
 * application, it is helpful to enable logging so that the problem can be located. ArduinoLog is designed so 
 * that log statements can remain in the code with minimal performance cost. In order to facilitate this the 
 * loglevel can be adjusted, and (if your code is completely tested) all logging code can be compiled out.
 * 
 * ---- Wildcards
 * 
 * %s	display as string (char*)
 * %S    display as string from flash memory (__FlashStringHelper* or char[] PROGMEM)
 * %c	display as single character
 * %C    display as single character or as hexadecimal value (prefixed by `0x`) if not a printable character
 * %d	display as integer value
 * %l	display as long value
 * %u	display as unsigned long value
 * %x	display as hexadecimal value
 * %X	display as hexadecimal value prefixed by `0x` and leading zeros
 * %b	display as binary number
 * %B	display as binary number, prefixed by `0b`
 * %t	display as boolean value "t" or "f"
 * %T	display as boolean value "true" or "false"
 * %D,%F display as double value
 * %p    display a  printable object 
 * 
 * ---- Internal Variables (auto-injected, don't consume arguments)
 * 
 * %L    current log level abbreviation (CRIT, ERRO, WARN, INFO, DBUG, TRCE)
 * %v    configured threshold/verbosity level abbreviation
 * %n    module/class name (set via constructor)
 * %m    raw timestamp in milliseconds since boot
 * %M    formatted timestamp (HH:MM:SS.mmm)
 * %r    free RAM in bytes (AVR, SAMD, SAM, ESP32, ESP8266)
 * 
 */

class Logging {
  public:
    explicit Logging(const char* moduleName = nullptr);
    ~Logging() = default;

    static void setLevel(int level);
    static void setOutput(Print* output);
    static void setPrefix(const char* format);
    static void clearPrefix();
    static void setDigit(int digit);

    template <class T, typename... Args> void critical(T msg, Args... args) {
      #ifndef DISABLE_LOGGING
        printLevel(LOG_LEVEL_CRITICAL, msg, args...);
      #endif
    }

    template <class T, typename... Args> void error(T msg, Args... args) {
      #ifndef DISABLE_LOGGING
        printLevel(LOG_LEVEL_ERROR, msg, args...);
      #endif
    }

    template <class T, typename... Args> void warning(T msg, Args...args) {
      #ifndef DISABLE_LOGGING
        printLevel(LOG_LEVEL_WARNING, msg, args...);
      #endif
    }

    template <class T, typename... Args> void info(T msg, Args...args) {
      #ifndef DISABLE_LOGGING
        printLevel(LOG_LEVEL_INFO, msg, args...);
      #endif
    }

    template <class T, typename... Args> void debug(T msg, Args... args) {
      #ifndef DISABLE_LOGGING
        printLevel(LOG_LEVEL_DEBUG, msg, args...);
      #endif
    }

    template <class T, typename... Args> void trace(T msg, Args... args) {
      #ifndef DISABLE_LOGGING
        printLevel(LOG_LEVEL_TRACE, msg, args...);
      #endif
    }

  private:
    void println(const char *format, va_list args);

    void println(const __FlashStringHelper *format, va_list args);

    void println(const Printable& obj, va_list args) {
      #ifndef DISABLE_LOGGING
        _logOutput->println(obj);
      #endif
    }

  protected:
    void printFormat(const char format, va_list *args);

    const char* getLevelAbbrev(int level);

  public:
    /**
     * Print internal format identifier (non-consuming format codes)
     * Useful for custom prefix implementations
     * Supports: %L, %v, %n
     */
    void printInternal(char format);

  private:
    void printPrefixFormat();

  private:
    template <class T> void printLevel(int level, T msg, ...) {
      #ifndef DISABLE_LOGGING
        if (level > _level)
          return;                    

        _currentLevel = level;

        if (_prefixFormat != nullptr) {
          printPrefixFormat();
        }

        va_list args;
        va_start(args, msg);
        println(msg, args);
        va_end(args);
      #endif
    }

    #ifndef DISABLE_LOGGING
      static int _level;
      int _currentLevel;
      static Print* _logOutput;
      const char* _moduleName;
      static const char* _prefixFormat;
      static int _digit;
    #endif
};
