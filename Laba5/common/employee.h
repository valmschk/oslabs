#pragma once

struct employee {
    int num;
    char name[10];
    double hours;
};

enum class CommandType {
    READ,
    MODIFY,
    QUIT,
    DONE
};

struct ClientRequest {
    CommandType cmd;
    int id;
};