#ifndef SVGVALIDATION_H
#define SVGVALIDATION_H

#include <libxml/parser.h>
#include <libxml/tree.h>
#include <libxml/encoding.h>
#include <libxml/xmlschemastypes.h>

#include <SVGParser.h>

bool validate_SVG_against_schema (xmlDocPtr doc, char *schemaFile);
bool validate_SVG_contents (SVGimage *img);
bool validate_rectangle (Rectangle *rect);
bool validate_rectangle_list (List *rectangles);
bool validate_circle (Circle *circ);
bool validate_circle_list (List *circles);
bool validate_path (Path *path);
bool validate_path_list (List *paths);
bool validate_group (Group *group);
bool validate_group_list (List *groups);
bool validate_attribute (Attribute *attr);
bool validate_attribute_list (List *attributes);

#endif

