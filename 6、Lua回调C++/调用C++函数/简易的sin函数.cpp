//
// Created by 江澎涌 on 2023/10/23.
//

#include "简易的sin函数.h"

int l_sin(lua_State *L) {
    // 获取一个 double 类型
//    double d = lua_tonumber(L, 1);
    double d = luaL_checknumber(L, 1);
    lua_pushnumber(L, sin(d));
    return 1;
}