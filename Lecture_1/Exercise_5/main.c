/// @file
#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>

/// Factorial base
#define FACTORIAL_BASE 10

/*!
 * Calculate factorial of \b n in cycle (not recursion).
 *
 * @param[in]	n Factorial base in range 0-20.
 * @return	\retval 0 if error occurs
 *		\retval n! if all ok
 */
 uint64_t factorial(uint8_t n)
 {
 	uint64_t tmp = 1;
 	if (n == 0) {
		goto _exit;	// 0! = 1
 	}

	if (n > 20) {		// uint64 may overload
		tmp = 0;
		goto _exit;
	}

 	for (uint8_t i = 2; i <= n; i++) {
		tmp *= i;
		// printf("i: %d - %I64u\n", i, tmp);
 	}

_exit:
 	return tmp;
 }

int main()
{
	printf("\t%d! = %I64u\n",
		FACTORIAL_BASE, factorial(FACTORIAL_BASE));
	return 0;
}
