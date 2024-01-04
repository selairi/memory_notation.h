# Common problems and solutions

If you get a memory leak or segment fault in your code, welcome. Those are the typical problems and solutions about memory management in C.

Sorry but the only way to solve it is to check those errors in your code.

# Problems

- [Return a variable stored on the stack](#VariableStoredOnStack)
- [Double free](#DoubleFree)
- [Use a released pointer](#ReleasedPointer)
- [Increase a pointer and release](#FreeIncreasedPointer)
- [Variables are deleted at the end of the block](#DeletedVariables)
- [Array out of bounds](#ArrayOutOfBounds)
- [Wrong casting](#WrongCast)
- Memory leaks
    - [Free after return](#FreeAfterReturn)
    - [Forget releasing memory](#ForgetFree)
    - [free() instead the specialized function](#ForgetSpecialized)


## Return a variable stored on the stack <a name='VariableStoredOnStack'></a>

Variables are deleted at the end of de block { }.

Wrong code:

```C
    int *function1() {
        int a = 2;
        return &a;
        // a is deleted at this point
    }
```

Solution:

```C
    memory_take_possession int *function1() {
        int *a = (int*)malloc(sizeof(int));
        *a = 2;
        return a;
    }
```

Or:

```C
    void function1(memory_guarded memory_ptr_out int *a) {
        *a = 2;
    }

    // ...

    int a;
    function1(&a);
```

## Double free <a name='DoubleFree'></a>

Release an already released pointer. Example:

Wrong code:

```C
    char *a = strdup("Hello world");
    // ...
    free(a);
    // ...
    free(a); // Error. 
```

Solution:

Remove the double free or set the pointer to NULL after free():

```C
    char *a = strdup("Hello world");
    // ...
    free(a);
    a = NULL;
    // ...
    free(a); // Error. 
```

## Use a released pointer <a name='ReleasedPointer'></a>


Try to use a pointer that has been released.

Wrong code:

```C
    char *pointer = strdup("Hello world");
    // ...
    free(pointer);
    // ...
    printf("%s\n", pointer); // Error. 
```

Solutions:

1. Use reference counting to control memory cycle of life.
2. Remove previous free(pointer).
3. Set pointer to NULL after free(pointer) and check if(pointer == NULL).
4. Use **memory_keep_alive** and **memory_release_after_of(mem)** tags.

## Increase a pointer and release <a name='FreeIncreasedPointer'></a>

Pointers can be increased or decreased with ++ or --. The original position cannot be found if pointer is increased or decreased.

Wrong code:

```C
    char *pointer = strdup("Hello world");
    // ...
    pointer++;
    // ...
    free(pointer); // Error: Pointer is not in the original position
```

Solution:

If you are going to increase or decrease a pointer, use a copy:

```C
    char *_pointer = strdup("Hello world");
    char *p = NULL;
    p = _pointer memory_owner_of(p);
    // ...
    p++;
    // ...
    free(_pointer); 
```
    
## Variables are deleted at the end of the block <a name='DeletedVariables'></a>


Wrong code:

```C
    void function() {
        int *a;
        {
            int b = 3;
            a = &b;
            // b is deleted at this point
        }
        *a = 2; // Error: b has been released
    }
```

Solution:

Copy values or use "malloc":

```C
    void function() {
        int c;
        int *a = &c;
        {
            int b = 3;
            *a = b;
        }
        *a = 2;    
    }
```

## Array out of bounds <a name='ArrayOutOfBounds'></a>

Wrong code:

```C
    int a[3];
    a[3] = 2; // Error: array positions are 0, 1 and 2.
```

Solution:

Check array boundaries or use an object to get or set array elements:

```C
    // Object that controls an array
    struct Array {
        int *private_array memory_owner;
        char *private_name memory_owner;
        int private_size;
    }

    struct Array *array_new(int size, memory_take_possession char *name) {
        struct Array *array = (struct Array*)malloc(sizeof(struct Array));
        array->private_size = size;
        array->private_name = name;
        array->private_array = (int *)malloc(sizeof(int)*size);
        return array;
    }

    int array_get(memory_guarded struct Array *array, int index) {
        if(index >= 0 && index < array->private_size) {
            return array->private_array[index];
        }
        fprintf("Error reading array %s. index %d size %d\n", array->private_name, index, array->private_size);
        exit(1);
        return -1;
    }

    int array_set(memory_guarded struct Array *array, int index, int value) {
        if(index >= 0 && index < array->private_size) {
            array->private_array[index] = value;
        }
        fprintf("Error writting array %s. index %d size %d\n", array->private_name, index, array->private_size);
        exit(1);
    }

    // ...
    struct Array *a = array_new(3, strdup("a"));
    array_set(a, 2, 2);
    struct Array *b = array_new(3, strdup("b"));
    array_set(b, 3, 2); // Program stops with a message warning.
```

## Wrong casting <a name='WrongCast'></a>

C lets cast from every type with no compiler error.

Wrong code:

```C
    struct Example {
        int a;
        char b;
    };
    int a;
    struct Example *p = (struct Example *) &a;
    p->b; // Error: p refers to "a" and "a" is an "int" no a "struct Example".
```

Solution:

Check types before cast.
    
## Memory leaks

Memory which is no longer needed is not released.

### Free after return <a name='FreeAfterReturn'></a>

Wrong code:

```C
    void function6(int a) {
        int *b = (int *)malloc(sizeof(int));
        *b = a;
        if(a < 10) {
            // Function returns and b is not released.
            return;
        }
        free(b);
    }
```

Solution:

1. Check the code and remember to release resources before all "return" and check cycle of life of each pointer.
2. Use [the cleanup attribute](http://omeranson.github.io/blog/2022/06/12/cleanup-attribute-in-C).

The cleanup attribute example:

```C
    void array_free(int **p) {
        free(*p);
        *p = NULL;
    }
      
    void function6(int a) {
        __attribute__((cleanup(array_free))) int *b = (int *)malloc(sizeof(int));
        *b = a;
        if(a < 10) {
            return;
        }
    }
```

### Forget releasing memory <a name='ForgetFree'></a>

Wrong code:

```C
    int main(void) {
        int *a = (int *)malloc(sizeof(int));
        // ...
        return 0;
        // a is not released.
    }
```

Solution:

1. Insert free() before end of cycle of life of the pointer.
2. Use [the cleanup attribute](http://omeranson.github.io/blog/2022/06/12/cleanup-attribute-in-C) on variable declaration.
3. Use reference counting.

### free() instead the specialized function <a name='ForgetSpecialized'></a>

Some objects has got their specialized functions to release it. 

Wrong code:

```C
    struct String {
        char *str memory_owner;
        int size;
    };

    struct String *string_new(char *str memory_take_possession) {
        struct String *s = (struct String*)malloc(sizeof(struct String));
        s->str = str; s->size = strlen(str);
        return s;
    }
    
    // Specialized function to release memory: 
    void string_free(struct String *s memory_take_possession) {
        free(s->str);
        free(s);
    }

    // ... Code ...

    struct String *s = string_new(strdup("Hello world"));
    // ...
    free(s); // Memory leak: s->str is not released
    // string_free(s); // is the right function.
```

Solution:

Check objects and check is they have got a function to release memory or reference counting.

