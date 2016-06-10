#ifndef DISPLAY_H
#define DISPLAY_H

void plot( screen s, zbuff zbuf, color c, int x, int y, double z);
void double_plot( screen s, zbuff zbuf,double_color double_c, int x, int y, double z); 
void clear_screen( screen s);
void save_ppm( screen s, char *file);
void save_extension( screen s, char *file);
void display( screen s);
void clear_zbuff(zbuff z);

color change_color( int i );
#endif
