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
  size_t* scalar;
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




int main(){
    printf("Compilation passed");
    //This is the template for everything.
    getch();
    return 0;
}
