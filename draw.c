#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "gmath.h"
#include "symtab.h"

/*======== void add_polygon() ==========
Inputs:   struct matrix *surfaces
         double x0
         double y0
         double z0
         double x1
         double y1
         double z1
         double x2
         double y2
         double z2  
Returns: 
Adds the vertices (x0, y0, z0), (x1, y1, z1)
and (x2, y2, z2) to the polygon matrix. They
define a single triangle surface.

04/16/13 13:05:59
jdyrlandweaver
====================*/
void add_polygon( struct matrix *polygons, 
		  double x0, double y0, double z0, 
		  double x1, double y1, double z1, 
		  double x2, double y2, double z2 ) {
  add_point(polygons, x0, y0, z0);
  add_point(polygons, x1, y1, z1);
  add_point(polygons, x2, y2, z2);
}

/*======== void draw_polygons() ==========
Inputs:   struct matrix *polygons
          screen s
          zbuff zbuf
	  struct constants k
	  struct lights *lights
	  int num_lights
Returns: 
Goes through polygons 3 points at a time, drawing 
lines connecting each points to create bounding
triangles. Then fills in triangles using polygon shading model.

04/16/13 13:13:27
jdyrlandweaver
& Henry
====================*/
void draw_polygons( struct matrix *polygons, screen s, zbuff zbuf, struct constants k, struct light *lights, int num_lights) {
  int i,j,b;
  color c, ambient_color, bottom_color, middle_color, top_color;
  int magic_num;
  double xb,yb,zb, xt,yt,zt, xm,ym,zm;
  //The bottom to top x increment (bt), bottom to middle(bm), middle to top(mt)
  double bt_inc,bm_inc,mt_inc;
  //The increment along the z axis
  double bt_z,bm_z,mt_z;
  //The normalized view vector. Only need to calculate once
  double * normal_view=normalize_light(lights[view_vector]);
  //The index of the top,bottom, and middle vertex normals
  int vb,vm,vt;
  struct vertex *vertices=calculate_vertex_normals(polygons);
 //Calculate ambient lighting
  ambient_color.red=k.r[Kambient]*lights[Kambient].c[Lred];
  ambient_color.green=k.g[Kambient]*lights[Kambient].c[Lgreen];
  ambient_color.blue=k.b[Kambient]*lights[Kambient].c[Lblue];
  for( i=0; i < polygons->lastcol-2; i+=3 ) {
    if ( calculate_dot( polygons, i ) < 0 ) {
      c.red=0;
      c.green=0;
      c.blue=0;
      bottom_color.red=0;
      bottom_color.green=0;
      bottom_color.blue=0;
      middle_color.red=0;
      middle_color.green=0;
      middle_color.blue=0;
      top_color.red=0;
      top_color.green=0;
      top_color.blue=0;
      //Figure out which points are the top,bottom and middle
      //Also figure out  top,bottom and middle vertex normals
      xt=polygons->m[0][i];
      yt=polygons->m[1][i];
      zt=polygons->m[2][i];
      vt=i;
      xb=polygons->m[0][i];
      yb=polygons->m[1][i];
      zb=polygons->m[2][i];
      vb=i;
      b=0;
      magic_num=0;
      for (j=1;j<3;j++){
	if (polygons->m[1][i+j] >= yt){
	  xt=polygons->m[0][i+j];
	  yt=polygons->m[1][i+j];
	  zt=polygons->m[2][i+j];
	  vt=i+j;
	  b=j;
	}
      }
      magic_num+=b;
      b=0;
      for (j=1;j<3;j++){
	if (polygons->m[1][i+j] <= yb){
	  xb=polygons->m[0][i+j];
	  yb=polygons->m[1][i+j];
	  zb=polygons->m[2][i+j];
	  vb=i+j;
	  b=j;
	}
      }
      magic_num+=b;
      magic_num=3-magic_num;
      xm=polygons->m[0][i+magic_num];
      ym=polygons->m[1][i+magic_num];
      zm=polygons->m[2][i+magic_num];
      vm=i+magic_num;
    
      //Increments for x
      bt_inc=(xt-xb)/(yt-yb);
      mt_inc=(xt-xm)/(yt-ym);
      bm_inc=(xm-xb)/(ym-yb);

      //Increments for z
      bt_z=(zt-zb)/(yt-yb);
      mt_z=(zt-zm)/(yt-ym);
      bm_z=(zm-zb)/(ym-yb);
      
      //Find the indices of the bottom,top, and middle vertices in the vertices matrix 
      double bvertex[3];
      double mvertex[3];
      double tvertex[3];
      //Use ints to speed search through boolean short circuiting
      int bfound=0;
      int mfound=0;
      int tfound=0;
      for (j=0;j<polygons->lastcol;j++){
	//	printf("Coords: %d %d %d vertex: %f %f %f index:%d\n",vertices[j].c[0],vertices[j].c[1],vertices[j].c[2],vertices[j].n[0],vertices[j].n[1],vertices[j].n[2],j);
	if (!bfound && vertices[j].c[0]==xb && vertices[j].c[1]==yb && vertices[j].c[2]==zb){
	  bvertex[0]=vertices[j].n[0];
	  bvertex[1]=vertices[j].n[1];
	  bvertex[2]=vertices[j].n[2];
	  bfound=1;
	}
	if (!mfound && vertices[j].c[0]==xm && vertices[j].c[1]==ym && vertices[j].c[2]==zb){
	  mvertex[0]=vertices[j].n[0];
	  mvertex[1]=vertices[j].n[1];
	  mvertex[2]=vertices[j].n[2];
	  mfound=1;
	}
	if (!tfound && vertices[j].c[0]==xt && vertices[j].c[1]==yt && vertices[j].c[2]==zt){
	  tvertex[0]=vertices[j].n[0];
	  tvertex[1]=vertices[j].n[1];
	  tvertex[2]=vertices[j].n[2];
	  tfound=1;
	}
	if (tfound && mfound && bfound)
	  j=polygons->lastcol+1;
      }
      
      /* flat shading
      //Calculate diffuse and specular lighting for each point light source
      int l;
      //The normalized surface normal
      double * surface_normal=calculate_surface_normal(polygons,i);
      //The normalized light
      double * normal_light;
      double theta;
      double tmp;
      for (l=num_lights;l>0;l--){
	normal_light=normalize_light(lights[l]);
	theta=diffuse_multiplier(surface_normal,normal_light);
	//Calculate diffuse reflection. Use ternary operator to save lines
	tmp=k.r[Kdiffuse]*lights[l].c[Lred]*theta;
	tmp>0 ? c.red+=tmp : 0;
	tmp=k.g[Kdiffuse]*lights[l].c[Lgreen]*theta;
	tmp>0 ? c.green+=tmp : 0;
	tmp=k.b[Kdiffuse]*lights[l].c[Lblue]*theta;
	tmp>0 ? c.blue+=tmp : 0;
	//Calculate specular reflection.
	theta=specular_multiplier(surface_normal,normal_light,normal_view);
	tmp=k.r[Kspecular]*lights[l].c[Lred]*theta;
	tmp>0 ? c.red+=tmp : 0;
	tmp=k.g[Kspecular]*lights[l].c[Lgreen]*theta;
	tmp>0 ? c.green+=tmp : 0;
	tmp=k.b[Kspecular]*lights[l].c[Lblue]*theta;
	tmp>0 ? c.blue+=tmp : 0;
	free(normal_light);
      }
      free(surface_normal);
      */

      //Calculate diffuse and specular lighting for bottom vertex for each light source
      int l;
      //The normalized light
      double * normal_light;
      double theta;
      double tmp;
      for (l=num_lights;l>0;l--){
	normal_light=normalize_light(lights[l]);
	theta=diffuse_multiplier(bvertex,normal_light);
	//Calculate diffuse reflection. Use ternary operator to save lines
	tmp=k.r[Kdiffuse]*lights[l].c[Lred]*theta;
	tmp>0 ? bottom_color.red+=tmp : 0;
	tmp=k.g[Kdiffuse]*lights[l].c[Lgreen]*theta;
	tmp>0 ? bottom_color.green+=tmp : 0;
	tmp=k.b[Kdiffuse]*lights[l].c[Lblue]*theta;
	tmp>0 ? bottom_color.blue+=tmp : 0;
	//Calculate specular reflection.
	theta=specular_multiplier(bvertex,normal_light,normal_view);
	tmp=k.r[Kspecular]*lights[l].c[Lred]*theta;
	tmp>0 ? bottom_color.red+=tmp : 0;
	tmp=k.g[Kspecular]*lights[l].c[Lgreen]*theta;
	tmp>0 ? bottom_color.green+=tmp : 0;
	tmp=k.b[Kspecular]*lights[l].c[Lblue]*theta;
	tmp>0 ? bottom_color.blue+=tmp : 0;
	free(normal_light);
      }
      //Add in ambient lighting
      bottom_color.red+=ambient_color.red;
      bottom_color.green+=ambient_color.green;
      bottom_color.blue+=ambient_color.blue;
      //Cap color at 255 if it is above that
      bottom_color.red>255 ? bottom_color.red=255 : 0;
      bottom_color.green>255 ? bottom_color.green=255 : 0;
      bottom_color.blue>255 ? bottom_color.blue=255 : 0;

      //Calculate diffuse and specular lighting for middle vertex for each light source
      for (l=num_lights;l>0;l--){
	normal_light=normalize_light(lights[l]);
	theta=diffuse_multiplier(mvertex,normal_light);
	//Calculate diffuse reflection. Use ternary operator to save lines
	tmp=k.r[Kdiffuse]*lights[l].c[Lred]*theta;
	tmp>0 ? middle_color.red+=tmp : 0;
	tmp=k.g[Kdiffuse]*lights[l].c[Lgreen]*theta;
	tmp>0 ? middle_color.green+=tmp : 0;
	tmp=k.b[Kdiffuse]*lights[l].c[Lblue]*theta;
	tmp>0 ? middle_color.blue+=tmp : 0;
	//Calculate specular reflection.
	theta=specular_multiplier(mvertex,normal_light,normal_view);
	tmp=k.r[Kspecular]*lights[l].c[Lred]*theta;
	tmp>0 ? middle_color.red+=tmp : 0;
	tmp=k.g[Kspecular]*lights[l].c[Lgreen]*theta;
	tmp>0 ? middle_color.green+=tmp : 0;
	tmp=k.b[Kspecular]*lights[l].c[Lblue]*theta;
	tmp>0 ? middle_color.blue+=tmp : 0;
	free(normal_light);
      }
      middle_color.red+=ambient_color.red;
      middle_color.green+=ambient_color.green;
      middle_color.blue+=ambient_color.blue;
      //Cap color at 255 if it is above that
      middle_color.red>255 ? middle_color.red=255 : 0;
      middle_color.green>255 ? middle_color.green=255 : 0;
      middle_color.blue>255 ? middle_color.blue=255 : 0;

      //Calculate diffuse and specular lighting for top vertex for each light source
      //printf("top normal %f %f %f\n",tvertex[0],tvertex[1],tvertex[2]);
      for (l=num_lights;l>0;l--){
	normal_light=normalize_light(lights[l]);
	theta=diffuse_multiplier(tvertex,normal_light);
	//Calculate diffuse reflection. Use ternary operator to save lines
	tmp=k.r[Kdiffuse]*lights[l].c[Lred]*theta;
	tmp>0 ? top_color.red+=tmp : 0;
	tmp=k.g[Kdiffuse]*lights[l].c[Lgreen]*theta;
	tmp>0 ? top_color.green+=tmp : 0;
	tmp=k.b[Kdiffuse]*lights[l].c[Lblue]*theta;
	tmp>0 ? top_color.blue+=tmp : 0;
	//Calculate specular reflection.
	theta=specular_multiplier(tvertex,normal_light,normal_view);
	tmp=k.r[Kspecular]*lights[l].c[Lred]*theta;
	tmp>0 ? top_color.red+=tmp : 0;
	tmp=k.g[Kspecular]*lights[l].c[Lgreen]*theta;
	tmp>0 ? top_color.green+=tmp : 0;
	tmp=k.b[Kspecular]*lights[l].c[Lblue]*theta;
	tmp>0 ? top_color.blue+=tmp : 0;
	free(normal_light);
      }
      top_color.red+=ambient_color.red;
      top_color.green+=ambient_color.green;
      top_color.blue+=ambient_color.blue;
      //Cap color at 255 if it is above that
      top_color.red>255 ? top_color.red=255 : 0;
      top_color.green>255 ? top_color.green=255 : 0;
      top_color.blue>255 ? top_color.blue=255 : 0;

      /* For annoying testing
      printf("starting to fill in polyon\nYB:%f YM:%f YT:%f\n",yb,ym,yt);
      printf("c.green:%d c.red:%d c.blue:%d\n",c.green,c.red,c.blue);
      printf("XB:%f XM:%f XT:%f\n",xb,xm,xt);
      printf("bt_inc:%f bm_inc:%f mt_inc:%f\n\n",bt_inc,bm_inc,mt_inc);
      */
      double left_x=xb;
      double right_x=xb;
      double left_z=zb;
      double right_z=zb;
      int passed_middle=0;
      //Color interpolation
      double bm_red,bm_green,bm_blue,mt_red,mt_green,mt_blue;
      if (ym != yb){
	double bm_red=(middle_color.red-bottom_color.red)/(ym-yb);
	double bm_green=(middle_color.green-bottom_color.green)/(ym-yb);
	double bm_blue=(middle_color.blue-bottom_color.blue)/(ym-yb);
      }
      if (ym != yt){
	double mt_red=(middle_color.red-bottom_color.red)/(yt-ym);
	double mt_green=(middle_color.green-bottom_color.green)/(yt-ym);
	double mt_blue=(middle_color.blue-bottom_color.blue)/(yt-ym);
      }
      double bt_red=(top_color.red-bottom_color.red)/(yt-yb);
      double bt_green=(top_color.green-bottom_color.green)/(yt-yb);
      double bt_blue=(top_color.blue-bottom_color.blue)/(yt-yb);
      color left_c,right_c;
      left_c.red=bottom_color.red;
      left_c.green=bottom_color.green;
      left_c.blue=bottom_color.blue;
      right_c.red=bottom_color.red;
      right_c.green=bottom_color.green;
      right_c.blue=bottom_color.blue;
      //printf("Bottom color %d %d %d\n",bottom_color.red,bottom_color.green,bottom_color.blue);
      //printf("Middle color %d %d %d\n",middle_color.red,middle_color.green,middle_color.blue);
      //printf("top color %d %d %d\n",top_color.red,top_color.green,top_color.blue);
      while (yb<yt){
	if (!passed_middle && yb>=ym){
	  right_x=xm;
	  right_z=zm;
	  passed_middle=1;
	  right_c.red=middle_color.red;
	  right_c.green=middle_color.green;
	  right_c.blue=middle_color.blue;
	}
	draw_gouraud_line(left_x,yb,left_z, right_x,yb,right_z, s,zbuf, left_c, right_c);
	left_x+=bt_inc;
	left_z+=bt_z;
	left_c.red+=bt_red;
	left_c.green+=bt_green;
	left_c.blue+=bt_blue;
	if (yb>=ym){
	  right_x+=mt_inc;
	  right_z+=mt_z;
	  right_c.red+=mt_red;
	  right_c.green+=mt_green;
	  right_c.blue+=mt_blue;
	}
	else{
	  right_x+=bm_inc;
	  right_z+=bm_z;
	  right_c.red+=bm_red;
	  right_c.green+=bm_green;
	  right_c.blue+=bm_blue;
	}
	yb+=1;
      }
      /*draw_line( polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 polygons->m[0][i+1],
		 polygons->m[1][i+1],
		 polygons->m[2][i+1],
		 s, zbuf, c);
    draw_line( polygons->m[0][i+1],
		 polygons->m[1][i+1], 
		 polygons->m[2][i+1],
		 polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 s, zbuf, c);
   draw_line( polygons->m[0][i+2],
		 polygons->m[1][i+2],
		 polygons->m[2][i+2],
		 polygons->m[0][i],
		 polygons->m[1][i],
		 polygons->m[2][i],
		 s, zbuf, c);
      */
    }
  }
  free(normal_view);
}


