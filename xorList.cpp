#include <iostream>
#include <string.h>
#define MAX_LENGTH 30
#define COMMANDS 12
#define NUM_PTR 5
#pragma warning(disable:4996)
using namespace std;
struct list {
    int data;
    list* ptr;
};

enum pointers {
    BEG, ACT, END, AACT, BACT
};
enum  commands {
    ACTUAL, NEXT, PREV, ADD_BEG, ADD_END,
    ADD_ACT, DEL_BEG, DEL_END, DEL_VAL,
    DEL_ACT, PRINT_FORWARD, PRINT_BACKWARD
};

list* XOR(list* node1, list* node2) {
    return (list*)((uintptr_t)(node1) ^ (uintptr_t)(node2));
}

//funckja zlicza ilosc wezlow w liscie
int count(list** ptr)
{
    list* elem = ptr[BEG];
    if (elem != NULL)
    {
        if (elem->ptr != NULL)
        {
            if (ptr[BEG]->ptr != ptr[END])
            {
                int amount = 1;
                list* act = XOR(ptr[BEG]->ptr, ptr[END]);
                list* prev = ptr[BEG];
                list* help;
                while (act != ptr[BEG])
                {
                    amount++;
                    help = act;
                    act = XOR(act->ptr, prev);
                    prev = help;
                }
                return amount;
            }
            else
                return 2;
        }
        else
            return 1;
    }
    else
        return 0;
}

//funckja sluzy do wypisania w konsoli danych w liscie 
//w zaleznosc jaki wskaznik przezkazemy jako pierwszy, funckja
//wypisze dane od poczatku do konca lub odwrotnie
void Print(list** ptr, list* start, list* prev) {
    int size = count(ptr);
    if (size != 0) {
        cout << start->data << " ";
        if (size == 2)
        {
            cout << prev->data << endl;
            return;
        }
        else if (size > 2)
        {
            list* help = start;
            list* tmp2 = XOR(start->ptr, prev);
            prev = help;
            while (tmp2 != start)
            {
                cout << tmp2->data << " ";
                help = tmp2;
                tmp2 = XOR(tmp2->ptr, prev);
                prev = help;
            }
            cout << endl;
        }
    }
    else
        cout << "NULL" << endl;
}
#pragma region add
//funkcja dodaje wezel na poczatku
//inty w parametrach funckji to indeksy wezlow
//roznia sie one w zalensoci od tego czy dodajemy na poczatku czy na koncu
//act_friend1 to BACT jezeli dodajemy na poczatku i AACT jezeli na koncu
void add_beg_end(list** pointers, int main, int other, int act_friend1, int act_friend2, int size, list*& elem)
{
    if (size > 1)
    {
        if (pointers[ACT] == pointers[main])
            pointers[act_friend1] = elem;
        elem->ptr = XOR(pointers[other], pointers[main]);
        if (pointers[main]->ptr != pointers[other])
        {
            pointers[main]->ptr = XOR(elem, XOR(pointers[main]->ptr, pointers[other]));
            pointers[other]->ptr = XOR(elem, XOR(pointers[other]->ptr, pointers[main]));
        }
        else
        {
            pointers[main]->ptr = XOR(elem, pointers[other]);
            pointers[other]->ptr = XOR(elem, pointers[main]);
        }
        if (pointers[ACT] == pointers[other])
        {
            pointers[act_friend2] = elem;
        }
    }
    else
    {
        pointers[act_friend1] = elem;
        pointers[act_friend2] = elem;
    }
    pointers[main] = elem;
}

//dodajemy nowy element przed aktualnym wezlem
void add_act(list** ptr, int size, list*& elem) {
    if (ptr[BEG] == ptr[ACT])
    {
        ptr[BEG] = elem;
    }
    elem->ptr = XOR(ptr[ACT], ptr[BACT]);
    if (size > 2)
    {
        ptr[BACT]->ptr = XOR(elem, XOR(ptr[BACT]->ptr, ptr[ACT]));
        ptr[ACT]->ptr = XOR(elem, XOR(ptr[BACT], ptr[ACT]->ptr));
    }
    else
    {
        ptr[BACT]->ptr = XOR(elem, ptr[ACT]);
        ptr[ACT]->ptr = XOR(elem, ptr[AACT]);
    }
    ptr[BACT] = elem;
}

