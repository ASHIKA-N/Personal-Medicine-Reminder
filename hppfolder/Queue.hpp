#pragma once
#include "DS.hpp"
#include <iostream>

int getTodayDayNumber();
Queue buildTodayQueue(const LinkedList &L);
void reminderCheck(Queue &q, LinkedList &L, const std::string &username, int remindQty);