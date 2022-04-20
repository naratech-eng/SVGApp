#ifndef XMLGENERATION_H
#define XMLGENERATION_H

#include <libxml/tree.h>

#include <SVGParser.h>

xmlDocPtr createDocument(SVGimage *img);
void addSVGImage(xmlDocPtr doc, SVGimage *img);
void addAttributes(xmlNodePtr node, List *attributes);
void addRectangles(xmlNodePtr node, List *rectangles);
void addCircles(xmlNodePtr node, List *circles);
void addPaths(xmlNodePtr node, List *paths);
void addGroups(xmlNodePtr node, List *groups);
void addFloatProperty(xmlNodePtr node, char *name, float value, char *units);

#endif

