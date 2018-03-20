//
//  main.cpp
//  顺序栈
//
//  Created by 江泽群 on 06/05/2017.
//  Copyright © 2017 江泽群. All rights reserved.
//

#include <iostream>
using namespace std;

enum{DEFAULSIZE = 100};

template <typename T>
class Stack {
public:
    Stack(int = DEFAULSIZE);
    ~Stack();
    bool isEmpty() {return base == top;}
    int length() {return int(top - base);}
    void push(T);
    T pop();
private:
    int maxSize;
    T *base;
    T *top;
};

int main(int argc, const char * argv[]) {
    // insert code here...
    Stack<char> stack_1(20);
    for (int i = 65; i < 73; i++)
    {
    stack_1.push(i);
    }
    stack_1.pop();
    stack_1.pop();
    stack_1.push('X');
    stack_1.push('Y');
    cout << "依次输出出栈元素：" <<endl;
    while (!stack_1.isEmpty())
        cout<< stack_1.pop() <<"\t";
    cout << endl;
    return 0;
}

//Stack类的成员函数定义
template <typename T>
Stack<T>::Stack(int m_size):maxSize(m_size)
{
    base = new T(m_size);
    top = base;
}

template <typename T>
Stack<T>::~Stack()
{
    delete base;
    base = NULL;
    top = NULL;
}

template <typename T>
void Stack<T>::push(T it)
{
    if (top - base == maxSize)
    {
        cerr << "Stack overflows!" << endl;
        abort();
    }
    *top++ = it;
}

template <typename T>
T Stack<T>::pop()
{
    if (top == base)
    {
        cerr << "Stack underflows!" << endl;
        abort();
    }
    T it = *(--top);
    return it;
}
