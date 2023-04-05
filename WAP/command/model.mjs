/**
 * Author: Peter Grofcik, xgrofc00
 * Date: 6.3.2021
 */
import {Invoker, Command, Receiver} from './library.mjs'

/**
 * Shown example stands for a simple bank DB of it's own accounts and Clients/Owners
 * -> shows example of Multi-level undo
 */

/**
 * Class action stands for a action to be done on DB object // work with finances on one or between two acctouns
 * Object itself is passed to a Command object as a Exec or Undo action // Each command expects both, where undo is an oposite action
 *                                                                         withdraw - deposit, move - move (exchange from and to)
 */
class action {
    constructor(actionType, value, from, to) {
        this.actionType = actionType;
        this.value = value;
        this.from = from;
        this.to = to;
    }
}

/**
 * Simply put bankDB is a Receiver (object passed to a Receiver in constructor)
 * ** Requirement function actions(), witch can
 * It's own constructor requires just a array of Accounts
 *
 */
class bankDB {
    constructor(accounts) {
        this.accounts = accounts
    }

    moveFin(from, to, value){
        this.accounts.forEach(
            function(account){
                if(account.accountNum == from){
                    account.withdrawal(value);
                }
                if(account.accountNum == to){
                    account.deposit(value);
                }
            }
        )
    }

    actions(action){
        switch (action.actionType){
            case "withdrawal":
                this.accounts.forEach(
                    function (account){
                        if(account.accountNum == action.from){
                            account.withdrawal(action.value);
                        }
                    }
                )
                break;
            case "deposit":
                this.accounts.forEach(
                    function (account){
                        if(account.accountNum == action.to){
                            account.deposit(action.value);
                        }
                    }
                )
                break;
            case "move" :
                this.moveFin(action.from, action.to, action.value);
                break;
            default:
                break;
        }
    }
}

/**
 * just stores different accounts in bank and provide deposit / withdraw // move is just a combination of withdraw and deposit
 */
class account {
    constructor(owner, accountNum, finance ) {
        this.owner = owner;
        this.accountNum = accountNum;
        this.finance = finance;
    }

    deposit(value){
        this.finance += value;
        console.log("depositing " + value + " to account " + this.accountNum);
    }

    withdrawal(value){
        this.finance -=value;
        console.log("withdrawing " + value + " from account " + this.accountNum);
    }

    shFinance(){
        return this.finance;
    }

}

/**
 * simple client class // not really needed just created in process of modeling problem based on IUS/IIS project
 */
class Client {
    constructor(name, birthYear) {
        this.name = name;
        this.birthYear = birthYear;
    }
}


/**
 * creating a mocking DB as object passed to Receiver
 * @type {bankDB}
 */
const db = new bankDB([
    new account(new Client("Peter", 1996),119, 500),
    new account(new Client("Michal", 1986),252, 1500)]);

/**
 * Simple actions & co-actions needed for command constructor
 * @type {action}
 */
const action1 = new action("move", 100, 119, 252);
const action1_u = new action("move", 100, 252, 119);

const action2 = new action("withdrawal", 500, 252, null);
const action2_u = new action("deposit", 500, null, 252);

const action3 = new action("deposit", 150, null, 119);
const action3_u = new action("withdrawal", 150, 119, null);


const receiver = new Receiver(db);

/**
 * commands to be stored and executed by invoker
 * @type {Command}
 */
const command1 = new Command(receiver, action1, action1_u);
const command2 = new Command(receiver, action2, action2_u);
const command3 = new Command(receiver, action3, action3_u);

const invoker = new Invoker();
console.log("Showing DB state before actions");
db.accounts.forEach(
    function (account) {
        console.log("Account " + account.accountNum + " has " + account.finance + " finances");
    }
)

console.log("\nExecuting actions\n");
invoker.ExecuteCommand(command1);
invoker.ExecuteCommand(command2);
invoker.ExecuteCommand(command3);

console.log("\nShowing DB state after executed actions");
db.accounts.forEach(
    function (account) {
        console.log("Account " + account.accountNum + " has " + account.finance + " finances");
    }
)

console.log("\nPerforming multi-level undo for last 2 actions\n");
invoker.UndoCommand();
invoker.UndoCommand();

console.log("\nShowing DB state after undo");
db.accounts.forEach(
    function (account) {
        console.log("Account " + account.accountNum + " has " + account.finance + " finances");
    }
)