/*======== void add_sphere() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  adds all the points for a sphere with center 
  (cx, cy) and radius r.

  should call generate_sphere to create the
  necessary points

  jdyrlandweaver
  ====================*/
void add_sphere( struct matrix * points, 
		 double cx, double cy, double cz, double r, 
		 int step ) {

  struct matrix * temp;
  int lat, longt;
  int index;
  int num_steps, num_points;
  double px0, px1, px2, px3;
  double py0, py1, py2, py3;
  double pz0, pz1, pz2, pz3;

  num_steps = MAX_STEPS / step;
  num_points = num_steps * (num_steps + 1);
  
  temp = new_matrix( 4, num_points);
  //generate the points on the sphere
  generate_sphere( temp, cx, cy, cz, r, step );

  int latStop, longStop, latStart, longStart;
  latStart = 0;
  latStop = num_steps;
  longStart = 0;
  longStop = num_steps;

  num_steps++;

  for ( lat = latStart; lat < latStop; lat++ ) {
    for ( longt = longStart; longt < longStop; longt++ ) {
      
      index = lat * num_steps + longt;

      px0 = temp->m[0][ index ];
      py0 = temp->m[1][ index ];
      pz0 = temp->m[2][ index ];
      
      px1 = temp->m[0][ (index + num_steps) % num_points ];
      py1 = temp->m[1][ (index + num_steps) % num_points ];
      pz1 = temp->m[2][ (index + num_steps) % num_points ];

      px3 = temp->m[0][ index + 1 ];
      py3 = temp->m[1][ index + 1 ];
      pz3 = temp->m[2][ index + 1 ];

      if (longt != longStop - 1) {
	px2 = temp->m[0][ (index + num_steps + 1) % num_points ];
	py2 = temp->m[1][ (index + num_steps + 1) % num_points ];
	pz2 = temp->m[2][ (index + num_steps + 1) % num_points ];
      }
      else {
	px2 = temp->m[0][ (index + 1) % num_points ];
	py2 = temp->m[1][ (index + 1) % num_points ];
	pz2 = temp->m[2][ (index + 1) % num_points ];
      }

      if (longt != 0)
	add_polygon( points, px0, py0, pz0, px1, py1, pz1, px2, py2, pz2 );
      if (longt != longStop - 1)
	add_polygon( points, px2, py2, pz2, px3, py3, pz3, px0, py0, pz0 );
    }
  }
}

