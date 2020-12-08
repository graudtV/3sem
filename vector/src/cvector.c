/* Implementation of some methods for general vector */

#include "cvector.h"

/* new_capacity - number of elements */
static int _cvec_expand(void *vector, size_t new_capacity, size_t elem_sz)
{
	struct _generic_cvector_ *vec = vector;
	void *new_data;
	if (!(new_data = realloc(vec->data, new_capacity * elem_sz)))
		return CVECTOR_BADALLOC;
	vec->data = new_data;
	vec->capacity = new_capacity;
	return CVECTOR_OK;
}

int _cvec_reserve_one_place(void *vector, size_t elem_sz)
{
	struct _generic_cvector_ *vec = vector;
	if (vec->size == vec->capacity)
		return _cvec_expand(vec, (vec->size * 2 + 1), elem_sz);
	return CVECTOR_OK;
}

int _cvec_reserve(void *vector, size_t new_capacity, size_t elem_sz)
{
	struct _generic_cvector_ *vec = vector;
	if (vec->capacity < new_capacity)
		return _cvec_expand(vec, new_capacity, elem_sz);
	return CVECTOR_OK;
}

void cvec_destroy(void *vector)
{
	struct _generic_cvector_ *vec = vector;
	free(vec->data);
	free(vec);
}