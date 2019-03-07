#include <RTOS.h>
#include <Test.h>
#include <string.h>

int led = 13;

int main() {
    
    using namespace RTOS;

    RTOS::init();

    // "Statically" allocate some memory
    char * s1 = (char *) Memory::static_alloc("s1", 16);
    char * s2 = (char *) Memory::static_alloc("s1", 16);
    strcpy(s1, "hello");
    strcpy(s2, "world");
    
    assert(strcmp(s1, s2) != 0);
    strcpy(s1, "equal");
    strcpy(s2, "equal");
    assert(strcmp(s1, s2) == 0);

    // Create a memory pool
    Memory::Pool_t * my_pool = Memory::Pool::init("my_pool", sizeof(int), 4);
    int * i1 = (int *) Memory::Pool::alloc(my_pool);
    *i1 = 1;
    int * i2 = (int *) Memory::Pool::alloc(my_pool);
    *i2 = 2;
    int * i3 = (int *) Memory::Pool::alloc(my_pool);
    *i3 = 3;
    int * i4 = (int *) Memory::Pool::alloc(my_pool);
    *i4 = 4;
    Memory::Pool::cons(i1, i2);
    Memory::Pool::cons(i2, i3);
    Memory::Pool::cons(i3, i4);
    
    assert(*i1 == 1);
    assert(*((int *) Memory::Pool::cdr(i1)) == 2);
    assert(*((int *) Memory::Pool::cdr(Memory::Pool::cdr(i1))) == 3);
    assert(*((int *) Memory::Pool::cdr(Memory::Pool::cdr(Memory::Pool::cdr(i1)))) == 4);

    Memory::Pool::dealloc(my_pool, i3);
    int *i5 = (int *) Memory::Pool::alloc(my_pool);
    *i5 = 5;

    assert(*i1 == 1);
    assert(*((int *) Memory::Pool::cdr(i1)) == 2);
    assert(*((int *) Memory::Pool::cdr(Memory::Pool::cdr(i1))) == 5);
    assert(Memory::Pool::cdr(Memory::Pool::cdr(Memory::Pool::cdr(i1))) == nullptr);

    // Allocate too much memory
    Memory::static_alloc("overflow", RTOS_VIRTUAL_HEAP);

    // Allocate too many chubnks
    Memory::Pool::alloc(my_pool);

    RTOS::halt();
    return 0;
}

namespace RTOS {
namespace UDF {

    void trace(Trace_t * t) {
        Trace::serial_trace(t);
    }

    bool error(Trace_t * t) { 
        static int trace_no = 0;
        trace_no++;
        switch (trace_no) {
            case 1:
                assert(t->tag == Error_Max_Alloc);
                break;
            case 2:
                assert(t->tag == Error_Max_Pool);
                break;
            default:
                break;
        }
        return true; 
    }

}}