/*======== void generate_sphere() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  sphere with center (cx, cy) and radius r

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_sphere( struct matrix * points, 
		      double cx, double cy, double cz, double r, 
		      int step ) {


  int circle, rotation;
  double x, y, z, circ, rot;

  int rotStart = step * 0;
  int rotStop = MAX_STEPS;
  int circStart = step * 0;
  int circStop = MAX_STEPS;
  
  for ( rotation = rotStart; rotation < rotStop; rotation += step ) {
    rot = (double)rotation / MAX_STEPS;
    for ( circle = circStart; circle <= circStop; circle+= step ) {

      circ = (double)circle / MAX_STEPS;
      x = r * cos( M_PI * circ ) + cx;
      y = r * sin( M_PI * circ ) *
	cos( 2 * M_PI * rot ) + cy;
      z = r * sin( M_PI * circ ) *
	sin( 2 * M_PI * rot ) + cz;

      add_point( points, x, y, z);
    }
  }
}    


/*======== void add_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r1
	    double r2
	    double step  
  Returns: 

  adds all the points required to make a torus
  with center (cx, cy) and radii r1 and r2.

  should call generate_torus to create the
  necessary points

  03/22/12 13:34:03
  jdyrlandweaver
  ====================*/
void add_torus( struct matrix * points, 
		double cx, double cy, double cz, double r1, double r2, 
		int step ) {

  struct matrix * temp;
  int lat, longt;
  int index;
  int num_steps;
  
  num_steps = MAX_STEPS / step;

  temp = new_matrix( 4, num_steps * num_steps );
  //generate the points on the torus
  generate_torus( temp, cx, cy, cz, r1, r2, step );
  int num_points = temp->lastcol;

  int latStop, longtStop, latStart, longStart;
  latStart = 0;
  longStart = 0;
  latStop = num_steps;
  longtStop = num_steps;
  for ( lat = latStart; lat < latStop; lat++ )
    for ( longt = longStart; longt < longtStop; longt++ ) {

      index = lat * num_steps + longt;

      if ( longt != num_steps-1) {
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps+1) % num_points],
		     temp->m[1][(index+num_steps+1) % num_points],
		     temp->m[2][(index+num_steps+1) % num_points],
		     temp->m[0][index+1],
		     temp->m[1][index+1],
		     temp->m[2][index+1] );
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps) % num_points],
		     temp->m[1][(index+num_steps) % num_points],
		     temp->m[2][(index+num_steps) % num_points],
		     temp->m[0][(index+num_steps) % num_points + 1],
		     temp->m[1][(index+num_steps) % num_points + 1],
		     temp->m[2][(index+num_steps) % num_points + 1]);
      }
      else {
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+1) % num_points],
		     temp->m[1][(index+1) % num_points],
		     temp->m[2][(index+1) % num_points],
		     temp->m[0][index+1-num_steps],
		     temp->m[1][index+1-num_steps],
		     temp->m[2][index+1-num_steps] );
	add_polygon( points, temp->m[0][index],
		     temp->m[1][index],
		     temp->m[2][index],
		     temp->m[0][(index+num_steps) % num_points],
		     temp->m[1][(index+num_steps) % num_points],
		     temp->m[2][(index+num_steps) % num_points],
		     temp->m[0][(index+1) % num_points],
		     temp->m[1][(index+1) % num_points],
		     temp->m[2][(index+1) % num_points]);
      }

    }
}

