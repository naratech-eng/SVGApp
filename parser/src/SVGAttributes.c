#define _POSIX_C_SOURCE = 200809L
#define _GNU_SOURCE

#include <SVGParser.h>
#include <SVGShapeHelpers.h>
#include <SVGAttributes.h>

void setAttribute(SVGimage *image, elementType elemType, int elemIndex, Attribute *newAttribute) {
    Rectangle *rectangle = NULL;
    Circle *circle = NULL;
    Path *path = NULL;
    Group *group = NULL;

    if ((image != NULL) && (newAttribute != NULL)) {
        switch (elemType) {
        case SVG_IMAGE:
            setAttributeSVGimage(image, newAttribute);
            break;
        case RECT:
            rectangle = getRectangleFromImage(image, elemIndex);

            if (rectangle != NULL) {
                setAttributeRectangle(rectangle, newAttribute);
            }
            break;
        case CIRC:
            circle = getCircleFromImage(image, elemIndex);

            if (circle != NULL) {
                setAttributeCircle(circle, newAttribute);
            }
            break;
        case PATH:
            path = getPathFromImage(image, elemIndex);

            if (path != NULL) {
                setAttributePath(path, newAttribute);
            }
            break;
        case GROUP:
            group = getGroupFromImage(image, elemIndex);

            if (group != NULL) {
                setAttributeGroup(group, newAttribute);
            }
            break;
        default:
            /* should not happen */
            break;
        }
    }
}

void setAttributeSVGimage(SVGimage *image, Attribute *newAttribute) {
    if ((image != NULL) && (image->otherAttributes != NULL) && (newAttribute != NULL)) {
        setAttributeHelper(image->otherAttributes, newAttribute);
    }
}

Rectangle *getRectangleFromImage(SVGimage *image, int rectangleIndex) {
    Rectangle *r = NULL;
    int i = 0;
    ListIterator iter;

    if ((image != NULL) && (image->rectangles != NULL)) {
        if (rectangleIndex < getLength(image->rectangles)) {
            iter = createIterator(image->rectangles);

            /* cycle through all list elements and leave the
             * loop at the correct index */
            while ((r = (Rectangle *)nextElement(&iter)) != NULL) {
                if (i == rectangleIndex) {
                    break;
                }
                i++;
            }
        }
    }

    return r;
}

Circle *getCircleFromImage(SVGimage *image, int circleIndex) {
    Circle *c = NULL;
    int i = 0;
    ListIterator iter;

    if ((image != NULL) && (image->circles != NULL)) {
        if (circleIndex < getLength(image->circles)) {
            iter = createIterator(image->circles);

            /* cycle through all list elements and leave the
             * loop at the correct index */
            while ((c = (Circle *)nextElement(&iter)) != NULL) {
                if (i == circleIndex) {
                    break;
                }
                i++;
            }
        }
    }

    return c;
}

Path *getPathFromImage(SVGimage *image, int pathIndex) {
    Path *p = NULL;
    int i = 0;
    ListIterator iter;

    if ((image != NULL) && (image->paths != NULL)) {
        if (pathIndex < getLength(image->paths)) {
            iter = createIterator(image->paths);

            /* cycle through all list elements and leave the
             * loop at the correct index */
            while ((p = (Path *)nextElement(&iter)) != NULL) {
                if (i == pathIndex) {
                    break;
                }
                i++;
            }
        }
    }

    return p;
}

Group *getGroupFromImage(SVGimage *image, int groupIndex) {
    Group *g = NULL;
    int i = 0;
    ListIterator iter;

    if ((image != NULL) && (image->groups != NULL)) {
        if (groupIndex < getLength(image->groups)) {
            iter = createIterator(image->groups);

            /* cycle through all list elements and leave the
             * loop at the correct index */
            while ((g = (Group *)nextElement(&iter)) != NULL) {
                if (i == groupIndex) {
                    break;
                }
                i++;
            }
        }
    }

    return g;
}


