---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by jiangpengyong.
--- DateTime: 2022/5/1 20:22
---

co = coroutine.wrap(function()
    print("Lua 江澎涌3")
    print("pcall ",pcall(coroutine.yield))
    print("Lua 江澎涌5")
end)

print("Lua 江澎涌1")

co()

print("Lua 江澎涌2")

co()

print("Lua 江澎涌6")