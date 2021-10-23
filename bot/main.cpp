#include <iostream>
#include "Client.hpp"

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        std::cerr << "Invalid number of arguments" << std::endl;
        return (1);
    }
    try
    {
        Client c(argv[1], argv[2], argv[3]);
        c.run();
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << std::endl;
    }
    return (0);
}
