/*
 UrlParser.h - A wrapper above joyent's url parser
*/
#pragma once

#include <Arduino.h>

struct UrlParserResult {

    String schema;
    String host;
    uint16_t port;
    String path;
    String query;
    String fragment;

    String pathAndQuery();
};

class UrlParser {
public:
    static bool parse( const char* url, UrlParserResult &result );
};
