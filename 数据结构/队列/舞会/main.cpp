//
//  main.cpp
//  汽车过渡口（队列）
//
//  Created by 江泽群 on 10/05/2017.
//  Copyright © 2017 江泽群. All rights reserved.
//

#include <iostream>
using namespace std;

//设置性别
enum sex{man, lady};

//Person类为舞会嘉宾,包含名字
class person
{
public:
    person(const char* newName, sex newSex);
    ~person();
    //打印person对象姓名
    void print ()
    {
        if (personSex == man) cout<<"Gentleman ";
        else cout << "Lady ";
        cout<<name;
    }
    char* name;
    sex personSex;
    person *next;
};

//sequence类为队列，包含两个person指针类型，分别指向队头与队尾
class sequence
{
public:
    sequence(const char* newName,sex newSex);
    ~sequence(){}
    //生成新的person对象并加入队列
    void append(const char* newName,sex newSex);
    //队头离开队列，并返回该对象
    person& leave();
    //判断队列式是否为空
    bool isEmpty();
private:
    person *front;
    person *rear;
};

//match函数对两个队列从领队开始依次进行匹配
void match(sequence&,sequence&);

int main()
{
    //初始化男士队列
    sequence Gentlemans("Leaderman", man);
    Gentlemans.append("Romeo", man);
    Gentlemans.append("Darcy", man);
    Gentlemans.append("Adam", man);
    Gentlemans.append("Sherlock", man);
    Gentlemans.append("John", man);
    sequence Ladies("Leaderlady", lady);
    Ladies.append("Juliet", lady);
    Ladies.append("Elizabeth", lady);
    Ladies.append("Eve", lady);
    match(Gentlemans,Ladies);
    return 0;
}
//Person类成员函数
//构造函数
person::person(const char* newName, sex newSex)
{
    int len = (int)strlen(newName);
    name = new char[len+1];
    strcpy(name,newName);
    personSex = newSex;
}
//析构函数
person::~person()
{
    if (name) delete name;
    name = NULL;
    next = NULL;
}

//sequence类的成员函数
//构造函数
sequence::sequence(const char* newName,sex newSex)
{
    person* it = new person(newName,newSex);
    rear = front = new person("\0",man);
    rear->next = it;
    it->next = NULL;
    rear = it;
}
//生成新的person对象并加入队列
void sequence::append(const char* newName,sex newSex)
{
    person* it = new person(newName,newSex);
    rear->next = it;
    rear = it;
    rear->next = NULL;
}
//队头离开队列，并返回该对象
person& sequence::leave()
{
    if (front == rear)
    {
        cout << "Sequence is empty!" << endl;
        abort();
    }
    person& temp = *(front->next);
    front->next = front->next->next;
    if (rear==&temp) rear=front;
    temp.next = NULL;
    return temp;
}
//判断队列式是否为空
bool sequence::isEmpty()
{
    return (front->next==NULL)? true: false;
}
//match函数对两个队列从领队开始依次进行匹配
void match(sequence& qA,sequence& qB)
{
    //qA,qB队头分别出列进行配对
    while ((!qA.isEmpty()) && (!qB.isEmpty()))
    {
        qA.leave().print();
        cout << "'s partner is ";
        qB.leave().print();
        cout << "." << endl;
    }
    //显示未配对的嘉宾
    while (!qA.isEmpty())
    {
        qA.leave().print();
        cout << " has no partner yet." << endl;
        
    }
    while (!qB.isEmpty())
    {
        qB.leave().print();
        cout << " has no partner yet." << endl;
        
    }
}
