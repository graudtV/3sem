/*  These definitions must NOT be called from client
 * code directly. Use interface from cvector.h */

#ifndef CVECTOR_DETAILS_H_
#define CVECTOR_DETAILS_H_

/* General structure of vectors of all types */
struct _generic_cvector_ {
	void *data;
	size_t size;
	size_t capacity;
};

int _cvec_reserve_one_place(void *vector, size_t elem_sz);
int _cvec_reserve(void *vector, size_t new_capacity, size_t elem_sz);
void _cvec_destroy(void *vector);

#define _assert_not_const(var)		\
	{ typeof((var) = (var)) VARIABLE_IS_CONST; }

#define _assert_is_modifiable(vecname) \
	_assert_not_const((vecname)->data[0])

#define _assert_is_owner(vecname) \
	_assert_not_const(*(vecname))

/* Transforms const size_t to size_t */
#define _modify_ *(size_t *) &

#endif // CVECTOR_DETAILS_H_