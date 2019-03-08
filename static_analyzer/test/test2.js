//{ expected: {undefined:[i, z], unused:[]} }

function f(x)
{
    var result = 0;

    for(i = 0; i < 10; i++)
    {
        var z = i + (x % 2);
        result = result + z;
    }
    return z;
}