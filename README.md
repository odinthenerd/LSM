# LSM
Lambda state machine is a state machine framework with an extremely simple API

The basic idea is to use a factory function to build a state machine out of lambdas rather than uglier syntaxes used by boost.MSM, under the hood it would be quite similar to MSM except it will compile far faster due to the use of brigand rather than boost.MPL.

Lets explain things with code:

```C++
struct StateData{ int i; };            //state can have data
struct Ev1{ int i; };            //event can have data
struct Ev2{};            //event
auto myGuard = [](Ev1&){};
auto doSomethingElse = []{};
struct MyFunctor{
    void operator(Ev2&){}
};
auto sm = makeSm(
    "someState"_s(data = StateData(), entry = [](auto& context){}, exit = []{}),
    "otherState"_s, //we can leave out entry and exit if we don't need them
    
    //can overload onEntry depending on event type
    "anotherState"_s(entry = [](auto& context, Ev2*){ /*only used if entry from an ev2*/}, entry = [](auto& context, void*){ /*used in all other cases*/}),
    //states can also have parent states
    "someState"_s/"childState"_s(entry = doSomethingElse),  //state d is a substate of state b
    transition("someState"_s,"otherState"_s, guard = [](Ev1*e){ return e->i ==4;}), //normal transition with guard
    transition("otherState"_s,"anotherState"_s, //multistep transition is a shortcut syntax for defining a long chain of transition 
        guard = event == ev2,  //we can also use shortcut 'event==' syntax for simple guards,
        []{ /*do something*/ }, //everything that is not a guard or rollback is an action
        action = doSomethingElse, //we can have one or more actions and actions can be explicit
        rollback = []{ /*do something*/ }, //roll back is triggered if an event occurs which is does not fulfill the guards
        guard = myGuard, //we can also use pre defined lambdas 
        //note we are not handeling Ev2 here, if it were to occur it would trigger a rollback
        MyFunctor{}, //functors also work as actions (or guards for that matter)
        guard = myGuard - event == ev2  //by using - we are specifying that Ev2 events should be ignorred (not trigger roll back)
        []{},
        guard = or(event == ev2, [](Ev1*e){/*...*/}),  //we can also combine guards 
        [](auto context){ context[state<B>].i = 4; }, //actions can also access all parent states
        guard = and([](auto context){ context[state<B>].i > 1; }, event == ev1), //guards can too
    ));
        
```

because we can specify actions and guards inline using lambdas the code becomes much more readable. The other main innovation is the addition of a "multi step transition". Depending on the design we often see a pattern of action, wait for event, action, wait for another event, where the order of actions and events we are waiting for are pretty fixed and linier. For example:

- find database
 - database found
- open database
 - success
- open table
 - success
- perform quirey
 - success
- perform other quirey
 - success

Looking at this we can see that we are only ever waiting for one particular event each time, if another event occurrs we have no hope of handeling it, in fact it probably means either an error or the user has canceled the request. In that case its best to roll back all the changes we have made so far (close the database etc.). 

For this (pretty common) special case we provide multistep transitions. Internally they are just a lot of substates in the origin state.

This is what the multistep transition would look like (assuming named lambdas exist for actions and guards):
```C++
transition("NotConnected"_s,"Done"_s>,
    guard = userWantsToQuiry, 
    findDatabase,
    guard = databaseFound,
    openDatabase,
    guard = success,
    rollback = closeDatabase,
    openTable,
    guard = success,
    rollback = closeTable,
    performQuery,
    guard = success,
    performOtherQuirey,
    guard = success,
    postResultToSomewhere)
    
```
