#include <iostream>
#include <boost/thread/thread.hpp>
void hello()
{
    std::cout << "Hello world, I'm a thread!" << std::endl;
    system("pause");
}
int main()
{
    boost::thread oneThread(&hello);
    oneThread.join();
}