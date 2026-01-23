#include "ArduinoLog.hpp"

#ifndef DISABLE_LOGGING
  int Logging::_level = LOG_LEVEL_SILENT;
  Print* Logging::_logOutput = nullptr;
  const char* Logging::_prefixFormat = nullptr;
  int Logging::_digit = 2;
#endif

Logging::Logging(const char* moduleName):
  _currentLevel(LOG_LEVEL_SILENT),
  _moduleName(moduleName)
{}

void Logging::setLevel(int level) {
  #ifndef DISABLE_LOGGING
    _level = level;
  #endif
}

void Logging::setOutput(Print* output) {
  #ifndef DISABLE_LOGGING
    _logOutput = output;
  #endif
}

void Logging::setPrefix(const char* format) {
  #ifndef DISABLE_LOGGING
    _prefixFormat = format;
  #endif
}

void Logging::clearPrefix() {
  #ifndef DISABLE_LOGGING
    _prefixFormat = nullptr;
  #endif
}

void Logging::setDigit(int digit) {
  #ifndef DISABLE_LOGGING
    _digit = digit;
  #endif
}

void Logging::println(const __FlashStringHelper *format, va_list args) {
  #ifndef DISABLE_LOGGING	  	
    PGM_P p = reinterpret_cast<PGM_P>(format);
    // This copy is only necessary on some architectures (x86) to change a passed
    // array in to a va_list.
    #ifdef __x86_64__
      va_list args_copy;
      va_copy(args_copy, args);
    #endif

    char c = pgm_read_byte(p++);
    for(;c != 0; c = pgm_read_byte(p++)) {
      if (c == '%') {
        c = pgm_read_byte(p++);

        #ifdef __x86_64__
          printFormat(c, &args_copy);
        #else
          printFormat(c, &args);
        #endif
      } else {
        _logOutput->print(c);
      }
    }

    _logOutput->println();
    #ifdef __x86_64__
      va_end(args_copy);
    #endif
  #endif
}

void Logging::println(const char *format, va_list args) {
  #ifndef DISABLE_LOGGING	  	
    // This copy is only necessary on some architectures (x86) to change a passed
    // array in to a va_list.
    #ifdef __x86_64__
      va_list args_copy;
      va_copy(args_copy, args);
    #endif

    for (; *format != 0; ++format) {
      if (*format == '%') {
        ++format;
        #ifdef __x86_64__
          printFormat(*format, &args_copy);
        #else
          printFormat(*format, &args);
        #endif
      } else {
        _logOutput->print(*format);
      }
    }

    _logOutput->println();
    #ifdef __x86_64__
      va_end(args_copy);
    #endif
  #endif
}

void Logging::printFormat(const char format, va_list *args) {
  #ifndef DISABLE_LOGGING
    if (format == '\0')
      return;
    if (format == '%') {
      _logOutput->print(format);
    }

    else if (format == 's') {
      char *s = va_arg(*args, char *);
      _logOutput->print(s);
    }
    else if (format == 'S') {
      __FlashStringHelper *s = va_arg(*args, __FlashStringHelper *);
      _logOutput->print(s);
    }

    else if (format == 'd') {
      _logOutput->print(va_arg(*args, int), DEC);
    }
    else if (format == 'f') {
      _logOutput->print(va_arg(*args, double), _digit);
    }

    else if (format == 'x') {
      _logOutput->print(va_arg(*args, int), HEX);
    }
    else if (format == 'X') {
      _logOutput->print("0x");
      //_logOutput->print(va_arg(*args, int), HEX);
      uint16_t h = (uint16_t) va_arg(*args, int);
      if (h < 0xFFF) _logOutput->print('0');
      if (h < 0xFF) _logOutput->print('0');
      if (h < 0xF) _logOutput->print('0');
      _logOutput->print(h, HEX);
    }

    else if (format == 'p') {
      Printable *obj = (Printable *) va_arg(*args, int);
      _logOutput->print(*obj);
    }

    else if (format == 'b') {
      _logOutput->print(va_arg(*args, int), BIN);
    }
    else if (format == 'B') {
      _logOutput->print("0b");
      _logOutput->print(va_arg(*args, int), BIN);
    }

    else if (format == 'l') {
      _logOutput->print(va_arg(*args, long), DEC);
    }
    else if (format == 'u') {
      _logOutput->print(va_arg(*args, unsigned long), DEC);
    }

    else if (format == 'c') {
      _logOutput->print((char) va_arg(*args, int));
    }
    else if (format == 'C') {
      char c = (char) va_arg(*args, int);
      if (c >= 0x20 && c < 0x7F) {
        _logOutput->print(c);
      } else {
        _logOutput->print("0x");
        if (c < 0xF) _logOutput->print('0');
        _logOutput->print(c, HEX);
      }
    }
    
    else if (format == 't') {
      if (va_arg(*args, int) == 1) {
        _logOutput->print("T");
      }
      else {
        _logOutput->print("F");
      }
    }
    else if (format == 'T') {
      if (va_arg(*args, int) == 1) {
        _logOutput->print(F("true"));
      }
      else {
        _logOutput->print(F("false"));
      }
    }
    
    // Internal variables - don't consume va_arg
    else if (format == 'L' || format == 'v' || format == 'n' || 
             format == 'm' || format == 'M' || format == 'r') {
      printInternal(format);
    }
  #endif
}

