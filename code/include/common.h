#pragma once
#include "config.h"
using namespace std;

void CopyNodeInfo(NodeInfo& node_info, const Node& nodeInfo) 
{
  node_info.id = nodeInfo.NODE_ID;
  node_info.address = nodeInfo.ADDRESS;
}

static int get_hex_index(char c) 
{
  if (c >= '0' && c <= '9') {
    return c - '0';
  } else if (c >= 'A' && c <= 'Z') {
    return 10 + c - 'A';
  } else if (c >= 'a' && c <= 'z') {
    return 10 + c - 'a';
  }
  return 0;
}

static char get_char(char c1, char c2) 
{
  return get_hex_index(c1) << 4 | get_hex_index(c2);
}

static string get_binary_string(const string& str) 
{
  string ret;
  ret.resize(str.size() / 2);
  for (int i = 0; i < str.size(); i += 2) {
    ret[i / 2] = get_char(str[i], str[i + 1]);
  }
  return ret;
}

bool isNodeType(const vector<NODE_TYPE>& vec_node_types, const NODE_TYPE nodeType)
{
  for(int i=0; i < vec_node_types.size(); ++i)
  {
    if(vec_node_types[i] == nodeType)
    {
      return true;
    }
  }
  return false;
}
