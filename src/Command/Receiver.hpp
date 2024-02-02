#pragma once

namespace command 
{

    // Receiver
    class Receiver {
    public:
        // Receiver class defines the action to be performed.
        void performAction()
        {
            std::cout << "Receiver is performing an action."
                    << std::endl;
        }
    };

}