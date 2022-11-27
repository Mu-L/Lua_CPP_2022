//
// Created by 江澎涌 on 2022/5/9.
//

#include "XmlParser.h"
#include "expat.h"
#include "lua.hpp"
#include "../Error.h"
#include <iostream>

typedef struct lxp_userdata {
    XML_Parser parser;
    lua_State *L;
} lxp_userdata;

static void f_StartElement(void *ud, const char *name, const char **atts);

static void f_CharData(void *ud, const char *s, int len);

static void f_EndElement(void *ud, const char *name);

static int lxp_make_parser(lua_State *L) {
    XML_Parser p;

    lxp_userdata *xpu = (lxp_userdata *) lua_newuserdata(L, sizeof(lxp_userdata));

    xpu->parser = nullptr;

    luaL_getmetatable(L, "Expat");
    lua_setmetatable(L, -2);

    p = xpu->parser = XML_ParserCreate(nullptr);
    if (!p) {
        luaL_error(L, "XML_ParserCreate failed.");
    }

    luaL_checktype(L, 1, LUA_TTABLE);
    lua_pushvalue(L, 1);
    lua_setuservalue(L, -2);

    XML_SetUserData(p, xpu);
    XML_SetElementHandler(p, f_StartElement, f_EndElement);
    XML_SetCharacterDataHandler(p, f_CharData);

    return 1;
}

static int lxp_parse(lua_State *L) {
    int status;
    size_t len;
    const char *s;
    lxp_userdata *xpu;

    xpu = (lxp_userdata *) luaL_checkudata(L, 1, "Expat");

    luaL_argcheck(L, xpu->parser != nullptr, 1, "parser is closed");

    s = luaL_optlstring(L, 2, nullptr, &len);
    lua_settop(L, 2);
    lua_getuservalue(L, 1);

    xpu->L = L;

    status = XML_Parse(xpu->parser, s, (int) len, s == nullptr);

    lua_pushboolean(L, status);
    return 1;
}

static void f_CharData(void *ud, const char *s, int len) {
    lxp_userdata *xpu = (lxp_userdata *) ud;
    lua_State *L = xpu->L;

    lua_getfield(L, 3, "CharacterData");

    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    lua_pushvalue(L, 1);
    lua_pushlstring(L, s, len);
    lua_call(L, 2, 0);
}

static void f_EndElement(void *ud, const char *name) {
    lxp_userdata *xpu = (lxp_userdata *) ud;
    lua_State *L = xpu->L;

    lua_getfield(L, 3, "EndElement");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    lua_pushvalue(L, 1);
    lua_pushstring(L, name);
    lua_call(L, 2, 0);
}

static void f_StartElement(void *ud, const char *name, const char **atts) {
    lxp_userdata *xpu = (lxp_userdata *) ud;
    lua_State *L = xpu->L;

    lua_getfield(L, 3, "StartElement");
    if (lua_isnil(L, -1)) {
        lua_pop(L, 1);
        return;
    }

    lua_pushvalue(L, 1);
    lua_pushstring(L, name);

    lua_newtable(L);
    for (; *atts; atts += 2) {
        lua_pushstring(L, *(atts + 1));
        lua_setfield(L, -2, *atts);
    }

    lua_call(L, 3, 0);
}

static int lxp_close(lua_State *L) {
    lxp_userdata *xpu = (lxp_userdata *) luaL_checkudata(L, 1, "Expat");

    if (xpu->parser) {
        XML_ParserFree(xpu->parser);
    }
    xpu->parser = nullptr;
    return 0;
}

static const struct luaL_Reg lxp_meths[] = {
        {"parse", lxp_parse},
        {"close", lxp_close},
        {"__gc", lxp_close},
        {nullptr, nullptr}
};

static const struct luaL_Reg lxp_funcs[] = {
        {"new", lxp_make_parser},
        {nullptr, nullptr}
};

int luaopen_lxp(lua_State *L) {
    luaL_newmetatable(L, "Expat");

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, lxp_meths, 0);

    luaL_newlib(L, lxp_funcs);
    return 1;
}

void xmlParserDemo() {

    lua_State *L = luaL_newstate();
    luaL_openlibs(L);
    luaopen_lxp(L);

    std::string fileName = "/Users/jiangpengyong/Desktop/code/CPP/CPP2022/lua/管理资源/XmlParser.lua";
    if (luaL_loadfile(L, fileName.c_str()) || lua_pcall(L, 0, 0, 0)) {
        error(L, "can't run file. file: %s", lua_tostring(L, -1));
    }

    lua_close(L);

}