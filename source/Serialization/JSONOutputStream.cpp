#include <cstdio>
#include <cstdarg>

#include "JSONOutputStream.h"

#define __STDC_FORMAT_MACROS
#include <inttypes.h>

namespace serialization {

JSONOutputStream& JSONOutputStream::operator<<(bool value) {
    handleNewValue();
    write(value ? "true" : "false");
    commitValue();
    return *this;
}

JSONOutputStream& JSONOutputStream::operator<<(std::int8_t value) {
    return formatValue("%" PRId8, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::uint8_t value) {
    return formatValue("%" PRIu8, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::int16_t value) {
    return formatValue("%" PRId16, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::uint16_t value) {
    return formatValue("%" PRIu16, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::int32_t value) {
    return formatValue("%" PRId32, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::uint32_t value) {
    return formatValue("%" PRIu32, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::int64_t value) {
    return formatValue("%" PRId64, value);
}

JSONOutputStream& JSONOutputStream::operator<<(std::uint64_t value) {
    return formatValue("%" PRIu64, value);
}

JSONOutputStream& JSONOutputStream::operator<<(const char* value) {
    return formatValue("\"%s\"", value);
}

JSONOutputStream& startArray(JSONOutputStream& os) {
    os.put('[');
    return os;
}

JSONOutputStream& endArray(JSONOutputStream& os) {
    os.startNewValue = false;
    os.put(']');
    os.commitValue();
    return os;
}

JSONOutputStream& startObject(JSONOutputStream& os) {
    os.put('{');
    return os;
}

JSONOutputStream& endObject(JSONOutputStream& os) {
    os.startNewValue = false;
    os.put('}');
    os.commitValue();
    return os;
}

JSONOutputStream& nil(JSONOutputStream& os) {
    os.write("null");
    os.commitValue();
    return os;
 }

JSONOutputStream& JSONOutputStream::format(const char *fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    vformat(fmt, args);
    va_end(args);
    return *this;
}

JSONOutputStream& JSONOutputStream::vformat(const char *fmt, std::va_list list) {
    handleNewValue();
    vfprintf(out, fmt, list);
    return *this;
}

void JSONOutputStream::put(char c) {
    handleNewValue();
    fputc(c, out);
}

void JSONOutputStream::write(const char* data, std::size_t count) {
    handleNewValue();
    fwrite(data, 1, count, out);
}

void JSONOutputStream::write(const char* data) {
    handleNewValue();
    fputs(data, out);
}

void JSONOutputStream::flush() {
    fflush(out);
}

void JSONOutputStream::commitValue() {
    startNewValue = true;
}

JSONOutputStream& JSONOutputStream::formatValue(const char *fmt, ...) {
    std::va_list args;
    va_start(args, fmt);
    vformatValue(fmt, args);
    va_end(args);
    return *this;
}

JSONOutputStream& JSONOutputStream::vformatValue(const char *fmt, std::va_list list) {
    vformat(fmt, list);
    commitValue();
    return *this;
}

void JSONOutputStream::handleNewValue() {
    if(startNewValue) {
        fputc(',', out);
        startNewValue = false;
    }
}

JSONOutputStream& operator<<(JSONOutputStream& os, const Key& k) {
    return os.format("\"%s\":", k.str);
}

} // namespace serialization
