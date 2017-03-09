#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "ml6.h"
#include "display.h"
#include "draw.h"
#include "matrix.h"
#include "parser.h"

#define NEXTDOUBLE(line) atof(strsep(&line, " ")) //next double in a line
#define REMOVENL(line) if(line[strlen(line) - 1] == '\n') line[strlen(line) - 1] = '\0' //removes new line in a string read from fgets


/*======== void parse_file () ==========
Inputs:   char * filename 
          struct matrix * transform, 
          struct matrix * pm,
          screen s
Returns: 

Goes through the file named filename and performs all of the actions listed in that file.
The file follows the following format:
     Every command is a single character that takes up a line
     Any command that requires arguments must have those arguments in the second line.
     The commands are as follows:
   line: add a line to the edge matrix - 
	    takes 6 arguemnts (x0, y0, z0, x1, y1, z1)
	 ident: set the transform matrix to the identity matrix - 
	 scale: create a scale matrix, 
	    then multiply the transform matrix by the scale matrix - 
	    takes 3 arguments (sx, sy, sz)
	 translate: create a translation matrix, 
	    then multiply the transform matrix by the translation matrix - 
	    takes 3 arguments (tx, ty, tz)
	 rotate: create an rotation matrix,
	    then multiply the transform matrix by the rotation matrix -
	    takes 2 arguments (axis, theta) axis should be x y or z
	 apply: apply the current transformation matrix to the 
	    edge matrix
	 display: draw the lines of the edge matrix to the screen
	    display the screen
	 save: draw the lines of the edge matrix to the screen
	    save the screen to a file -
	    takes 1 argument (file name)
	 quit: end parsing

See the file script for an example of the file format


IMPORTANT MATH NOTE:
the trig functions int math.h use radian mesure, but us normal
humans use degrees, so the file will contain degrees for rotations,
be sure to conver those degrees to radians (M_PI is the constant
for PI)
====================*/
void parse_file ( char * filename, 
                  struct matrix * transform, 
                  struct matrix * edges,
                  screen s) {
  
  FILE *f;
  char line[256];
  clear_screen(s);

  if(strcmp(filename, "stdin") == 0) f = stdin;
  else f = fopen(filename, "r");

  while(fgets(line, 255, f) != NULL) {
    REMOVENL(line);

    if(strcmp(line, "line") == 0)
    {
      //read coordinates
      fgets(line, 255, f); REMOVENL(line);
      //**************************
      char* coords = line;

      double x0 = NEXTDOUBLE(coords);
      double y0 = NEXTDOUBLE(coords);
      double z0 = NEXTDOUBLE(coords);
      double x1 = NEXTDOUBLE(coords);
      double y1 = NEXTDOUBLE(coords);
      double z1 = NEXTDOUBLE(coords);
      add_edge(edges, x0, y0, z0, x1, y1, z1);
    }
    else if(strcmp(line, "ident") == 0)
    {
      ident(transform);
    }
    else if(strcmp(line, "scale") == 0)
    {
      //read scale factors
      fgets(line, 255, f); REMOVENL(line);
      //**************************
      char* factors = line;
      double sx = NEXTDOUBLE(factors);
      double sy = NEXTDOUBLE(factors);
      double sz = NEXTDOUBLE(factors);

      //makes scale matrix and multiply
      struct matrix* scaleMat = make_scale(sx, sy, sz);
      matrix_mult(scaleMat, transform);
      free_matrix(scaleMat);
    }
    else if(strcmp(line, "move") == 0)
    {
      fgets(line, 255, f); REMOVENL(line);
      //**************************
      char* factors = line;
      double tx = NEXTDOUBLE(factors);
      double ty = NEXTDOUBLE(factors);
      double tz = NEXTDOUBLE(factors);

      //make trans matrix and multiply
      struct matrix* transMat = make_translate(tx, ty, tz);
      matrix_mult(transMat, transform);
      free_matrix(transMat);
    }
    else if(strcmp(line, "rotate") == 0)
    {
      fgets(line, 255, f); REMOVENL(line);
      //**************************
      char* rotateArgs = line;

      char rotType = *(strsep(&rotateArgs, " "));
      double degreeMeasure = NEXTDOUBLE(rotateArgs);

      struct matrix* m;
      if(rotType == 'x') m = make_rotX(degreeMeasure);
      else if(rotType == 'y') m = make_rotY(degreeMeasure);
      else m = make_rotZ(degreeMeasure);

      matrix_mult(m, transform);
      free_matrix(m);
    }
    else if(strcmp(line, "apply") == 0)
    {
      matrix_mult(transform, edges);
    }
    else if(strcmp(line, "display") == 0)
    {
      color c; c.red = 255; c.green = 0; c.blue = 0;
      draw_lines(edges, s, c);
      display(s);
    }
    else if(strcmp(line, "save") == 0)
    {
      //read argument, file name
      fgets(line, 255, f); REMOVENL(line);
      char* pictureFile = line;

      color c; c.red = 255; c.green = 0; c.blue = 0;
      draw_lines(edges, s, c);
      save_extension(s, pictureFile);
    }
    else if(strcmp(line, "quit") == 0) break;
  }
}
  