/*======== void generate_torus() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double r
	    double step  
  Returns: 

  Generates all the points along the surface of a 
  tarus with center (cx, cy) and radii r1 and r2

  Adds these points to the matrix parameter

  03/22/12 11:30:26
  jdyrlandweaver
  ====================*/
void generate_torus( struct matrix * points, 
		     double cx, double cy, double cz, double r1, double r2, 
		     int step ) {

  double x, y, z, circ, rot;
  int circle, rotation;

  double rotStart = step * 0;
  double rotStop = MAX_STEPS;
  double circStart = step * 0;
  double circStop = MAX_STEPS;

  for ( rotation = rotStart; rotation < rotStop; rotation += step ) {

    rot = (double)rotation / MAX_STEPS;
    for ( circle = circStart; circle < circStop; circle+= step ) {

      circ = (double)circle / MAX_STEPS;
      x = cos( 2 * M_PI * rot ) *
	( r1 * cos( 2 * M_PI * circ ) + r2 ) + cx;
      y = r1 * sin( 2 * M_PI * circ ) + cy;
      z = sin( 2 * M_PI * rot ) *
	( r1 * cos( 2 * M_PI * circ ) + r2 ) + cz;

      add_point( points, x, y, z );
    }
  }
}

/*======== void add_box() ==========
  Inputs:   struct matrix * points
            double x
	    double y
	    double z
	    double width
	    double height
	    double depth
  Returns: 

  add the points for a rectagular prism whose 
  upper-left corner is (x, y, z) with width, 
  height and depth dimensions.

  jdyrlandweaver
  ====================*/
