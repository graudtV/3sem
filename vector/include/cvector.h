/* Implementation of vector container on C */

#ifndef CVECTOR_H_
#define CVECTOR_H_

#include <stdlib.h>
#include "cvector_details.h"

#define CVECTOR_OK 0
#define CVECTOR_BADALLOC 1

/*  Allocates vector of arbitrary type.
 *  Warning! Using vector, which was not initialized with a
 * cvec_create() upon creation or with cvec_init(), is undefined behaviour!
 * Each created vector should be destroyed with cvec_destroy */
#define cvec_create() \
	calloc(1, sizeof (struct _generic_cvector_))

/*  Destroys vector of arbitrary type */
void cvec_destroy(void *vector);

/*  vector_of(type) can be used in any place, where a declaration
 * of a vector is required, i.e. to create a vector in a function,
 * to use it as a field in structure or to pass as an argument
 * to function. This type can be safely copied. All such objects
 * will refer to the same underlying vector. Thus, vector_of(type)
 * can and should be passed to functions without pointers.*/
#define cvector_of(type) _cvector_##type##_

/*  Defines a structure, which will be used for operations
 * with vector. Must be called in any file, which uses vector */
#define USING_CVECTOR(type)							\
	typedef struct {								\
		type * const data;							\
		const size_t size;							\
		const size_t capacity;						\
	} * const cvector_of(type);						\

/* returns CVECTOR_OK if success, error code otherwise */
#define cvec_push_back(vecname, value) ({						\
		int _vec_ret_status = _cvec_reserve_one_place(			\
			vecname, sizeof(vecname->data[0]));					\
		if (_vec_ret_status == CVECTOR_OK) {					\
			(vecname)->data[(vecname)->size] = (value);			\
			++ (_modify_ (vecname)->size);						\
		}														\
		_vec_ret_status;										\
	})

#define cvec_reserve(vecname, new_capacity)			\
	_cvec_reserve(vecname, new_capacity, sizeof(vecname->data[0]))

/* Iterators */
#define cvec_begin(vecname)		((vecname)->data)
#define cvec_end(vecname)		((vecname)->data + (vecname)->size)

#ifdef CVECTOR_SINGLE_FILE
#include "cvector.c"
#endif

#endif // CVECTOR_H_