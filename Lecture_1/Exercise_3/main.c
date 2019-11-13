/// @file

#include <stdio.h>
#include <stdlib.h>

/// Convert meters to inches
#define MTOI(m) (m/0.0254)

int main()
{
	printf("Please enter measurement of rectangle\n");
	printf("! Separate height and width with space !\n");
	printf("! Use . (dot) for float value !\n:");

	float a = 0;
	float b = 0;
	scanf("%f %f", &a, &b);

	printf("Perimeter, inches: %.2f\n", ((MTOI(a) + MTOI(b)) * 2));
	printf("Area, inches^2: %.2f\n", (MTOI(a) * MTOI(b)));

	return 0;
}