void setAttributeRectangle(Rectangle *rect, Attribute *newAttribute) {
    if ((rect != NULL) && (newAttribute != NULL)) {
        /* special fields */
        if (!strcmp(newAttribute->name, "x")) {
            read_number_and_unit(newAttribute->value, &rect->x, NULL);
            deleteAttribute(newAttribute);
        } else if (!strcmp(newAttribute->name, "y")) {
            read_number_and_unit(newAttribute->value, &rect->y, NULL);
            deleteAttribute(newAttribute);
        } else if (!strcmp(newAttribute->name, "width")) {
            read_number_and_unit(newAttribute->value, &rect->width, NULL);
            deleteAttribute(newAttribute);
        } else if (!strcmp(newAttribute->name, "height")) {
            read_number_and_unit(newAttribute->value, &rect->height, NULL);
            deleteAttribute(newAttribute);
        } else {
            setAttributeHelper(rect->otherAttributes, newAttribute);
        }
    }
}

void setAttributeCircle(Circle *circ, Attribute *newAttribute) {
    if ((circ != NULL) && (newAttribute != NULL)) {
        /* special fields */
        if (!strcmp(newAttribute->name, "cx")) {
            read_number_and_unit(newAttribute->value, &circ->cx, NULL);
            deleteAttribute(newAttribute);
        } else if (!strcmp(newAttribute->name, "cy")) {
            read_number_and_unit(newAttribute->value, &circ->cy, NULL);
            deleteAttribute(newAttribute);
        } else if (!strcmp(newAttribute->name, "r")) {
            read_number_and_unit(newAttribute->value, &circ->r, NULL);
            deleteAttribute(newAttribute);
        } else {
            setAttributeHelper(circ->otherAttributes, newAttribute);
        }
    }
}

void setAttributePath(Path *path, Attribute *newAttribute) {
    if ((path != NULL) && (newAttribute != NULL)) {
        /* special treatment for "d" attribute */
        if (!strcmp(newAttribute->name, "d")) {
            if (path->data != NULL) {
                free(path->data);
            }

            path->data = strdup(newAttribute->value);
            deleteAttribute(newAttribute);
        } else {
            setAttributeHelper(path->otherAttributes, newAttribute);
        }
    }
}

void setAttributeGroup(Group *group, Attribute *newAttribute) {
    if ((group != NULL) && (group->otherAttributes != NULL) && (newAttribute != NULL)) {
        setAttributeHelper(group->otherAttributes, newAttribute);
    }
}

void setAttributeHelper(List *attributes, Attribute *newAttribute) {
    void *attr = NULL;

    if ((attributes != NULL) && (newAttribute != NULL)) {
        attr = findElement(attributes, compare_attribute_name, newAttribute);   

        if (attr != NULL) {
            /* attribute was found -> update it */
            free(((Attribute *)attr)->value);
            ((Attribute *)attr)->value = strdup(newAttribute->value);
            deleteAttribute(newAttribute);
        } else {
            /* attribute was not found -> add it to list */
            insertBack(attributes, newAttribute);
        }
    }
}

void addComponent(SVGimage* image, elementType elemType, void * newComponent) {
    if ((image != NULL) && (newComponent != NULL)) {
        switch (elemType) {
        case SVG_IMAGE:
            break;
        case RECT:
            insertBack(image->rectangles, (Rectangle *)newComponent);
            break;
        case CIRC:
            insertBack(image->circles, (Circle *)newComponent);
            break;
        case PATH:
            insertBack(image->paths, (Path *)newComponent);
            break;
        case GROUP:
            insertBack(image->groups, (Group *)newComponent);
            break;
        default:
            /* should not happen */
            break;
        }
    }
}

void setSVGTitle(char *filename, char *title) {
    SVGimage *image = NULL;

    image = createSVGimage(filename);

    if ((image != NULL) && (title != NULL)) {
        /* the new title is trimmed here */
        strncpy(image->title, title, 255);
        /* terminate string, in case strncpy didn't do that */
        title[255] = 0x00;

        writeSVGimage(image, filename);
        deleteSVGimage(image);
    }
}

void setSVGDescription(char *filename, char *desc) {
    SVGimage *image = NULL;

    image = createSVGimage(filename);

    if ((image != NULL) && (desc != NULL)) {
        /* the new description is trimmed here */
        strncpy(image->description, desc, 255);
        /* terminate string, in case strncpy didn't do that */
        desc[255] = 0x00;

        writeSVGimage(image, filename);
        deleteSVGimage(image);
    }
}


