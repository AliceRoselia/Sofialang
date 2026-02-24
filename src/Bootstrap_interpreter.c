#include<stdio.h>

#include <conio.h>



struct primitiveContainer;

struct dictEntry
{
// Not sure if this is necessary yet. It might be possible we can force the primitive dicts to be perfect-hashable or something. We don't know yet.
size_t key;
size_t value;
};



// 8 bytes in normal 64-bits compilers.
// size_t would be used as raw bytes for numbers.
union data_ptr
{
  char* scalar;
  struct dictEntry* values;
  struct primitiveContainer* container;
  struct primitiveContainer (*primitiveFunc) (struct primitiveContainer); //For "primitive" and jit-compiled functions.
};


typedef struct primitiveContainer{
  // Primitive string, array, or dict.
  // Assume a dict unless shown otherwise.
  //
  size_t size;
  union data_ptr data;
}primitiveContainer;

//A primitive container is assumed to be a dict.
//Its "type" argument will tell you what its other slots contain.

size_t lookup_dict(primitiveContainer container, size_t key)
{
    for (size_t i=0; i<container.size; ++i)
    {
        if (container.data.values[i].key == key)
            return container.data.values[i].value;
    }
};

// A silly dict. This is a bootstrap implementation so perhaps we won't optimize this.

enum instruction_type{
    SofiaGoto,
    SofiaReturn,
    SofiaCall
};

typedef struct SofiaInstruction
{
    primitiveContainer src;
    enum instruction_type type;
}SofiaInstruction;


void interpret(primitiveContainer src, primitiveContainer context)
{

    size_t srcPtr = 0;
}



int main(){
    printf("Compilation passed");
    //This is the template for everything.
    getch();
    return 0;
}
