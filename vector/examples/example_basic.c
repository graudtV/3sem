/* Basic cvector example */

#include <stdio.h>

#define CVECTOR_SINGLE_FILE
#include "../include/cvector.h"

USING_CVECTOR(int);
// USING_CVECTOR(char), etc - any vectors you need

void fill_with_conseq_numbers(cvector_modify(int) numbers, int nvalues, int init)
{
	cvec_reserve(numbers, nvalues); // reducing number of reallocations
	for (int i = init; i < nvalues + init; ++i)
		cvec_push_back(numbers, i);
}

void print_int_vector(cvector_view(int) numbers)
{
	for (int i = 0; i < numbers->size; ++i)
		printf("%d ", numbers->data[i]);
	printf("\n");
}

void print_int_vector_by_range(const int *fst, const int *last)
{
	while (fst != last)
		printf("%d ", *fst++);
	printf("\n");
}

/* Generates vector with numbers from fst to last (including);
 *  Ownership is transferred to caller. Thus, caller must destroy returned
 * vector himself. Note. If vector was passed anywhere with owner rights,
 * it cannot be used no more in the current place */
cvector_own(int) generate_vec(int fst, int last)
{
	cvector_own(int) numbers = cvec_create();
	cvec_reserve(numbers, last - fst + 1)
	for (int i = fst; i <= last; ++i)
		cvec_push_back(numbers, i);
	return numbers;
}

int main()
{
	cvector_own(int) numbers = cvec_create(); // creates empty vector

	fill_with_conseq_numbers(cvec_get_modifier(numbers), 10, 0);
	print_int_vector(cvec_get_viewer(numbers)); // 0 1 2 3 4 5 6 7 8 9

	/* C++ style */
	print_int_vector_by_range(cvec_begin(numbers), cvec_end(numbers)); // 0 1 2 3 4 5 6 7 8 9

	cvec_destroy(numbers);


	cvector_own(int) vals = generate_vec(5, 12); // passing vector as a return value
	print_int_vector(cvec_get_viewer(vals)); // 5 6 7 8 9 10 11 12 
	cvec_destroy(vals); // don't forget do destroy!

	return 0;
}