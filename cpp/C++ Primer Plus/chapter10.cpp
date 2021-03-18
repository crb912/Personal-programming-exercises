//
// Created by bing on 3/18/21.
//
#include <string>
#include <iostream>
#include <cstring>

// ------------------------- Exercise 2 --------------------------------------
class Person {
private:
    static const int LIMIT = 25;
    std::string lname; // Person’s last name
    char fname[LIMIT]; // Person’s first name
public:
    Person()
    {
        lname = "";
        fname[0] = '\0';
    } // #1
    Person(const std::string& ln, const char* fn = "Heyyou"); // #2
    // the following methods display lname and fname
    void Show() const; // firstname lastname format
    void FormalShow() const; // lastname, firstname format
};



Person::Person(const std::string& ln, const char* fn)
{
    lname = ln;
    strncpy(fname, fn, LIMIT-1);
}



void Person::Show() const
{
    std::cout << fname << " " << lname << std::endl;
}

void Person::FormalShow() const
{
    std::cout << lname  << ", " << fname<< std::endl;
}

void test_exercise_2(){
    Person one;
    Person two("Smythecraft");
    Person three("Dimwiddy", "Sam");
    one.Show();
    one.FormalShow();
    std::cout << std::endl;
    two.Show();
    two.FormalShow();
    std::cout << std::endl;
    three.Show();
    three.FormalShow();
}

// ----------------------------- Exercise 5 -----------------------------------

typedef struct customer
{
  char fullname[35];
  double payment;
} Item;

class Stack
{
private:
    enum {MaxCustom = 10};
    double total_payments = 0.0;
    Item cus_stack[MaxCustom];
    int top = 0;  // stack top
public:

    void add(Item x)
    {
        if (MaxCustom == top)
        {
            std::cout << "error stack full" << std::endl;
            return;
        }
        cus_stack[top] = x;
        ++top;
        std::cout << "Welcome: " << x.fullname << std::endl;
    }

    void remove()
    {
        if (0 == top)
        {
            std::cout << "error, stack empty!" << std::endl;
            return;
        }
        auto x = cus_stack[top-1];
        total_payments += x.payment;
        --top;
        std::cout << "Total payments: " << total_payments << std::endl;
    }
};

void test_exercise_5()
{
    Stack s = Stack();
    Item cus1{"Bob", 2.3};
    s.add(cus1);
    s.remove();
    Item cus2{"Tom", 2.7};
    s.add(cus2);
    s.remove();
}






// ----------------------------------------------------------------------------
int main(){
    //test_exercise_2();
    test_exercise_5();
}