#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "matrix.h"
#include "gmath.h"
#include "symtab.h"


/*======== double * calculate_normal() ==========
  Inputs:   double ax
            double ay
	    double az
	    double bx
	    double by
	    double bz  
  Returns: A double arry of size 3 representing the 
           cross product of <ax, ay, az> and <bx, by, bz>

  04/17/12 16:46:30
  jonalf
  ====================*/
double * calculate_normal( double ax, double ay, double az,	
			   double bx, double by, double bz ) {
  
  double *normal;
  normal = (double *)malloc(3 * sizeof(double));

  normal[0] = ay*bz - az*by;
  normal[1] = az*bx - ax*bz;
  normal[2] = ax*by - ay*bx;

  return normal;
}

/*======== double calculate_dot() ==========
  Inputs:   struct matrix *points
            int i  
  Returns: The dot product of a surface normal and
           a view vector
  
  calculates the dot product of the surface normal to
  triangle points[i], points[i+1], points[i+2] and a 
  view vector (use <0, 0, -1> to start.

  04/17/12 16:38:34
  jonalf
  ====================*/
double calculate_dot( struct matrix *points, int i ) {

  double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );

  //set up view vector
  vx = 0;
  vy = 0;
  vz = -1;

  //calculate dot product
  dot = normal[0] * vx + normal[1] * vy + normal[2] * vz;

  free(normal);  
  return dot;
}

/*======== double calculate_surface_normal() ==========
  Inputs:   struct matrix *points
            int i  
  Returns: double *
     The normalized surface normal of the polygon
  
  Calculates the normalized surface normal of triangle points[i], points[i+1], points[i+2] 

  05/31/16 20:38:34
  Henry
  ====================*/
double * calculate_surface_normal( struct matrix *points, int i ) {

  double ax, ay, az, bx, by, bz;
  double *normal;
  double vx, vy, vz;
  double dot;

  //calculate A and B vectors
  ax = points->m[0][i+1] - points->m[0][i];
  ay = points->m[1][i+1] - points->m[1][i];
  az = points->m[2][i+1] - points->m[2][i];

  bx = points->m[0][i+2] - points->m[0][i];
  by = points->m[1][i+2] - points->m[1][i];
  bz = points->m[2][i+2] - points->m[2][i];

  //get the surface normal
  normal = calculate_normal( ax, ay, az, bx, by, bz );
  double normal_magnitude=sqrt(pow(normal[0],2) + pow(normal[1],2) + pow(normal[2],2));
  normal[0]=normal[0]/normal_magnitude;
  normal[1]=normal[1]/normal_magnitude;
  normal[2]=normal[2]/normal_magnitude;
  return normal;
}

/*======== double diffuse_multiplier() ==========
  Inputs:   double *normal
            double *lights
  Returns: The value by which the constant of diffuse reflection and 
   the color of the light are to be multiplied by to get diffuse reflection.
   IMPORTANT: The vectors must be normalized!
  
  05/31/16 20:38:34
  Henry
  ====================*/
double diffuse_multiplier(double *normal, struct light light){
  double light_magnitude=sqrt(pow(light.l[x_vector],2) + pow(light.l[y_vector],2) + pow(light.l[z_vector],2));
  double dot_product = normal[0]*(light.l[0]/light_magnitude) + normal[1]*(light.l[1]/light_magnitude) + normal[2]*(light.l[2]/light_magnitude);
  printf("light_magnitude: %f\n",light_magnitude);
  return dot_product;
}

double * normalize_light(struct light light){
  double *new_light;
  new_light = (double *)malloc(3 * sizeof(double));
  n
}

/*======== double specular_multiplier() ==========
  Inputs:   double *normal
            double *lights
	    double *view
  Returns: The value by which the constant of diffuse reflection and 
   the color of the light are to be multiplied by to get specular reflection.
   IMPORTANT: The vectors must be normalized!
  
  06/2/16 13:37:34
  Henry
  ====================*/
double ambient_multiplier(double *normal, struct light light, struct light view){
  
  double dot_product = normal[0]*(light.l[0]/light_magnitude) + normal[1]*(light.l[1]/light_magnitude) + normal[2]*(light.l[2]/light_magnitude);
  printf("light_magnitude: %f\n",light_magnitude);
  return dot_product;
}