void add_box( struct matrix * polygons,
	      double x, double y, double z,
	      double width, double height, double depth ) {

  double x2, y2, z2;
  x2 = x + width;
  y2 = y - height;
  z2 = z - depth;

  add_polygon( polygons, 
	       x, y, z, 
	       x, y2, z,
	       x2, y2, z);
  add_polygon( polygons, 
	       x2, y2, z, 
	       x2, y, z,
	       x, y, z);
  //back
  add_polygon( polygons, 
	       x2, y, z2, 
	       x2, y2, z2,
	       x, y2, z2);
  add_polygon( polygons, 
	       x, y2, z2, 
	       x, y, z2,
	       x2, y, z2);
  //top
  add_polygon( polygons, 
	       x, y, z2, 
	       x, y, z,
	       x2, y, z);
  add_polygon( polygons, 
	       x2, y, z, 
	       x2, y, z2,
	       x, y, z2);
  //bottom
  add_polygon( polygons, 
	       x2, y2, z2, 
	       x2, y2, z,
	       x, y2, z);
  add_polygon( polygons, 
	       x, y2, z, 
	       x, y2, z2,
	       x2, y2, z2);
  //right side
  add_polygon( polygons, 
	       x2, y, z, 
	       x2, y2, z,
	       x2, y2, z2);
  add_polygon( polygons, 
	       x2, y2, z2, 
	       x2, y, z2,
	       x2, y, z);
  //left side
  add_polygon( polygons, 
	       x, y, z2, 
	       x, y2, z2,
	       x, y2, z);
  add_polygon( polygons, 
	       x, y2, z, 
	       x, y, z,
	       x, y, z2); 
}
  
