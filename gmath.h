#ifndef GMATH_H
#define GMATH_H

#include "matrix.h"
#include "symtab.h"

struct vertex
{
  //c holds the vertex's coordinates, n stores the vector
  int c[3];
  double n[3];
};

double * calculate_normal( double a1, double a2, double a3,
			   double b1, double b2, double b3 );
double calculate_dot( struct matrix *points, int i );
double * calculate_surface_normal( struct matrix *points, int i );
double * calculate_surface_non_normalized( struct matrix *points, int i );
double diffuse_multiplier(double *normal, double *light);
double specular_multiplier(double *normal, double *light, double *view);
double * normalize_light(struct light light);
struct vertex * calculate_vertex_normals( struct matrix *points);

#endif
