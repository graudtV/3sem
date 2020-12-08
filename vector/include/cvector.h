/* Implementation of vector container on C */

#ifndef CVECTOR_H_
#define CVECTOR_H_

#include <stdlib.h>
#include "cvector_details.h"

#define CVECTOR_OK 0
#define CVECTOR_BADALLOC 1

/* TODO: rewrite
 *  Allocates vector of arbitrary type.
 *  Warning! Using vector, which was not initialized with a
 * cvec_create() upon creation or with cvec_init(), is undefined behaviour!
 * Each created vector should be destroyed with cvec_destroy */
#define cvec_create() \
	calloc(1, sizeof (struct _generic_cvector_))

/*  Destroys vector of arbitrary type */
#define cvec_destroy(vecname) {													\
		_assert_is_owner(vecname) /* ERR: CANNOT DELETE: NOT OWNING ACCESS */	\
		_cvec_destroy((void *) (vecname));										\
	}	

/* There are three types, which can be used to control cvector
 *  - vector_own(type) is a type for vector owner. Using it, vector can
 * be initialized, modified and destroyed.
 *  - vector_view(type) can be used for not owning access with
 * only read permissions. Is is similar to const lvalue references in C++
 * or std::string_view. Trying to modify or destroy it will cause compilation
 * error.
 *  - vector_modify(type) can be used for not owning access with both
 * read and write permissions. It is similar to non-const lvalue references
 * in C++. Trying to destroy it will cause compilation error. */
#define cvector_own(type) _cvector_##type##_
#define cvector_view(type) _cvector_##type##_view_
#define cvector_modify(type) _cvector_##type##_not_owning_modify_

/* Returns an object with not owning read-only access to existing vector */
#define cvec_get_viewer(vecname)	\
	((void *) (vecname))

/* Returns an object with not owning read and write access to existing vector */
#define cvec_get_modifier(vecname) ({															\
		_assert_is_owner(vecname) /* ERR: YOU HAVE ONLY VIEW ACCESS, CANNOT GET MODIFIER */	\
		((void *) (vecname));																\
	})

/*  Defines a structure, which will be used for operations
 * with vector. Must be called in any file, which uses vector */
#define USING_CVECTOR(type)								\
	typedef struct {									\
		type * const data;								\
		const size_t size;								\
		const size_t capacity;							\
	} * const cvector_own(type);						\
														\
	typedef struct {									\
		const type * const data;						\
		const size_t size;								\
		const size_t capacity;							\
	} const * const cvector_view(type);					\
														\
	typedef struct {									\
		type * const data;								\
		const size_t size;								\
		const size_t capacity;							\
	} const * const cvector_modify(type);					

/* returns CVECTOR_OK if success, error code otherwise */
#define cvec_push_back(vecname, value) ({									\
		_assert_is_modifiable(vecname);	/* ERR: VECTOR IS NOT MODIFIABLE */	\
		int _vec_ret_status = _cvec_reserve_one_place(						\
			(void *) vecname, sizeof(vecname->data[0]));					\
		if (_vec_ret_status == CVECTOR_OK) {								\
			(vecname)->data[(vecname)->size] = (value);						\
			++ (_modify_ (vecname)->size);									\
		}																	\
		_vec_ret_status;													\
	})

#define cvec_reserve(vecname, new_capacity) {										\
		_assert_is_modifiable(vecname) /* ERR: VECTOR IS NOT MODIFIABLE */			\
		_cvec_reserve((void *) vecname, new_capacity, sizeof(vecname->data[0]));	\
	}

#define cvec_clear(vecname)	{													\
		_assert_is_modifiable(vecname) /* ERR: VECTOR IS NOT MODIFIABLE */		\
		(_modify_ (vecname)->size) = 0;											\
	}

/* Iterators */
#define cvec_begin(vecname)		((vecname)->data)
#define cvec_end(vecname)		((vecname)->data + (vecname)->size)

#ifdef CVECTOR_SINGLE_FILE
#include "../src/cvector.c"
#endif

#endif // CVECTOR_H_