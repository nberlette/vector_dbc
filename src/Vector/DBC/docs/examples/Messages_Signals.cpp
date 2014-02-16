#include <cstdlib>
#include <iostream>

#include "Vector/DBC/Database.h"

int main()
{
    Vector::DBC::Database database;
    database.load(CMAKE_CURRENT_SOURCE_DIR "/data/Database.dbc");

    /* loop over messages */
    for (auto message : database.messages) {
        std::cout << "Message " << message.second.name << std::endl;

        /* loop over signals of this messages */
        for (auto signal : message.second.signals) {
            std::cout << "  Signal " << signal.second.name << std::endl;
        }
    }

    return EXIT_SUCCESS;
}
