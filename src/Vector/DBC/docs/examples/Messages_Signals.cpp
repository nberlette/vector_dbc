#include <cstdlib>
#include <iostream>

#include "Vector/DBC.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        std::cout << "Messages_Signals <database.dbc>" << std::endl;
        return -1;
    }

    /* load database file */
    Vector::DBC::File file;
    if (file.load(argv[1]) != Vector::DBC::Status::Ok) {
        return EXIT_FAILURE;
    }

    /* loop over messages */
    for (auto message : file.messages) {
        std::cout << "Message " << message.second.name << std::endl;

        /* loop over signals of this messages */
        for (auto signal : message.second.signals) {
            std::cout << "  Signal " << signal.second.name << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
