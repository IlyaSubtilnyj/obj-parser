#pragma once
namespace command 
{
    
    // Command interface
    class Command {
    public:
        // The execute method is declared in the Command
        // interface.
        virtual void execute() = 0;
    };

    // Concrete Command
    class ConcreteCommand : public Command {
    private:
        Receiver& receiver;
    
    public:
        // ConcreteCommand takes a reference to a Receiver
        // object in its constructor.
        ConcreteCommand(Receiver& rec)
            : receiver(rec)
        {
        }
    
        // The execute method calls the action on the Receiver.
        void execute() { receiver.performAction(); }
    };

}