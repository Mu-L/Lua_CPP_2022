---
--- Generated by EmmyLua(https://github.com/EmmyLua)
--- Created by jiangpengyong.
--- DateTime: 2022/5/10 21:33
---

function foo(x)
    coroutine.yield(10, x)
end

function foo1(x)
    foo(x + 1)
    return 3
end