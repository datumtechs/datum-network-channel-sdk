#pragma once

#include <string>
using namespace std;
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

/**
 * This class for packing msg_id and real_data, with a total len
 */
class simple_buffer {
 public:
  simple_buffer(const string& id, const char* data, uint64_t length) {
    len_ = sizeof(int32_t) + sizeof(uint8_t) + id.size() + length;
    uint8_t id_len = sizeof(uint8_t) + id.size();
    buf_ = new char[len_];
    memset(buf_, 0, len_);
    memcpy(buf_, (const char*)&len_, sizeof(int32_t));
    memcpy(buf_ + sizeof(int32_t), (const char*)&id_len, sizeof(uint8_t));
    memcpy(buf_ + sizeof(int32_t) + sizeof(uint8_t), (const char*)id.data(), id.size());
    memcpy(buf_ + sizeof(int32_t) + sizeof(uint8_t) + id.size(), data, length);
  }
  ~simple_buffer() {
    delete[] buf_;
  }

 public:
  char* data() {
    return buf_;
  }
  const char* data() const {
    return buf_;
  }
  int32_t len() {
    return len_;
  }

 private:
  int32_t len_ = 0;
  char* buf_ = nullptr;
};
