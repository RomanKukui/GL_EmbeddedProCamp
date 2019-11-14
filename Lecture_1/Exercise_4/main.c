#include <stdio.h>
#include <stdlib.h>

/// pi constant
#define PI 3.14159265359

/// Radius of circle(meters), may be tuned in other conditions
#define RADIUS 6

/// Circle perimeter calculation macro
#define CIRCLE_PERIMETER(r) (2 * PI * r)

/// Circle area calculation macro
#define CIRCLE_AREA(r) (PI * r * r)

int main()
{
	printf("Circle perimeter (R = %0.2f m):\t%.2fm\n",
		(float)RADIUS, CIRCLE_PERIMETER(RADIUS));

        printf("Circle area (R = %0.2fm):\t%.2fm^2\n",
		(float)RADIUS, CIRCLE_AREA(RADIUS));

	return 0;
}
