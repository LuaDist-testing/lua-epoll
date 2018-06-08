/*
 * Lua binding to epoll
 * Copyright (C) 2016 Kim Alvefur
 *
 * This project is MIT/X11 licensed. Please see the
 * COPYING file in the source package for more information.
 *
 */
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include <lualib.h>
#include <lauxlib.h>

const char *const ctl_op_s[] = {
	"add",
	"mod",
	"del",
	NULL
};

int ctl_op_i[] = {
	EPOLL_CTL_ADD,
	EPOLL_CTL_MOD,
	EPOLL_CTL_DEL,
	-1
};

int lep_ctl(lua_State *L) {
	struct epoll_event event;
	int epfd = lua_tointeger(L, lua_upvalueindex(1));
	int op = ctl_op_i[luaL_checkoption(L, 1, NULL, ctl_op_s)];
	int fd = luaL_checkint(L, 2);
	const char *mode = luaL_optstring(L, 3, "rw");
	event.events = 0;
	event.data.fd = fd;

	if(strchr(mode, 'r') != NULL) {
		event.events |= EPOLLIN;
	}

	if(strchr(mode, 'w') != NULL) {
		event.events |= EPOLLOUT;
	}

	if(epoll_ctl(epfd, op, fd, &event) < 0) {
		lua_pushnil(L);
		lua_pushstring(L, strerror(errno));
		lua_pushinteger(L, errno);
		return 3;
	}

	lua_pushboolean(L, 1);

	return 1;
}

int lep_wait(lua_State *L) {
	struct epoll_event event;
	int epfd = lua_tointeger(L, lua_upvalueindex(1));
	int timeout = 1000 * luaL_optnumber(L, 1, 0);
	int ret = epoll_wait(epfd, &event, 1, timeout);

	if(ret < 1) {
		lua_pushnil(L);

		if(ret == 0) {
			lua_pushstring(L, "timeout");
			lua_pushinteger(L, 0);
		} else {
			lua_pushstring(L, strerror(errno));
			lua_pushinteger(L, errno);
		}

		return 3;
	}

	if(ret == 1) {
		lua_pushinteger(L, event.data.fd);
		lua_pushboolean(L, event.events & EPOLLIN);
		lua_pushboolean(L, event.events & EPOLLOUT);
		return 3;
	}

	return 0;
}

int luaopen_epoll(lua_State *L) {
	lua_createtable(L, 0, 1);
	lua_pushinteger(L, epoll_create1(0));
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, lep_ctl, 1);
	lua_setfield(L, -3, "ctl");
	lua_pushvalue(L, -1);
	lua_pushcclosure(L, lep_wait, 1);
	lua_setfield(L, -3, "wait");
	lua_pop(L, 1);
	return 1;
}


