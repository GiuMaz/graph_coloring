#include <iostream>
#include "shrinking_set.h"

using namespace std;
int main( /* int argc, char *argv[] */ ) {
    ShrinkingSet s(1,10);
    s.print_status("CONSTRUCTION");

    s.erase(3);
    s.erase(8);
    s.erase(7);
    s.print_status("AFTER ERASE");

    s.add_value(7);
    s.add_value(11);
    s.add_value(8);
    s.add_value(8);
    s.add_value(22);
    s.print_status("AFTER INSERTION");

    s.reduce_to(22);
    s.print_status("REDUCE TO 22");
    s.reduce_to(21);
    s.print_status("REDUCE TO 21");
    s.reduce_to(20);
    s.print_status("REDUCE TO 20");
    s.reduce_to(8);
    s.print_status("REDUCE TO 8");


    cout << "POP ALL MINIMAL VALUE\n\t";
    while ( s.size() > 0 ) {
        cout << s.select_min() << " ";
        s.erase(s.select_min());
    }
    cout << endl << endl;

    s.add_value(100);
    s.print_status("INSERTION IN EMTPY");

    return 0;
}
