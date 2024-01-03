# Common problems and solutions

If you get a memory leak or segment fault in your code, welcome. Those are the typical problems and solutions about memory management in C.

Sorry but the only way to solve it is to check those errors in your code.

# Problems

## Return a variable stored on the stack

Variables are deleted at the end of de block { }.

Wrong code:

    int *function1() {
        int a = 2;
        return &a;
        // a is deleted at this point
    }

Solution:

    memory_take_possession int *function1() {
        int *a = (int*)malloc(sizeof(int));
        *a = 2;
        return a;
    }

Or better choice, because the stack is faster than "malloc":

    void function1(memory_guarded memory_ptr_out int *a) {
        *a = 2;
    }

    // ...

    int a;
    function1(&a);

## Double free

Release an already released pointer. Example:

Wrong code:

    char *a = strdup("Hello world");
    // ...
    free(a);
    // ...
    free(a); // Error. 

Solution:

Remove the double free or set the pointer to NULL after free():

    char *a = strdup("Hello world");
    // ...
    free(a);
    a = NULL;
    // ...
    free(a); // Error. 

## Use a released pointer

Try to use a pointer that has been released.

Wrong code:

    char *pointer = strdup("Hello world");
    // ...
    free(pointer);
    // ...
    printf("%s\n", pointer); // Error. 

Solutions:

1. Use reference counting to control memory cycle of life.
2. Remove previous free(pointer).
3. Set pointer to NULL after free(pointer) and check if(pointer == NULL).

## Increase a pointer and release

Pointers can be increased or decreased with ++ or --. The original position cannot be found if pointer is increased or decreased.

Wrong code:

    char *pointer = strdup("Hello world");
    // ...
    pointer++;
    // ...
    free(pointer); // Error: Pointer is not in the original position

Solution:

If you are going to increase or decrease a pointer, use a copy:

    char *_pointer = strdup("Hello world");
    char *p = NULL;
    p = _pointer memory_owner_of(p);
    // ...
    p++;
    // ...
    free(_pointer); 
    
    
## Variables are deleted at the end of the block

Wrong code:

    void function() {
        int *a;
        {
            int b = 3;
            a = &b;
            // b is deleted at this point
        }
        *a = 2; // Error: b has been released
    }

Solution:

Copy values or use "malloc":

    void function() {
        int c;
        int *a = &c;
        {
            int b = 3;
            *a = b;
        }
        *a = 2;    
    }

## Array out of bounds

Wrong code:

    int a[3];
    a[3] = 2; // Error: array positions are 0, 1 and 2.

Solution:

Check array boundaries or use and object to get or set array elements:

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
    array_set(b, 3, 2); // Program stops with a warning.


