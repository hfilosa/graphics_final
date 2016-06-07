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
            double *light
  Returns: The value by which the constant of diffuse reflection and 
   the color of the light are to be multiplied by to get diffuse reflection.
   IMPORTANT: The vectors must be normalized!
  
  05/31/16 20:38:34
  Henry
  ====================*/
double diffuse_multiplier(double *normal, double *light){
  double dot_product = normal[0]*light[0] + normal[1]*light[1] + normal[2]*light[2];
  return dot_product;
}

/*======== double normalize_light() ==========
  Inputs:  struct light
  Returns: The normalized light vector.
    Can also be used to normalize view vector.
  
  06/2/16 18:44:34
  Henry
  ====================*/
double * normalize_light(struct light light){
  double *new_light;
  new_light = (double *)malloc(3 * sizeof(double));
  double light_magnitude=sqrt(pow(light.l[x_vector],2) + pow(light.l[y_vector],2) + pow(light.l[z_vector],2));
  new_light[0]=light.l[0]/light_magnitude;
  new_light[1]=light.l[1]/light_magnitude;
  new_light[2]=light.l[2]/light_magnitude;
  return new_light;
}

/*======== double specular_multiplier() ==========
  Inputs:   double *normal
            double *light
	    double *view
  Returns: The value by which the constant of diffuse reflection and 
   the color of the light are to be multiplied by to get specular reflection.
   IMPORTANT: The vectors must be normalized!
  
  06/2/16 13:37:34
  Henry
  ====================*/
double specular_multiplier(double *normal, double *light, double *view){
  double dot_product = normal[0]*light[0] + normal[1]*light[1] + normal[2]*light[2];  
  double tmp[3];
  tmp[0]=(2*dot_product*normal[0]) - light[0];
  tmp[1]=(2*dot_product*normal[1]) - light[1];
  tmp[2]=(2*dot_product*normal[2]) - light[2];
  //We now have to normalize this tmp vector
  //The tmp vector is really the vector of reflection
  //When in doubt normalize!
  double mag=sqrt(pow(tmp[0],2) + pow(tmp[1],2) + pow(tmp[2],2));
  tmp[0]=tmp[0]/mag;
  tmp[1]=tmp[1]/mag;
  tmp[2]=tmp[2]/mag;
  dot_product = tmp[0]*view[0] + tmp[1]*view[1] + tmp[2]*view[2];
  //Sharpening factor. High values will make reflections sharper. Arbitrary value
  int sharp=4;
  dot_product=pow(dot_product,3);
  return dot_product;
}

/*======== double calculate_vertex_normal() ==========
  Inputs:   struct matrix *points

  Returns: struct matrix *
     matrix with 6 rows. rows[0-2] are coordinates
       rows[3-5] hold the corresponding normalized vertix normal
  

  05/31/16 20:38:34
  Henry
  ====================*/
struct vertex * calculate_vertex_normals( struct matrix *points) {

  double ax, ay, az, bx, by, bz;
  double vx, vy, vz;
  double dot;
  double *tmp1;

  struct vertex *unsorted=(vertex *)malloc(points->cols*sizeof(vertex));
  int i,j;
  for (i=0;i<points->lastcol;i+=3){
    tmp1=calculate_surface_normal(points,i);
    for (j=i;j<i+3;j++){
      unsorted[j]->c[0]=points->m[0][j];
      unsorted[j]->c[1]=points->m[1][j];
      unsorted[j]->c[2]=points->m[2][j];
      unsorted[j]->n[0]=tmp1[0];
      unsorted[j]->n[1]=tmp1[1];
      unsorted[j]->n[2]=tmp1[2];
    }
    free(tmp1);
  }
  //sort the vertices struct
  struct vertex * sorted=(vertex *)malloc(points->cols*sizeof(vertex));
  int present;
  double tmp[3];
  int index=0;
  double magnitude;
  for (i=0;i<points->cols;i++){
    //zero out tmp array
    tmp[0]=0;
    tmp[1]=0;
    tmp[2]=0;
    //Check if already in sorted
    present=0;
    for (j=0;j<index;j++){
      if (sorted[j]->c[0] == unsorted[i]->c[0] && sorted[j]->c[1] == unsorted[i]->c[1] && sorted[j]->c[2] == unsorted[i]->c[2]){
	present=1;
	j=sorted->lastcol+1;
      }
    }
    if (!present){
      //Add in the coordinates
      sorted[index]->c[0]=unsorted[i]->c[0];
      sorted[index]->c[1]=unsorted[i]->c[1];
      sorted[index]->c[2]=unsorted[i]->c[2];
      //calculate the vertex normal
      for (j=i;j<points->cols;j++){
	if (sorted[index]->c[0] == unsorted[j]->c[0] && sorted[index]->c[1] == unsorted[j]->c[1] && sorted[index]->c[2] == unsorted[j]->c[2]){
	  tmp[0]+=unsorted[j]->n[0];
	  tmp[1]+=unsorted[j]->n[1];
	  tmp[2]+=unsorted[j]->n[2];
	}
      }
      magnitude=sqrt(pow(tmp[0],2)+pow(tmp[1],2)+pow(tmp[2],2));
      tmp[0]=tmp[0]/magnitude;
      tmp[1]=tmp[1]/magnitude;
      tmp[2]=tmp[2]/magnitude;
      sorted[index]->n[0]=tmp[0];
      sorted[index]->n[1]=tmp[1];
      sorted[index]->n[2]=tmp[2];
    }
  }
  return sorted;
}