/*======== void add_circle() ==========
  Inputs:   struct matrix * points
            double cx
	    double cy
	    double y
	    double step  
  Returns: 


  03/16/12 19:53:52
  jdyrlandweaver
  ====================*/
void add_circle( struct matrix * points, 
		 double cx, double cy, 
		 double r, double step ) {
  
  double x0, y0, x, y, t;
  
  x0 = cx + r;
  y0 = cy;

  for ( t = step; t <= 1; t+= step ) {
    
    x = r * cos( 2 * M_PI * t ) + cx;
    y = r * sin( 2 * M_PI * t ) + cy;
    
    add_edge( points, x0, y0, 0, x, y, 0 );
    x0 = x;
    y0 = y;
  }

  add_edge( points, x0, y0, 0, cx + r, cy, 0 );
}

/*======== void add_curve() ==========
Inputs:   struct matrix *points
         double x0
         double y0
         double x1
         double y1
         double x2
         double y2
         double x3
         double y3
         double step
         int type  
Returns: 

Adds the curve bounded by the 4 points passsed as parameters
of type specified in type (see matrix.h for curve type constants)
to the matrix points

03/16/12 15:24:25
jdyrlandweaver
====================*/
void add_curve( struct matrix *points, 
		double x0, double y0, 
		double x1, double y1, 
		double x2, double y2, 
		double x3, double y3, 
		double step, int type ) {

  double x, y, t;
  struct matrix * xcoefs;
  struct matrix * ycoefs;
  
  //generate the coeficients
  if ( type == BEZIER_MODE ) {
    ycoefs = generate_curve_coefs(y0, y1, y2, y3, BEZIER_MODE);
    xcoefs = generate_curve_coefs(x0, x1, x2, x3, BEZIER_MODE);
  }

  else {
    xcoefs = generate_curve_coefs(x0, x1, x2, x3, HERMITE_MODE);
    ycoefs = generate_curve_coefs(y0, y1, y2, y3, HERMITE_MODE);
  }

  /*
  printf("a = %lf b = %lf c = %lf d = %lf\n", xcoefs->m[0][0],
         xcoefs->m[1][0], xcoefs->m[2][0], xcoefs->m[3][0]);
  */

  for (t=step; t <= 1; t+= step) {
    
    x = xcoefs->m[0][0] * t * t * t + xcoefs->m[1][0] * t * t
      + xcoefs->m[2][0] * t + xcoefs->m[3][0];

    y = ycoefs->m[0][0] * t * t * t + ycoefs->m[1][0] * t * t
      + ycoefs->m[2][0] * t + ycoefs->m[3][0];

    add_edge(points, x0, y0, 0, x, y, 0);
    x0 = x;
    y0 = y;
  }

  free_matrix(xcoefs);
  free_matrix(ycoefs);
}

