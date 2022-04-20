#ifndef SVGHELPERS_H
#define SVGHELPERS_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlwriter.h>
#include <libxml/xmlschemastypes.h>

#include "SVGParser.h"

typedef enum {
    CT_Unknown = 0,
    CT_Svg,
    CT_Title,
    CT_Desc,
    CT_Rect,
    CT_Circle,
    CT_Path,
    CT_Group
} ContentType;

void print_element_names(xmlNode * a_node);

void initialize_svg_strings(SVGimage *svg);
void initialize_svg_lists(SVGimage *svg);
void initialize_lists(Group *grp);
void parse_xml_tree(xmlNode *root, SVGimage *img, Group *grp);
void read_string(xmlNode *node, char *dest, int length);
void read_namespace(xmlNode *node, SVGimage *img);
List *read_attributes(xmlNode *node);
Attribute *copy_attribute(Attribute *a);
Attribute *create_attribute_text(char *name, char *content);
char *append_float_attribute_to_string(char *str, char *name, float value);
char *append_string_attribute_to_string(char *str, char *name, char *value);
char *append_string_to_string(char *str, char *prefix, char *str2, char *postfix);
List *get_rectangles(Group *grp);
List *get_circles(Group *grp);
List *get_paths(Group *grp);
List *get_groups(Group *grp);
int num_rects_with_area_in_group(Group *grp, float area);
int num_circles_with_area_in_group(Group *grp, float area);
int num_paths_with_data_in_group(Group *grp, char* data);
int num_groups_with_length_in_group(Group *grp, int len);
int num_attributes_in_group(Group *grp);

#endif

