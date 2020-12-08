/* Basic cvector example */

#include <stdio.h>

#define CVECTOR_SINGLE_FILE
#include "../vector/cvector.h"

USING_CVECTOR(int);
// USING_CVECTOR(char), etc - any vectors you need

void fill_with_conseq_numbers(cvector_of(int) numbers, int nvalues, int init)
{
	cvec_reserve(numbers, nvalues); // reducing number of reallocations
	for (int i = init; i < nvalues + init; ++i)
		cvec_push_back(numbers, i);
}

void print_int_vector(cvector_of(int) numbers)
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

int main()
{
	cvector_of(int) numbers = cvec_create(); // creates empty vector

	fill_with_conseq_numbers(numbers, 10, 0);
	print_int_vector(numbers); // 0 1 2 3 4 5 6 7 8 9

	/* C++ style */
	print_int_vector_by_range(cvec_begin(numbers), cvec_end(numbers)); // 0 1 2 3 4 5 6 7 8 9

	cvec_destroy(numbers);
	return 0;
}