/*======== void add_point() ==========
Inputs:   struct matrix * points
         int x
         int y
         int z 
Returns: 
adds point (x, y, z) to points and increment points.lastcol
if points is full, should call grow on points
====================*/
void add_point( struct matrix * points, double x, double y, double z) {
  
  if ( points->lastcol == points->cols )
    grow_matrix( points, points->lastcol + 100 );

  points->m[0][points->lastcol] = x;
  points->m[1][points->lastcol] = y;
  points->m[2][points->lastcol] = z;
  points->m[3][points->lastcol] = 1;

  points->lastcol++;
}

/*======== void add_edge() ==========
Inputs:   struct matrix * points
          int x0, int y0, int z0, int x1, int y1, int z1
Returns: 
add the line connecting (x0, y0, z0) to (x1, y1, z1) to points
should use add_point
====================*/
void add_edge( struct matrix * points, 
	       double x0, double y0, double z0, 
	       double x1, double y1, double z1) {
  add_point( points, x0, y0, z0 );
  add_point( points, x1, y1, z1 );
}

/*======== void draw_lines() ==========
Inputs:   struct matrix * points
         screen s
         color c 
Returns: 
Go through points 2 at a time and call draw_line to add that line
to the screen
====================*/
void draw_lines( struct matrix * points, screen s, zbuff zbuf, color c) {

  int i;
 
  if ( points->lastcol < 2 ) {
    
    printf("Need at least 2 points to draw a line!\n");
    return;
  }

  for ( i = 0; i < points->lastcol - 1; i+=2 ) {

    draw_line( points->m[0][i], points->m[1][i], points->m[2][i] ,
	       points->m[0][i+1], points->m[1][i+1], points->m[2][i+1],
	       s, zbuf, c);
    //FOR DEMONSTRATION PURPOSES ONLY
    //draw extra pixels so points can actually be seen    
    /*
    draw_line( points->m[0][i]+1, points->m[1][i], 
	       points->m[0][i+1]+1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]+1, 
	       points->m[0][i+1], points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i], 
	       points->m[0][i+1]-1, points->m[1][i+1], s, c);
    draw_line( points->m[0][i], points->m[1][i]-1, 
	       points->m[0][i+1], points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]+1, 
	       points->m[0][i+1]+1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]+1, 
	       points->m[0][i+1]-1, points->m[1][i+1]+1, s, c);
    draw_line( points->m[0][i]-1, points->m[1][i]-1, 
	       points->m[0][i+1]-1, points->m[1][i+1]-1, s, c);
    draw_line( points->m[0][i]+1, points->m[1][i]-1, 
	       points->m[0][i+1]+1, points->m[1][i+1]-1, s, c);
    */
  } 	       
}


void draw_line(int x0, int y0, double z0, int x1, int y1, double z1, screen s, zbuff zbuf, color c) {
 
  int x, y, d, dx, dy;
  double z,dz;

  x = x0;
  y = y0;
  z = z0;
  
  //swap points so we're always drawing left to right
  if ( x0 > x1 ) {
    x = x1;
    y = y1;
    z = z1;
    x1 = x0;
    y1 = y0;
    z1 = z0;
  }

  //need to know dx and dy for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;

  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {

    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      dz = (z1-z)/abs(x1 - x);
      while ( x <= x1 ) {
	plot(s, zbuf, c, x, y, z);

	if ( d < 0 ) {
	  x = x + 1;
	  d = d + dy;
	  z+=dz;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  d = d + dy - dx;
	  z+=dz;
	}
      }
    }

    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      dz = (z1-z)/abs(y1-y);
      while ( y <= y1 ) {

	plot(s, zbuf, c, x, y, z);
	if ( d > 0 ) {
	  y = y + 1;
	  d = d - dx;
	  z+=dz;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  d = d + dy - dx;
	  z+=dz;
	}
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 

    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
      dz = (z1-z)/abs(x1 - x);
      while ( x <= x1 ) {

	plot(s, zbuf, c, x, y, z);

	if ( d > 0 ) {
	  x = x + 1;
	  d = d + dy;
	  z+=dz;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  d = d + dy + dx;
	  z+=dz;
	}
      }
    }

    //slope < -1: Octant 7 (3)
    else {

      d =  (dy / 2) + dx;
      dz = (z1-z)/abs(y-y1);
      while ( y >= y1 ) {
	
	plot(s, zbuf, c, x, y, z);
	if ( d < 0 ) {
	  y = y - 1;
	  d = d + dx;
	  z+=dz;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  d = d + dy + dx;
	  z+=dz;
	}
      }
    }
  }
}

