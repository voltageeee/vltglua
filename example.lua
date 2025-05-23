// custom syntaxis
l_var longassvariable = 0
g_var var2 = 0

l_func doSomeMath(num1, num2) {
    num1 += num2
    longassvariable++
    num2--

    return num1 * num2
}

l_var new = doSomeMath(longassvariable)

if new == var2 {
    print("something happens here!")
}

// the programm will output
local longassvariable = 0
 var2 = 0

local function doSomeMath(num1, num2) 
    num1  =     num1  +  num2
    longassvariable =     longassvariable + 1
    num2 =     num2 - 1

    return num1 * num2
end

local new = doSomeMath(longassvariable)

if new == var2 then
    print("something happens here!")
end
