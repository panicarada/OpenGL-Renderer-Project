//
// Created by 邱泽鸿 on 2020/12/23.
//


#include "Light.h"


std::set<int> Light::availableSlots = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // 至多十个灯
bool Light::isInited = false;
