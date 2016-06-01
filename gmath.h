#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"
#include "symtab.h"

double * calculate_normal( double a1, double a2, double a3,
			   double b1, double b2, double b3 );
double calculate_dot( struct matrix *points, int i );
double * calculate_surface_normal( struct matrix *points, int i );
double diffuse_multiplier(double *normal, struct light light);

#endif
