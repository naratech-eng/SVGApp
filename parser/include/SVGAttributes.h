#ifndef SVGATTRIBUTES_H
#define SVGATTRIBUTES_H

#include <SVGParser.h>

void setAttributeSVGimage(SVGimage *image, Attribute *newAttribute);
void setAttributeHelper(List *attributes, Attribute *newAttribute);
void setAttributeRectangle(Rectangle *rectangle, Attribute *newAttribute);
void setAttributeCircle(Circle *circle, Attribute *newAttribute);
void setAttributePath(Path *path, Attribute *newAttribute);
void setAttributeGroup(Group *group, Attribute *newAttribute);
Rectangle *getRectangleFromImage(SVGimage *image, int rectangleIndex);
Circle *getCircleFromImage(SVGimage *image, int circleIndex);
Path *getPathFromImage(SVGimage *image, int pathIndex);
Group *getGroupFromImage(SVGimage *image, int groupIndex);

#endif

