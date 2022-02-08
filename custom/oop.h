#ifndef CUSTOM_OOP_H
#define CUSTOM_OOP_H

#include <stddef.h>

//takes an embedded field pointer and converts to a pointer to the encapsulating object
//performs no type-checking to remain portable
#define getSuper(parent_type, field_name, field_ptr)\
    ((parent_type *)( (char *)(field_ptr) - offsetof(parent_type, field_name) ))

#endif