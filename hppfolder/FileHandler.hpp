#pragma once
#include <fstream>
#include "Struct.hpp"

struct Med;
struct LinkedList;

void loadFromFile(LinkedList &L, const string &username);
void commitToFile(const LinkedList &L, const string &username);
void clearUserData(const string &username, LinkedList &L);
void clearFileData(const string &username);
