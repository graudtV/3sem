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

	cvector_view(int) v = cvec_viewer(numbers);
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

cvector_own(int) generate_vec(int nvalues, int init)
{
	cvector_own(int) numbers = cvec_create();
	fill_with_conseq_numbers(cvec_modifier(numbers), nvalues, init);
	return numbers;
}

int main()
{
	cvector_own(int) numbers = cvec_create(); // creates empty vector

	fill_with_conseq_numbers(cvec_modifier(numbers), 10, 0);
	print_int_vector(cvec_viewer(numbers)); // 0 1 2 3 4 5 6 7 8 9
	
	cvector_own(int) vals = generate_vec(5, 7);
	print_int_vector(cvec_viewer(vals));
	cvec_destroy(vals);

	/* C++ style */
	print_int_vector_by_range(cvec_begin(numbers), cvec_end(numbers)); // 0 1 2 3 4 5 6 7 8 9

	cvec_destroy(numbers);

	return 0;
}