//funckja dodaje wezel do listy
//w zmiennej controller mamy informacje w jakim miejscu listy mamy go dodac
void add_node(list** ptr, int controller, int data) {
    int size = count(ptr);
    list* elem = new list;
    elem->data = data;
    if (size == 0)
    {
        for (int i = 0;i < NUM_PTR;i++)
            ptr[i] = elem;
        elem->ptr = nullptr;
    }
    else if (size > 0) {
        if (size == 1) {
            elem->ptr = ptr[BEG];
            ptr[BEG]->ptr = elem;
        }
        switch (controller) {
        case 0:
            add_beg_end(ptr, BEG, END, BACT, AACT, size, elem);
            break;
        case 1:
            add_beg_end(ptr, END, BEG, AACT, BACT, size, elem);
            break;
        case 2:
            add_act(ptr, size, elem);
            break;
        }
    }
}
#pragma endregion 
//fucnkja sprawdza,czy wezel jest zainicjowany
//jezeli tak to wyswietla jego wartosc
void value_node(list* node) {
    if (node == nullptr)
        cout << "NULL" << endl;
    else
        cout << node->data << endl;
}

//funckja przestawia wskaznik ACT,BACT,AACT do przodu lub do tylu
//jesli main to indeks to BACT to przesuwamy do przodu
//jesli main to AACT to do tylu
void next_prev(list** pointers, int main, int other) {
    int size = count(pointers);
    if (size == 0)
    {
        cout << "NULL" << endl;
        return;
    }
    else if (size == 1)
    {
    }
    else if (size == 2)
    {
        list* tmp = pointers[ACT];
        pointers[ACT] = pointers[main];
        pointers[main] = tmp;
        pointers[other] = tmp;
    }
    else
    {
        list* tmp = pointers[other];
        pointers[other] = XOR(tmp->ptr, pointers[ACT]);
        pointers[main] = pointers[ACT];
        pointers[ACT] = tmp;
    }
    cout << pointers[ACT]->data << endl;
}
#pragma region del
//funckja usuwa wezel, na ktory wskazuje wskaznik pod indeksem main
//jezeli cotroller jest rowny 1 to na dany wezel wskazuje jeszcze jeden wskaznik
//jezeli 2 to 2 wskazniki, pod zminnymi bonus kryja sie indeksy dodtkowych wezlow
void delete_node(list** pointers, int size, int main, int prev, int bonus, int bonus2, int controller)
{
    list* tmp;
    if (size == 2)
    {
        tmp = pointers[prev];
        tmp->ptr = NULL;
        delete tmp->ptr;
    }
    else if (size == 3)
    {
        tmp = XOR(pointers[main]->ptr, pointers[prev]);
        tmp->ptr = pointers[prev];
        pointers[prev]->ptr = tmp;
        delete pointers[main];
    }
    else
    {
        tmp = XOR(pointers[main]->ptr, pointers[prev]);
        tmp->ptr = XOR(pointers[prev], XOR(tmp->ptr, pointers[main]));
        pointers[prev]->ptr = XOR(XOR(pointers[prev]->ptr, pointers[main]), tmp);
        delete pointers[main];
        pointers[main] = tmp;
    }
    pointers[main] = tmp;
    if (controller == 2)
    {
        pointers[bonus] = tmp;
        pointers[bonus2] = tmp;
    }
    if (controller == 1)
        pointers[bonus] = tmp;
}

//funckja przesuwa wskaznik przed lub za ACT o jedno miejsce
void   move_pointer(list*& main, list*& prev, int size) {
    if (size > 2)
        main = XOR(prev, main->ptr);
}

//funckja sprawdza,czy wezel(pierwszy lub ostatni) jest jednoczesnie
//wezlem poprzedzajacym lub nastepnym po ACT
//jezeli usuwamy poczatek listy to zmienna indeksmain to indeks poczatkowego 
//jezeli ostatni to ostatniego, odwrotnie jest z zmienna indeks other
void delete_beg_end(list** pointers, int size, int indexmain, int indexother)
{
    int controller = 0;
    if (size == 2)
    {
        controller = 2;
        delete_node(pointers, size, indexmain, indexother, AACT, BACT, controller);
    }
    else if (pointers[indexmain] == pointers[AACT])
    {
        move_pointer(pointers[AACT], pointers[ACT], size);
        delete_node(pointers, size, indexmain, indexother, 0, 0, controller);
    }
    else if (pointers[indexmain] == pointers[BACT])
    {
        move_pointer(pointers[BACT], pointers[ACT], size);
        delete_node(pointers, size, indexmain, indexother, 0, 0, controller);
    }
    else
    {
        delete_node(pointers, size, indexmain, indexother, 0, 0, controller);
    }
}

