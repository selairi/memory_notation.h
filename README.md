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

The tags defined by "memory_notation.h" are:

-**memory_guarded**
-**memory_owner**
-**memory_take_possession**
-**memory_keep_alive**
-**memory_release_after_of(mem)**
-**memory_owner_of(mem)**
-**memory_ref_count**
-**memory_ptr_inout**
-**memory_ptr_out**

**WARNING**: This is only a notation. All tags are ignored by the compiler.

# How to use it

Copy "src/memory_notation.h" file to your project and include it.

"memory_notation.h" controls two cases:

1. Memory refereed by a pointer has an owner that controls its cycle of life.
2. Memory refereed by a pointer has a reference counting system or garbage collector.

## 1. For memory owner method
For memory owner method use tags:
- **memory_guarded**: Memory has an owner then don't free it.
- **memory_owner**: This object is the owner of memory refereed by the pointer.
- **memory_take_possession**: The memory refereed by the pointer, has no owner. You have to control the cycle of life of that memory.

## 2. For reference counting system or garbage collector
For reference counting system or garbage collector use "**memory_ref_count**" tag. Example:

    memory_ref_count Object *strange_function();
    void object_ref(memory_ref_count Object *o); // Must be run when Object pointer is gotten.
    void object_unref(memory_ref_count Object *o); // Must be run when Object pointer is released.

    // ... Code ...

    Object *o = strange_function();
    object_ref(o);
    // Use o
    object_unref(o);

# Examples

Memory owner method example:

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


Reference counting example:

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

# **memory_take_possession**

When is refereed to a pointer returned by a function, it means that you have to control the cycle of life of that pointer:

    memory_take_possession char *strdup(memory_guarded const char *str); 

But when is refereed to a function argument means that the function will control the cycle of life of that pointer:

    void function5(memory_take_possession int *a) {
        // Use a
        free(a);
    }

# **memory_keep_alive** and **memory_release_after_of(mem)**

There are objects that need other objects while they are alive.

- **memory_keep_alive**: The pointer must not be freed until the present object finishes.

Imagine that you are using an image library that renders a final photo. The object that renders the final photo could need other images. Those images could not be freed until the final image is rendered. 

Example:

    struct Image { ... };
    struct ImageBuilder { ... };
    void image_builder_add_resource(
        struct ImageBuilder *builder memory_guarded, 
        struct Image *resource memory_keep_alive);
    
    // ...
    
    struct Image *img = image_new_from_file("file.png");
    struct ImageBuilder *builder = image_builder_new();
    image_builder_add_resource(builder, img memory_release_after_of(builder));
    // free(img); -> Error the image is need until render
    image_builder_render(builder);
    free(img);


# Returning values as function arguments

Sometimes function arguments are used to return values:
    
    void function3(int a, int *b, int *c) {
        *b *= 2;
        *c = a + *b;
    }

Then these tags must be used:
   
- **memory_ptr_out**: Function argument is used to return values. 
- **memory_ptr_inout**: Function argument is used to give values to the function and return values. 

In the example:

    void function3(
        int a, 
        int *b memory_guarded memory_ptr_inout, 
        int *c memory_guarded memory_ptr_out 
    ) {
        *c = a + *b;
    }

More examples:

    void function4(
        int a,
        int *b memory_take_possession memory_ptr_out
    ) {
        b = (int *)malloc(sizeof(int)*a);
    }


# memory_owner_of(mem)

Sometimes several pointers can be refereed to the same memory. The code can be complicated and **memory_owner_of(mem)** could help:

    int *b;
    int a = 2 memory_owner_of(b);
    b = &a;


# Short version

All tags are self explained, but is a long notation:

- memory_guarded
- memory_owner
- memory_take_possession
- memory_owner_of(mem)
- memory_ref_count
- memory_ptr_inout
- memory_ptr_out

There are a short version of all tags:

- m_g
- m_o
- m_t
- m_o_(mem)
- m_rc
- m_io
- m_out

# Common problems and solutions

Please read [PROBLEMS.md](PROBLEMS.md) file.
