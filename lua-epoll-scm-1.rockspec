package = "lua-epoll"
version = "scm-1"
source = {
   url = "hg+https://code.zash.se/lua-epoll/"
}
description = {
   summary = "Minimal binding to the Linux epoll API.",
   detailed = [[
This is a minimal binding to the Linux epoll API for the Lua language.
]],
   homepage = "https://code.zash.se/lua-epoll/",
   license = "MIT"
}
dependencies = {}
build = {
   type = "builtin",
   modules = {
      epoll = {
         sources = "lepoll.c"
      }
   }
}
