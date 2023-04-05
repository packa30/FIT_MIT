/**
 * Author: Peter Grofcik, xgrofc00
 * Date: 6.3.2021
 */

/**
 * Invoker doesn't implement a request directly, instead refers to a Command to perform a request.
 * Supports Execution of a command and Undo.
 * This way makes invoker independent on how the request is performed.
 * @class{Invoker}
 */
export class Invoker {
    constructor() {
        this.commands = [];
    }

    ExecuteCommand(command) {
        this.command = command;
        this.commands.push(command);
        this.command.Execute();
    }

    UndoCommand() {
        if (this.command.length > 0){
            this.command = this.commands.pop();
            this.command.Undo();
        }else{
            console.log("nothing to be undone anymore");
        }

    }
}

/**
 * Command itself encapsulates action/request and it's undo version as well as receiver object which performs the request.
 * Exe actions and undo actions should have oposite effect and need's to have a correct format for execution on receiver.
 * @class{Command}
 */
export class Command {
    constructor(receiver, execute, undo) {
        this.receiver = receiver;
        this.execute = execute;
        this.undo = undo;
    }

    Execute() {
        this.receiver.Action(this.execute);
    }

    Undo() {
        this.receiver.Action(this.undo);
    }
}

/**
 * Receiver itself should simply perform the request passed on from Command.
 * This object should itself refer to a DB/object that perform requests if we just used Command design pattern, but for a universal implementation
 * I used this class that simply gets object in constructor and executes an action on it, so that whole implementation shows 3 main parts of it itself.
 * ** The only not universal part of implementation is that it requests a actions() function to be implemented in refered object.
 * @class{Receiver}
 */
export class Receiver {
    constructor(bankDB) {
        this.database = bankDB
    }

    Action(action) {
        this.database.actions(action)
    }
}