void draw_gouraud_line(int x0, int y0, double z0, int x1, int y1, double z1, screen s, zbuff zbuf, color left_c, color right_c) {
 
  int x, y, d, dx, dy;
  double z,dz;
  color tmpc;
  int red,green,blue;
  red=0;
  green=0;
  blue=0;

  x = x0;
  y = y0;
  z = z0;
  
  //swap points so we're always drawing left to right
  if ( x0 > x1 ) {
    x = x1;
    y = y1;
    z = z1;
    x1 = x0;
    y1 = y0;
    z1 = z0;
    tmpc.red=right_c.red;
    tmpc.blue=right_c.blue;
    tmpc.green=right_c.green;
    right_c.red=left_c.red;
    right_c.green=left_c.green;
    right_c.blue=left_c.blue;
    left_c.red=tmpc.red;
    left_c.green=tmpc.green;
    left_c.blue=tmpc.blue;
  }
  //Need to know dx and dy for this version
  dx = (x1 - x) * 2;
  dy = (y1 - y) * 2;

  //positive slope: Octants 1, 2 (5 and 6)
  if ( dy > 0 ) {
    //slope < 1: Octant 1 (5)
    if ( dx > dy ) {
      d = dy - ( dx / 2 );
      dz = (z1-z)/abs(x1 - x);
      red = (right_c.red-left_c.red)/abs(x1-x);
      green = (right_c.green-left_c.green)/abs(x1-x);
      blue = (right_c.blue-left_c.blue)/abs(x1-x);
      while ( x <= x1 ) {
	plot(s, zbuf, left_c, x, y, z);

	if ( d < 0 ) {
	  x = x + 1;
	  d = d + dy;
	  z+=dz;
	}
	else {
	  x = x + 1;
	  y = y + 1;
	  d = d + dy - dx;
	  z+=dz;
	}
	left_c.red+=red;
	left_c.green+=green;
	left_c.blue+=blue;
      }
    }
    //slope > 1: Octant 2 (6)
    else {
      d = ( dy / 2 ) - dx;
      dz = (z1-z)/abs(y1-y);
      red = (right_c.red-left_c.red)/abs(y1-y);
      green = (right_c.green-left_c.green)/abs(y1-y);
      blue = (right_c.blue-left_c.blue)/abs(y1-y);
      while ( y <= y1 ) {

	plot(s, zbuf, left_c, x, y, z);
	if ( d > 0 ) {
	  y = y + 1;
	  d = d - dx;
	  z+=dz;
	}
	else {
	  y = y + 1;
	  x = x + 1;
	  d = d + dy - dx;
	  z+=dz;
	}
	left_c.red+=red;
	left_c.green+=green;
	left_c.blue+=blue;
      }
    }
  }

  //negative slope: Octants 7, 8 (3 and 4)
  else { 
    //slope > -1: Octant 8 (4)
    if ( dx > abs(dy) ) {

      d = dy + ( dx / 2 );
      dz = (z1-z)/abs(x1 - x);
      red = (right_c.red-left_c.red)/abs(x1-x);
      green = (right_c.green-left_c.green)/abs(x1-x);
      blue = (right_c.blue-left_c.blue)/abs(x1-x);
      while ( x <= x1 ) {
	plot(s, zbuf, left_c, x, y, z);

	if ( d > 0 ) {
	  x = x + 1;
	  d = d + dy;
	  z+=dz;
	}
	else {
	  x = x + 1;
	  y = y - 1;
	  d = d + dy + dx;
	  z+=dz;
	}
	left_c.red+=red;
	left_c.green+=green;
	left_c.blue+=blue;
      }
    }

    //slope < -1: Octant 7 (3)
    else {
      d =  (dy / 2) + dx;
      dz = (z1-z)/abs(y-y1);
      red = ((double)(right_c.red-left_c.red))/abs(y-y1);
      green = ((double)(right_c.green-left_c.green))/abs(y-y1);
      blue = ((double)(right_c.blue-left_c.blue))/abs(y-y1);
      while ( y >= y1 ) {
	
	plot(s, zbuf, left_c, x, y, z);
	if ( d < 0 ) {
	  y = y - 1;
	  d = d + dx;
	  z+=dz;
	}
	else {
	  y = y - 1;
	  x = x + 1;
	  d = d + dy + dx;
	  z+=dz;
	}
	left_c.red+=red;
	left_c.green+=green;
	left_c.blue+=blue;
      }
    }
  }
}

