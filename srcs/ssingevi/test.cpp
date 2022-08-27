#include <cstdarg>
#include <iostream>
#include <string>


class test_class
{
    public:

    typedef void (test_class::*pf)(void);
    // std::string str;
    test_class(){};

    ~test_class(){};

    void test_2(void)
    {
        std::cout << "test_2 " << std::endl;
    }
    void test(int nb, ...)
    {
        va_list ap;
        // va_list ap_2;
        va_start(ap, nb);
        pf fct;

        fct = va_arg(ap, void (test_class::*)(void));
        va_end(ap);
        (this->*fct)();
        // test_2();
        // va_start(ap_2, nb);
        // ap_2 = ap;
        // test_2(ap);
        // std::cout << va_arg(ap_2, int)<< std::endl;
        // std::cout << va_arg(ap_2, int)<< std::endl;
        // std::cout << va_arg(ap_2, int)<< std::endl;
    }


};

int main(void)
{
    test_class test;
    test.test(1, &test_class::test_2);
}