void Logging::printInternal(char format) {
  #ifndef DISABLE_LOGGING
  if (format == 'L') {
    // Current calling log level abbreviation
    _logOutput->print(getLevelAbbrev(_currentLevel));
  }
  else if (format == 'v') {
    // Configured threshold/verbosity level abbreviation
    _logOutput->print(getLevelAbbrev(_level));
  }
  else if (format == 'n') {
    // Module/class name
    if (_moduleName != NULL) {
      _logOutput->print(_moduleName);
    }
    }
    else if (format == 'm') {
      // Raw millis timestamp
      _logOutput->print(millis());
    }
    else if (format == 'M') {
      // Formatted timestamp (HH:MM:SS.mmm)
      printFormattedTime(millis());
    }
    else if (format == 'r') {
      // Free RAM (platform-specific)
      #if defined(ARDUINO_ARCH_AVR)
      extern int __heap_start, *__brkval;
      int freeRam = (int) &freeRam - (__brkval == 0 ? (int) &__heap_start : (int) __brkval);
      _logOutput->print(freeRam);
      #elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
      char stackVar;
      extern char *_end;
      int freeRam = (int)&stackVar - (int)&_end;
      _logOutput->print(freeRam);
      #elif defined(ESP32)
      _logOutput->print(ESP.getFreeHeap());
      #elif defined(ESP8266)
      _logOutput->print(ESP.getFreeHeap());
      #elif defined(__x86_64__) || defined(__i386__)
      // Native platform - simulate free RAM for testing
      char stackVar;
      static char* heapStart = nullptr;
      if (heapStart == nullptr) heapStart = &stackVar;
      int simulatedFreeRam = (int)(&stackVar - heapStart) + 2048;
      _logOutput->print(simulatedFreeRam);
      #else
      _logOutput->print(F("N/A"));
      #endif
    }
  #endif
}

const char* Logging::getLevelAbbrev(int level) {
  #ifndef DISABLE_LOGGING
    switch(level) {
      case LOG_LEVEL_CRITICAL: return LEVEL_ABBREV_CRITICAL;
      case LOG_LEVEL_ERROR:    return LEVEL_ABBREV_ERROR;
      case LOG_LEVEL_WARNING:  return LEVEL_ABBREV_WARNING;
      case LOG_LEVEL_INFO:     return LEVEL_ABBREV_INFO;
      case LOG_LEVEL_DEBUG:    return LEVEL_ABBREV_DEBUG;
      case LOG_LEVEL_TRACE:    return LEVEL_ABBREV_TRACE;
      default:                 return "UNKN";
    }
  #else
    return "";
  #endif
}

void Logging::printFormattedTime(unsigned long ms) {
  #ifndef DISABLE_LOGGING
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    unsigned long hours = minutes / 60;
    unsigned long remainderMs = ms % 1000;
    unsigned long remainderSec = seconds % 60;
    unsigned long remainderMin = minutes % 60;

    // Print HH:MM:SS.mmm format
    if (hours < 10) _logOutput->print('0');
    _logOutput->print(hours);
    _logOutput->print(':');
    if (remainderMin < 10) _logOutput->print('0');
    _logOutput->print(remainderMin);
    _logOutput->print(':');
    if (remainderSec < 10) _logOutput->print('0');
    _logOutput->print(remainderSec);
    _logOutput->print('.');
    if (remainderMs < 100) _logOutput->print('0');
    if (remainderMs < 10) _logOutput->print('0');
    _logOutput->print(remainderMs);
  #endif
}

void Logging::printPrefixFormat() {
  #ifndef DISABLE_LOGGING
    if (_prefixFormat == nullptr)
      return;

    for (const char* p = _prefixFormat; *p != 0; ++p) {
      if (*p == '%' && *(p + 1) != 0) {
        ++p;
        printInternal(*p);
      } else {
        _logOutput->print(*p);
      }
    }
  #endif
}
