//{ expected: {undefined:[y],unused:[x]} }
var x;

function f(x)
{
    var z;
    {
        return x+y+z;
    }
}