//w tej funckcji usuwamy wezel, na ktory wskazuje wskaznic ACT
//na poczatku sprawdzamy czy dany wezel jest jednoczesnie poczatkiem lub koncem listy
//dane na ten temat zapsiujemy do zmiennej controller2, ktora na poczatku jest zainicjowana 1
//jesli na wezel wskazuje jeszcze inny wskaznik to zapisujemy to w zmiennej controller
void del_act(list** ptr, int size) {
    int controller2 = 1;
    int controller = 0;
    if (ptr[ACT] == ptr[END]) {
        controller = 1;
        controller2 = END;
    }
    else if (ptr[ACT] == ptr[BEG]) {
        controller2 = BEG;
        if (size == 2)
            controller = 1;
    }
    move_pointer(ptr[BACT], ptr[ACT], size);
    switch (controller2) {
    case 1:

        delete_node(ptr, size, ACT, AACT, 0, 0, controller);
        break;
    case END:

        delete_node(ptr, size, ACT, AACT, END, 0, controller);
        break;
    case BEG:
        move_pointer(ptr[BEG], ptr[END], size);
        delete_node(ptr, size, ACT, AACT, BEG, 0, controller);
    }
}

//funckja na podstawie tego, ile wezlow ma lista
//albo usuwa jedyny wezel
//albo uruchamia odpowednia fucnkje
void del(list** pointers, int control) {
    int number = count(pointers);
    if (number == 0)
    {
    }
    else if (number == 1)
    {
        delete pointers[ACT];
        for (int i = 0;i < NUM_PTR;i++)
            pointers[i] = NULL;
    }
    else
    {
        switch (control) {
        case 0:
            delete_beg_end(pointers, number, BEG, END);
            break;
        case 1:
            delete_beg_end(pointers, number, END, BEG);
            break;
        case 2:
            del_act(pointers, number);
            break;
        }
    }
}

//funckja usuwa wezel, ktory nie jest poczatkiem, koncem ani aktualnym elementem
//zmienna control mowi nam czy wezel ten jest sąsiadem aktualnego wezla
//jezeli tak to przesuwa 
void del_casual(list*& node_main, list*& node_prev, int control, list** pointers) {
    int size = count(pointers);
    list* tmp = node_main;
    node_main = XOR(node_main->ptr, node_prev);
    if (size > 3) {
        node_main->ptr = XOR(XOR(node_main->ptr, tmp), node_prev);
        node_prev->ptr = XOR(XOR(node_prev->ptr, tmp), node_main);
    }
    else {
        node_main->ptr = node_prev;
        node_prev->ptr = node_main;
    }
    if (control != 0)
    {
        pointers[control] = XOR(pointers[control]->ptr, pointers[ACT]);
    }
    delete tmp;
}

//funckja sprawdza, czy wezel ktory chcemy usunac jest jednym 
//z wezlow,na ktory wskazują przechowywane przez nas wskazniki
//na podstawie tego uruchamia odpowiednie funckje
void  which_node_del(list** ptr, list*& to_delete, list* prev) {
    int controller;
    if (to_delete == ptr[ACT])
    {
        controller = 2;
        del(ptr, controller);
        to_delete = ptr[ACT];
    }
    else if (to_delete == ptr[BEG])
    {
        controller = 0;
        del(ptr, controller);
        to_delete = ptr[BEG];
    }
    else if (to_delete == ptr[END])
    {
        controller = 1;
        del(ptr, controller);
        to_delete = ptr[END];
    }
    else if (to_delete == ptr[AACT])
    {
        del_casual(to_delete, prev, AACT, ptr);
    }
    else if (to_delete == ptr[BACT])
    {
        del_casual(to_delete, prev, BACT, ptr);
    }
    else
    {
        del_casual(to_delete, prev, 0, ptr);
    }
}

