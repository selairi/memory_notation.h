# memory_notation.h

This simple header provides a notation to aid in memory control in the C language.

Imagine the following situation:

You have to use the following function from a C library:

    int *function();

This function returns a pointer. Do you have to free this pointer?

    int *p = function();
    // ... Code ...
    free(p); // ??

"memory_notation.h" suggest a simple notation to fix this problem:

    memory_take_possession int *function1();
    memory_guarded int *function2();

    // ...

    int *p1 = function1();
    int *p2 = function2();
    // ... Code ...
    free(p1);
    // p2 will be freed in other place. Don't worry.

# How to use it

Copy "src/memory_notation.h" file to your project and include it.

"memory_notation.h" controls two cases:

1. Memory refereed by a pointer has an owner that controls its cycle of life.
2. Memory refereed by a pointer has a reference counting system or garbage collector.

## For memory owner method
For memory owner method use tags:
- **memory_guarded**: Memory has an owner then don't free it.
- **memory_owner**: This object is the owner of memory refereed by the pointer.
- **memory_take_possession**: The memory refereed by the pointer, has no owner. You have to control the cycle of life of that memory.

## For reference counting system or garbage collector
For reference counting system or garbage collector use "**memory_ref_count**" tag. Example:

    memory_ref_count Object *strange_function();
    void object_ref(memory_ref_count Object *o);
    void object_unref(memory_ref_count Object *o);

    // ... Code ...

    Object *o = strange_function();


    struct RefCount {
        int count;
        memory_owner Object *obj;
    };
    
    void refcount_ref(memory_ref_count struct RefCount *p) {
        p->count++;
    }

    void refcount_unref(memory_ref_count struct RefCount *p) {
        p->count--;
        if(p->count <= 0) { 
            free(p->obj);
            p->obj = NULL;
        }
    }

    memory_ref_count struct RefCount *refcount_new(Object *o) {
        struct RefCount *p = (struct RefCount*)malloc(sizeof(struct RefCount));
        p->count = 1;
        p->obj = o;
        return p;
    }

    memory_ref_count struct RefCount *strange_function();
    
    // ... Code ...
    
    struct RefCount *p = strange_function();
    refcount_ref(p);
    // Use p
    refcount_unref(p);

# Example

Example:

    struct Example {
    	memory_guarded char *name; // This pointer must not be freed by this object
    	memory_owner   char *id;     // This object must be freed by this object
    };
    
    memory_take_possession struct Example *example_new(
        memory_guarded        char *name, 
        memory_take_possession char *id
    ) 
    {
    	struct Example *ex = (struct Example *)malloc(sizeof(struct Example));
    	ex->name = name; ex->id = id;
    	return ex;
    }
    
    void example_delete(memory_owner struct Example *ex) {
    	free(ex->id);
    	free(ex);
    }
    
    memory_guarded char *example_id(memory_guarded struct Example *ex) {
    	return ex->id;
    }
    
    int main() {
      const char *name = "Hello";
      struct Example *ex = example_new((char*)name, strdup("World"));
      printf("%s\n", example_id(ex));
      example_delete(ex);
    }
