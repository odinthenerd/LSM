# KSM
Kvasir state machine is a state machine framework with an extremely simple API

The basic idea is to use a factory function to build a state machine out of lambdas (and for that matter Kvasir::Register:FieldActions) rather than uglier syntaxes used by boost.MSM, under the hood it would be quite similar to MSM.

Lets explain things with code:

```C++
struct A{};            //state
constexpr A* a{};  //tag
struct B{ int i; };            //state can have data
constexpr B* b{};  //tag
struct C{};            //state
constexpr C* c{};  //tag
struct C{};            //state
constexpr C* c{};  //tag
struct Ev1{ int i; };            //event can have data
constexpr Ev1* ev1{};  //tag
struct Ev2{};            //event
constexpr Ev2* ev2{};  //tag
auto myGuard = [](Ev1*){};
auto doSomethingElse = []{};
struct MyFunctor{
    void operator(Ev2*){}
};
auto sm = makeSm(
    state % a + [](auto& context){  /*+ means on entry*/} - [](){/*- means on exit*/},
    state % b, //we can leave out entry and exit if we don't need them
    
    //can overload onEntry depending on event type
    state % c + [](auto& context, Ev2*){ /*only used if entry from an ev2*/} + [](auto& context, void*){ /*used in all other cases*/},
    //states can also have parent states
    state % b/d + doSomethingElse,  //state d is a substate of state b
    transition(a,b, guard % [](Ev1*e){ return e->i ==4;}), //normal transition with guard
    transition(b,c, //multistep transition is a shortcut syntag for defining a long chain of transition 
        guard % event == ev2,  //we can also use shortcut 'event==' syntax for simple guards,
        []{ /*do something*/ }, //everything that is not a guard or rollback is an action
        doSomethingElse, //we can have one or more actions
        rollback % []{ /*do something*/ }, //roll back is triggered if an event occurs which is does not fulfill the guards
        guard % myGuard, //we can also use pre defined lambdas 
        //note we are not handeling Ev2 here, if it were to occur it would trigger a rollback
        MyFunctor{}, //functors also work as actions (or guards for that matter)
        guard % myGuard - event == ev2  //by using - we are specifying that Ev2 events should be ignorred (not trigger roll back)
        set(Kvasir::Uart0Cfg::enable), //Kvasir FieldActions can also be used seamlessly
        guard % or(event == ev2, [](Ev1*e){/*...*/}),  //we can also combine guards 
        [](auto context){ get<B>(context).i = 4; }, //actions can also access all parent states
        guard % and([](auto context){ get<B>(context).i > 1; }, event == ev1), //guards can too
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

This is what the multistep transition would look like (assuming names lambdas exist for actions and guards):
```C++
transition(a,b,
    guard % userWantsToQuiry, 
    findDatabase,
    guard % databaseFound,
    openDatabase,
    guard % success,
    rollback % closeDatabase,
    openTable,
    guard % success,
    rollback % closeTable,
    performQuery,
    guard % success,
    performOtherQuirey,
    guard % success,
    postResultToSomewhere)
    
```
