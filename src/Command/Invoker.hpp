#pragma once

namespace command 
{

    // Invoker
    class Invoker {
    private:
        Command* command;
    public:
        // The setCommand method allows setting the command to
        // be executed.
        void setCommand(Command* cmd) { command = cmd; }
        // The executeCommand method triggers the execution of
        // the command.
        void executeCommand() { command->execute(); }
    };

}

//  Create a Receiver instance.
//     Receiver receiver;
 
//     // Create a ConcreteCommand, passing the Receiver to it.
//     ConcreteCommand command(receiver);
 
//     // Create an Invoker.
//     Invoker invoker;
 
//     // Set the command to be executed by the invoker.
//     invoker.setCommand(&command);
 
//     // Execute the command.
//     invoker.executeCommand();
