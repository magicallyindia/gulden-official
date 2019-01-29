// AUTOGENERATED FILE - DO NOT MODIFY!
// This file generated by Djinni from libunity.djinni

#pragma once

#include <cstdint>
#include <string>
#include <utility>

struct PeerRecord final {
    std::string ip;
    std::string hostname;
    int32_t height;
    int32_t latency;
    std::string userAgent;
    int64_t protocol;

    PeerRecord(std::string ip_,
               std::string hostname_,
               int32_t height_,
               int32_t latency_,
               std::string userAgent_,
               int64_t protocol_)
    : ip(std::move(ip_))
    , hostname(std::move(hostname_))
    , height(std::move(height_))
    , latency(std::move(latency_))
    , userAgent(std::move(userAgent_))
    , protocol(std::move(protocol_))
    {}
};