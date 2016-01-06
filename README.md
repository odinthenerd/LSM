# KSM
Kvasir state machine is a state machine framework with an extremely simple API

The basic idea is to use a factory function to build a state machine out of lambdas (and for that matter Kvasir::Register:FieldActions) rather than uglier syntaxes used by boost.MSM, under the hood it would be quite similar to MSM.

struct A{};            //state
constexpr A* a{};  //tag
struct A{ int i; };            //state can have data
constexpr B* b{};  //tag
struct A{};            //state
constexpr C* c{};  //tag
struct Ev1{ int i; };            //event can have data
constexpr Ev1* ev1{};  //tag
struct Ev2{};            //event
constexpr Ev2* ev2{};  //tag
auto isEv2 = [](Ev2*){};
auto doSomethingElse = []{};
auto sm = makeSm(
    state % a + [](auto& context){  /*+ means on entry*/} - [](){/*- means on exit*/},
    state % b, //we can leave out entry and exit if we don't need them
    state % c + [](auto& context, Ev2*){ /*only used if entry from an ev2*/} + [](auto& context, void*){ /*used in all other cases*/}, //can overload onEntry depending on event type
    transition(a,b, guard % [](Ev1*e){ return e->i ==4;}), //normal transition
    transition(b,c,
        guard % isEv2,  //we can also use pre defined lambdas,
        action % []{ /*do something*/ } + doSomethingElse, //we can have one or more actions
        rollback % []{ /*do something*/ }, //roll back is triggered if an event occurs which is does not fulfill the guards
        guard % or(isEv2, [](Ev1*e){/*...*/})  //next step

The other main innovation is the addition of a "multi step transition".