//funckja sprawdza,czy w liscie sa wezly z daną liczbą 
//jezeli tak to usuwa go
void del_val(list** ptr, int data) {
    int size = count(ptr);
    int size2 = size;
    list* act = ptr[BEG];
    list* prev = ptr[END];
    list* help;
    int controller;
    for (int i = 0;i < size;i++)
    {
        controller = 0;
        if (act->data == data)
        {
            controller = 1;
            which_node_del(ptr, act, prev);
            size2--;
        }
        if (controller == 0)
        {
            if (size2 == 1)
            {
            }
            if (size2 == 2)
            {
                act = act->ptr;
            }
            else {
                help = act;
                act = XOR(prev, act->ptr);
                prev = help;
            }
        }
    }
}

//funckja usuwa pozostale wezly z listy
void delete_list(list** ptr)
{
    int size = count(ptr);
    int controller = 0;
    for (int i = 0;i < size;i++)
    {
        if (ptr[ACT] == ptr[BEG])
            controller = 2;
        else
            controller = 0;
        del(ptr, controller);
    }
}
#pragma endregion

//funckja na podstawie komendy uruchamia kolejne funckje
void choose(char* command, list** pointers, const char** commands)
{
    int data = 0;
    int controller;
    if (!strncmp(command, commands[ACTUAL], 6))
        value_node(pointers[ACT]);
    if (!strncmp(command, commands[PREV], 4))
        next_prev(pointers, AACT, BACT);
    if (!strncmp(command, commands[NEXT], 4))
        next_prev(pointers, BACT, AACT);
    if (!strncmp(command, commands[ADD_END], 7))
    {
        cin >> data;
        controller = 1;
        add_node(pointers, controller, data);
    }
    if (!strncmp(command, commands[ADD_BEG], 7))
    {
        cin >> data;
        controller = 0;
        add_node(pointers, controller, data);
    }
    if (!strncmp(command, commands[ADD_ACT], 7)) {
        cin >> data;
        controller = 2;
        add_node(pointers, controller, data);
    }
    if (!strncmp(command, commands[DEL_VAL], 7)) {
        cin >> data;
        del_val(pointers, data);
    }
    if (!strncmp(command, commands[DEL_END], 7)) {
        if (pointers[ACT] == pointers[END]) {
            controller = 2;
        }
        else
            controller = 1;
        del(pointers, controller);
    }
    if (!strncmp(command, commands[DEL_BEG], 7)) {
        if (pointers[ACT] == pointers[BEG]) {
            controller = 2;
        }
        else
            controller = 0;
        del(pointers, controller);
    }
    if (!strncmp(command, commands[DEL_ACT], 7)) {
        controller = 2;
        del(pointers, controller);
    }
    if (!strncmp(command, commands[PRINT_FORWARD], 13))
        Print(pointers, pointers[BEG], pointers[END]);
    if (!strncmp(command, commands[PRINT_BACKWARD], 14))
        Print(pointers, pointers[END], pointers[BEG]);
}

const char** create_command() {
    const char** arr = (const char**)malloc(sizeof(char*) * COMMANDS);
    if (arr == nullptr)
    {
        cout << "Error";
        return arr;
    }
    arr[ACTUAL] = "ACTUAL";
    arr[NEXT] = "NEXT";
    arr[PREV] = "PREV";
    arr[ADD_BEG] = "ADD_BEG";
    arr[ADD_END] = "ADD_END";
    arr[ADD_ACT] = "ADD_ACT";
    arr[DEL_BEG] = "DEL_BEG";
    arr[DEL_END] = "DEL_END";
    arr[DEL_VAL] = "DEL_VAL";
    arr[DEL_ACT] = "DEL_ACT";
    arr[PRINT_FORWARD] = "PRINT_FORWARD";
    arr[PRINT_BACKWARD] = "PRINT_BACKWARD";
    return arr;
}

int main()
{
    list** pointers = (list**)malloc(sizeof(list*) * NUM_PTR);
    if (pointers == nullptr)
    {
        cout << "Error";
        return -1;
    }
    for (int i = 0;i < NUM_PTR;i++)
    {
        pointers[i] = nullptr;
    }
    char command[MAX_LENGTH];
    const char** commands = create_command();
    while (scanf("%s", command) != EOF)
    {
        choose(command, pointers, commands);
    }
    delete_list(pointers);
    free(pointers);
    free(commands);
    return 0;
}
