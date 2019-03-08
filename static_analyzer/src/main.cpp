#include <iostream>
#include <cstring>
#include <sstream>
#include <iterator>
#include <sstream>
#include <fstream>
#include <stack>

#include "rapidjson/document.h"

#include "Lexer.h"
#include "Parser.h"
#include "ASTPrinter.h"
#include "Analyzer.h"
#include "SymbolTableBuilder.h"

std::string loadFile(const std::string& path)
{
    std::ifstream t(path);
    t.seekg(0, std::ios::end);
    size_t size = t.tellg();
    std::string buffer(size, ' ');
    t.seekg(0);
    t.read(&buffer[0], size);
    return buffer;
}

std::vector<std::string> run(const std::string& code)
{
    IPLVector<Token> tokens;
    Tokenize(code.c_str(), tokens);
    auto ast = Parse(tokens);

    std::ostringstream test;
    ASTPrinter p(test);
    ast->Accept(p);

    std::string ast_json = test.str();
    rapidjson::Document d;

    std::cout << ast_json << std::endl;

    d.Parse(ast_json.c_str());

    SymbolTable st;
    SymbolTableBuilder stb;
    stb.buildSymbolTable(d, st);

    Analyzer a;
    return a.analyze(&d, &st);
}

int getSingle(int arr[], int n)
{
    // Initialize result
    int result = 0;

    int x, sum;

    // Iterate through every bit
    for (int i = 0; i < 32; i++)
    {
        // Find sum of set bits at ith position in all
        // array elements
        sum = 0;
        x = (1 << i);
        for (int j=0; j< n; j++ )
        {
            if (arr[j] & x)
                sum++;
        }

        if ((sum % 6) != 0)
        {
            result |= x;
        }
    }
    return result;
}

void swap(int* x, int* y)
{
    int temp = *x;
    *x = *y;
    *y = temp;
}

int partition(int arr[], int start, int end)
{
    int pivotIndex = start;
    int pivotElem = arr[end];

    for(int j = start; j < end; j++)
    {
        if(arr[j] < pivotElem)
        {
            swap(&arr[pivotIndex], &arr[j]);
            pivotIndex++;
        }
    }
    swap(&arr[pivotIndex], &arr[end]);

    return pivotIndex;
}

void quicksort(int* arr, int startIndex, int endIndex)
{
    std::stack<int> stack;
    stack.push(startIndex);
    stack.push(endIndex);

    while(!stack.empty())
    {
        int end = stack.top();
        stack.pop();
        int start = stack.top();
        stack.pop();

        int pivotIndex = partition(arr, start, end);
        int pivotLeft = pivotIndex-1;
        int pivotRight = pivotIndex+1;

        if(pivotLeft > start)
        {
            stack.push(start);
            stack.push(pivotLeft);
        }

        if(pivotRight < end)
        {
            stack.push(pivotRight);
            stack.push(end);
        }
    }
}

void insertionSort(int* arr, int size)
{
    int i, j, value;

    for(i = 1; i < size; i++)
    {
        value = arr[i];
        j = i - 1;

        while(j >= 0 && arr[j] > value)
        {
            arr[j+1] = arr[j];
            j--;
        }
        arr[j+1] = value;
    }
}

int main(int argc, const char* argv[])
{
    int arr[] = {2,2,31,1,2,32,56,56,787,665,4,3,2,25,6};
    int size = sizeof(arr)/sizeof(arr[0]);

    //quicksort(arr, 0, size-1);
    insertionSort(arr, size);

    int x = 1;
//    const char* path = nullptr;
//    if(argc == 2)
//    {
//        std::cout << "Using passed file path.\n";
//        path = argv[--argc];
//    }
//    else
//    {
//        std::cout << "No file path passed. Using default.\n";
//        path = "../test/test2.js";
//    }
//
//    auto source = loadFile(path);
//    std::cout << source << std::endl;
//    run(source);

    return 0;
}
