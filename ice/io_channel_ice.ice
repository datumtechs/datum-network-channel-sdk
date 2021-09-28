#pragma once
module ChannelSdk {
    exception SendDataException
    {
    }

    sequence<byte> bytes;
    interface IoChannel {
       ["amd"] idempotent int send(string nodeid, string msgid, bytes data)
            throws SendDataException;
    };
};