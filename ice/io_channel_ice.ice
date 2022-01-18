#pragma once
module ChannelSdk {
    exception SendDataException
    {
    }

    sequence<byte> bytes;

    struct DataStruct
    {
        string msgid;
        string nodeid;
        bytes data;
    }

    interface IoChannel {
       ["amd"] idempotent int send(string nodeid, string msgid, bytes data)
            throws SendDataException;
    };
};