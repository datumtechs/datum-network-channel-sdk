// ==============================================================================
// Copyright 2020 The LatticeX Foundation
// This file is part of the Rosetta library.
//
// The Rosetta library is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// The Rosetta library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// along with the Rosetta library. If not, see <http://www.gnu.org/licenses/>.
// ==============================================================================
#if USE_BUFFER
#include "cycle_buffer.h"
#include <cstring>
#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <assert.h>
#include <unistd.h>
using namespace std;

cycle_buffer::~cycle_buffer() {
  delete[] buffer_;
  buffer_ = nullptr;
}

void cycle_buffer::reset() {
  is_full_ = false;
  is_empty_ = true;
  r_pos_ = 0;
  w_pos_ = 0;
  remain_space_ = n_;
}

bool cycle_buffer::can_read(int32_t length) {
  std::unique_lock<std::mutex> lck(mtx_);
  return (n_ - remain_space_ >= length);
}

bool cycle_buffer::can_remove(double t) {
  if (
    (remain_space_ == n_) // no datas
    && (timer_.elapse() > t) // no visits in t seconds
  ) {
    return true;
  }
  return false;
}
/////////////////////////////////////////
int32_t cycle_buffer::peek(char* data, int32_t length) {
  timer_.start();
  {
    unique_lock<mutex> lck(mtx_);
    cv_.wait(lck, [&]() {
      if (n_ - remain_space_ >= length)
        return true;
      cout << "can not peek" << endl;
      return false;
    });
  }
  timer_.start();
  {
    unique_lock<mutex> lck(mtx_);
    if (r_pos_ >= w_pos_) {
      if (r_pos_ <= n_ - length) {
        memcpy(data, buffer_ + r_pos_, length);
      } else {
        int first_n = n_ - r_pos_;
        memcpy(data, buffer_ + r_pos_, first_n);
        memcpy(data + first_n, buffer_, length - first_n);
      }
    } else {
      memcpy(data, buffer_ + r_pos_, length);
    }
    cv_.notify_all();
  }
  timer_.start();
  return length;
}

bool cycle_buffer::can_read() {
  unique_lock<mutex> lck(mtx_);
  //log_info << "can read remain data:" << n_ - remain_space_;
  if (n_ - remain_space_ > sizeof(int32_t) + sizeof(uint8_t)) {
    int len = 0;
    if (r_pos_ < w_pos_) {
      len = *(int32_t*)(buffer_ + r_pos_);
    } else {
      char dlen[sizeof(int32_t)];
      if (r_pos_ <= n_ - sizeof(int32_t)) {
	      memcpy(dlen, buffer_ + r_pos_, sizeof(int32_t));
      } else {
	      memcpy(dlen, buffer_ + r_pos_, n_ - r_pos_);
	      memcpy(dlen + n_ - r_pos_, buffer_, sizeof(int32_t) - (n_ - r_pos_));
      }
        len = *(int32_t*)dlen;
    }
    //log_info << "remain data:" << n_ - remain_space_ << ", data size:" << len;
    if (n_ - remain_space_ >= len)
      return true;
  }
  return false;
}

int32_t cycle_buffer::read(string& id, string& data) {
  if (n_ - remain_space_ > sizeof(int32_t) + sizeof(uint8_t)) {
    unique_lock<mutex> lck(mtx_);
    int len = 0;
    string tmp;
    if (r_pos_ < w_pos_) {
      len = *(int32_t*)(buffer_ + r_pos_);
      if (n_ - remain_space_ >= len) {
	      tmp.resize(len);
	      memcpy(&tmp[0], buffer_ + r_pos_, len);
      }
    } else {
      char data_len[sizeof(int32_t)];
      if (r_pos_ <= n_ - sizeof(int32_t)) {
        memcpy(data_len, buffer_ + r_pos_, sizeof(int32_t));
      } else {
        int remain_len = n_ - r_pos_;
        memcpy(data_len, buffer_ + r_pos_, remain_len);
        memcpy(data_len + remain_len, buffer_, sizeof(int32_t) - remain_len);
      }
      len = *(int32_t*)data_len;
      if (n_ - remain_space_ >= len) {
	      tmp.resize(len);
        if (n_ - r_pos_ >= len) {
          memcpy(&tmp[0], buffer_ + r_pos_, len);
        } else {
	        memcpy(&tmp[0], buffer_ + r_pos_, n_ - r_pos_);
	        memcpy((char*)&tmp[0] + n_ - r_pos_, buffer_, len - (n_ - r_pos_));
        }
      }
    }
    if (tmp.size() > sizeof(int32_t) + sizeof(uint8_t)) {
      uint8_t len2 = *(uint8_t*)((char*)&tmp[0] + sizeof(int32_t));
      id.resize(len2 - sizeof(uint8_t));
      memcpy(&id[0], (char*)&tmp[0] + sizeof(int32_t) + sizeof(uint8_t), id.size());
      data.resize(len - sizeof(int32_t) - len2);
      memcpy(&data[0], (char*)&tmp[0] + sizeof(int32_t) + len2, data.size());
      r_pos_ = (r_pos_ + len) % n_;
      remain_space_ += len;
      return tmp.size();
    }
  }
  return 0;
}

