#include<stdio.h>
#include<stdlib.h>
#include <conio.h>



struct primitiveContainer;
struct dictEntry;

constexpr size_t SOFIA_TYPE = 0;
constexpr size_t SOFIA_VAR_KEY = 1;
constexpr size_t SOFIA_SRC = 2;
constexpr size_t SOFIA_CONTEXT = 3;


char* SOFIA_MEMORY;
char* SOFIA_CURRENT_MEMORY;
char* Sofia_malloc(size_t SIZE)
{
    char* result = SOFIA_CURRENT_MEMORY;
    SOFIA_CURRENT_MEMORY += SIZE;
    return result;
}



// 8 bytes in normal 64-bits compilers.
// size_t would be used as raw bytes for numbers.
union data_ptr
{
  size_t literal;
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

primitiveContainer make_dict();

typedef struct dictEntry
{
// Not sure if this is necessary yet. It might be possible we can force the primitive dicts to be perfect-hashable or something. We don't know yet.
size_t key;
struct primitiveContainer value;
}dictEntry;

//A primitive container is assumed to be a dict.
//Its "type" argument will tell you what its other slots contain.

primitiveContainer lookup_dict(primitiveContainer container, size_t key)
{
    for (size_t i=0; i<container.size; ++i)
    {
        if (container.data.values[i].key == key)
            return container.data.values[i].value;
    }
};

size_t lookup_scalar(primitiveContainer container)
{
    return container.data.literal;
}

void insert_dict(primitiveContainer container, size_t key, primitiveContainer value)
{
    for (size_t i=0; i<container.size; ++i)
    {
        if (container.data.values[i].key == key)
            container.data.values[i].value = value;
            return;
    }
    if (container.size & (container.size-1)) //If not a power of two.
    {
        container.data.values[container.size].value = value;
        container.data.values[container.size].key = key;
        ++container.size;
    }
    else
    {
        dictEntry* newEntry = (dictEntry*)Sofia_malloc(sizeof(dictEntry));
        for (size_t i=0; i<container.size; ++i)
        {
            newEntry[i].key = container.data.values[i].key;
            newEntry[i].value = container.data.values[i].value;
        }
        container.data.values[container.size].value = value;
        container.data.values[container.size].key = key;
        ++container.size;
    }
    //Deliberately leak memory. We will clean everything up after the bootstrap is complete.
}

// A silly dict. This is a bootstrap implementation so perhaps we won't optimize this.

enum instruction_type{
    SofiaGoto,
    SofiaReturn,
    SofiaCall,
    PrimitiveCall
};

typedef struct SofiaInstruction
{
    primitiveContainer src;
    enum instruction_type type;
}SofiaInstruction;


primitiveContainer interpret(primitiveContainer src, primitiveContainer context)
{
    SofiaInstruction* srcPtr = (SofiaInstruction*)(src.data.scalar);
    SofiaInstruction* endPtr = srcPtr + src.size;
    while (srcPtr < endPtr)
    {
    switch(srcPtr->type){
    case SofiaGoto:
        srcPtr += lookup_scalar(srcPtr->src);
        break;
    case SofiaReturn:
        return lookup_dict(context,lookup_scalar(srcPtr->src));
        ++srcPtr;
        break;
    case SofiaCall:
        insert_dict(context,lookup_scalar(lookup_dict(srcPtr->src,SOFIA_VAR_KEY)),interpret(lookup_dict(srcPtr->src,SOFIA_SRC),lookup_dict(srcPtr->src,SOFIA_CONTEXT)));
        ++srcPtr;
        break;
    case PrimitiveCall:
        insert_dict(context,lookup_scalar(lookup_dict(srcPtr->src,SOFIA_VAR_KEY)),lookup_dict(srcPtr->src,SOFIA_SRC).data.primitiveFunc(lookup_dict(srcPtr->src,SOFIA_CONTEXT)));
        ++srcPtr;
        break;
    }
    }

    primitiveContainer placeHolder = make_dict();
    return placeHolder;
}

primitiveContainer make_dict(){
    primitiveContainer ans;
    ans.size = 0;
    ans.data.literal = 0;
    return ans;
}


int main(){
    char* SOFIA_MEMORY = (char*)(malloc(32*1024*1024)); //32 MB of memory.
    char* SOFIA_CURRENT_MEMORY = SOFIA_MEMORY;

    primitiveContainer GLOBAL_SOFIA_CONTEXT = make_dict();


    printf("Compilation passed");
    //This is the template for everything.
    getch();
    free(SOFIA_MEMORY);
    return 0;
}
