#ifndef SVGSHAPEHELPERS_H
#define SVGSHAPEHELPERS_H

#include "SVGParser.h"

void init_svg_rectangle(Rectangle *rect);
void init_svg_circle(Circle *circ);
void init_svg_path(Path *path);
void init_svg_rectangle_with_area(Rectangle *rect, float area);
void init_svg_circle_with_area(Circle *circ, float area);
void init_svg_path_with_data(Path *path, char *data);

void parse_rectangle_attribute_list(Rectangle *rect, List *attributes);
void parse_circle_attribute_list(Circle *circle, List *attributes);
void parse_path_attribute_list(Path *path, List *attributes);
void parse_group_attribute_list(Group *group, List *attributes);
void parse_svg_attribute_list(SVGimage *img, List *attributes);
float calc_rectangle_area(Rectangle *rect);
float calc_circle_area(Circle *circ);
bool compare_rectangle_area(const void *rect1, const void *rect2);
bool compare_circle_area(const void *circ1, const void *circ2);
bool compare_paths(const void *path1, const void *path2);
bool compare_attribute_name(const void *attr1, const void *attr2);
int group_length(Group *grp);
int num_attributes_in_rectangle(Rectangle *rect);
int num_attributes_in_circle(Circle *circ);
int num_attributes_in_path(Path *path);
void read_number_and_unit(char *input, float *number, char *unit);

#endif