int32_t cycle_buffer::read(char* data, int32_t length) {
  timer_.start();
  {
    do {
      unique_lock<mutex> lck(mtx_);
      cv_.wait_for(lck, std::chrono::milliseconds(1000), [&]() {
        if (n_ - remain_space_ >= length)
          return true;
        return false;
      });
      if (n_ - remain_space_ >= length)
        break;
    } while (true);
  }
  timer_.start();
  {
    unique_lock<mutex> lck(mtx_);
    if (r_pos_ >= w_pos_) {
      if (r_pos_ <= n_ - length) {
        memcpy(data, buffer_ + r_pos_, length);
        r_pos_ += length;
      } else {
        int first_n = n_ - r_pos_;
        memcpy(data, buffer_ + r_pos_, first_n);
        memcpy(data + first_n, buffer_, length - first_n);
        r_pos_ = length - first_n;
      }
    } else {
      memcpy(data, buffer_ + r_pos_, length);
      r_pos_ += length;
    }
    remain_space_ += length;
    cv_.notify_all();
  }
  timer_.start();
  return length;
}

void cycle_buffer::realloc(int32_t length) {
  unique_lock<mutex> lck(mtx_);
  if (remain_space_ >= length) {
    return;
  }

  if (remain_space_ < length) {
    int32_t new_n = n_ * ((length / n_) + 2); // at least 2x
    // cout << "buffer can not write. expected:" << length << ", actual:" << remain_space_
    //           << ". will expand from " << n_ << " to " << new_n << endl;

    char* newbuffer_ = new char[new_n];
    int32_t havesize = size();
    if (w_pos_ > r_pos_) {
      memcpy(newbuffer_, buffer_ + r_pos_, havesize);
    } else {
      int first_n = n_ - r_pos_;
      memcpy(newbuffer_, buffer_ + r_pos_, first_n);
      if (havesize - first_n > 0) {
        memcpy(newbuffer_ + first_n, buffer_, havesize - first_n);
      }
    }
    n_ = new_n;
    remain_space_ = n_ - havesize;
    r_pos_ = 0;
    w_pos_ = havesize;
    delete[] buffer_;
    buffer_ = newbuffer_;
    newbuffer_ = nullptr;
  }
}

// data --> buffer_
int32_t cycle_buffer::write(const char* data, int32_t length) {
  timer_.start();
  {
    realloc(length);
    unique_lock<mutex> lck(mtx_);
    cv_.wait(lck, [&]() {
      if (remain_space_ >= length)
        return true;
      cout << "never enter here. can not write" << endl;
      return false;
    });
  }
  timer_.start();
  {
    unique_lock<mutex> lck(mtx_);
    if (w_pos_ >= r_pos_) {
      if (w_pos_ <= n_ - length) {
        memcpy(buffer_ + w_pos_, data, length);
        w_pos_ += length;
      } else {
        int first_n = n_ - w_pos_;
        memcpy(buffer_ + w_pos_, data, first_n);
        memcpy(buffer_, data + first_n, length - first_n);
        w_pos_ = length - first_n;
      }
    } else {
      memcpy(buffer_ + w_pos_, data, length);
      w_pos_ += length;
    }
    remain_space_ -= length;
    //log_info << "write remain data:" << n_ - remain_space_;
    cv_.notify_all();
  }
  timer_.start();
  return length;
}
#endif // USE_BUFFER
