#define _POSIX_C_SOURCE = 200809L
#define _GNU_SOURCE

#include <SVGParser.h>
#include <SVGHelpers.h>
#include <SVGShapeHelpers.h>

#include <LinkedListAPI.h>

#include <math.h>

/* private functions */
char *getStrValueByName(const char *str, const char *name);
float getFloatValueByName(const char *str, const char *name);

char *attrToJSON(const Attribute *a) {
    char *json = NULL;
    int length = 0;

    if ((a != NULL) && (a->name != NULL) && (a->value != NULL)) {
        /* determine length of output string */
        length = snprintf(NULL, 0, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);

        /* allocate for length + 1 (trailing 0x00) */
        json = calloc(length+1,sizeof(char));

        /* create output string */
        if (json != NULL) {
            snprintf(json, length+1, "{\"name\":\"%s\",\"value\":\"%s\"}", a->name, a->value);
        }
    } else {
        json = strdup("{}");
    }

    return json;
}

char *circleToJSON(const Circle *c) {
    char *json = NULL;
    int length = 0;
    int numAttr = 0;
    char *attrList = NULL;

    if (c != NULL) {
        /* determine number of other attributes */
        numAttr = getLength(c->otherAttributes);
        attrList = attrListToJSON(c->otherAttributes);

        /* determine length of output string */
        length = snprintf(NULL, 0, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\",\"other\":%s}", c->cx, c->cy, c->r, numAttr, c->units, attrList);

        /* allocate for length + 1 (trailing 0x00) */
        json = calloc(length+1,sizeof(char));

        /* create output string */
        if (json != NULL) {
            snprintf(json, length+1, "{\"cx\":%.2f,\"cy\":%.2f,\"r\":%.2f,\"numAttr\":%d,\"units\":\"%s\",\"other\":%s}", c->cx, c->cy, c->r, numAttr, c->units, attrList);
        }

        free(attrList);
    } else {
        json = strdup("{}");
    }

    return json;
}

char *rectToJSON(const Rectangle *r) {
    char *json = NULL;
    int length = 0;
    int numAttr = 0;
    char *attrList = NULL;

    if (r != NULL) {
        /* determine number of other attributes */
        numAttr = getLength(r->otherAttributes);
        attrList = attrListToJSON(r->otherAttributes);

        /* determine length of output string */
        length = snprintf(NULL, 0, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\",\"other\":%s}", r->x, r->y, r->width, r->height, numAttr, r->units, attrList);

        /* allocate for length + 1 (trailing 0x00) */
        json = calloc(length+1,sizeof(char));

        /* create output string */
        if (json != NULL) {
            snprintf(json, length+1, "{\"x\":%.2f,\"y\":%.2f,\"w\":%.2f,\"h\":%.2f,\"numAttr\":%d,\"units\":\"%s\",\"other\":%s}", r->x, r->y, r->width, r->height, numAttr, r->units, attrList);
        }

        free(attrList);
    } else {
        json = strdup("{}");
    }

    return json;
}

char *pathToJSON(const Path *p) {
    char *json = NULL;
    int length = 0;
    int numAttr = 0;
    char *attrList = NULL;

    if (p != NULL) {
        /* determine number of other attributes */
        numAttr = getLength(p->otherAttributes);
        attrList = attrListToJSON(p->otherAttributes);

        /* determine length of output string */
        length = snprintf(NULL, 0, "{\"d\":\"%s\",\"numAttr\":%d,\"other\":%s}", p->data, numAttr, attrList);

        /* allocate for length + 1 (trailing 0x00) */
        json = calloc(length+1,sizeof(char));

        /* create output string */
        if (json != NULL) {
            snprintf(json, length+1, "{\"d\":\"%s\",\"numAttr\":%d,\"other\":%s}", p->data, numAttr, attrList);
        }

        free(attrList);
    } else {
        json = strdup("{}");
    }

    return json;
}

char *groupToJSON(const Group *g) {
    char *json = NULL;
    int length = 0;
    int numAttr = 0;
    int children = 0;
    char *attrList = NULL;

    if (g != NULL) {
        /* determine number of children */
        children += getLength(g->rectangles);
        children += getLength(g->circles);
        children += getLength(g->paths);
        children += getLength(g->groups);

        /* determine number of other attributes */
        numAttr = getLength(g->otherAttributes);
        attrList = attrListToJSON(g->otherAttributes);

        /* determine length of output string */
        length = snprintf(NULL, 0, "{\"children\":%d,\"numAttr\":%d,\"other\":%s}", children, numAttr, attrList);

        /* allocate for length + 1 (trailing 0x00) */
        json = calloc(length+1,sizeof(char));

        /* create output string */
        if (json != NULL) {
            snprintf(json, length+1, "{\"children\":%d,\"numAttr\":%d,\"other\":%s}", children, numAttr, attrList);
        }

        free(attrList);
    } else {
        json = strdup("{}");
    }

    return json;
}

char* SVGtoJSON(const SVGimage* image) {
    char *json = NULL;
    int length = 0;
    int numR = 0;
    int numC = 0;
    int numP = 0;
    int numG = 0;
    List *list = NULL;

    if (image != NULL) {
        /* determine total number of children */
        list = getRects((SVGimage *)image);
        numR = getLength(list);
        freeList(list);

        list = getCircles((SVGimage *)image);
        numC = getLength(list);
        freeList(list);
        
        list = getPaths((SVGimage *)image);
        numP = getLength(list);
        freeList(list);

        list = getGroups((SVGimage *)image);
        numG = getLength(list);
        freeList(list);

        /* determine length of output string */
        length = snprintf(NULL, 0, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", numR, numC, numP, numG);

        /* allocate for length + 1 (trailing 0x00) */
        json = calloc(length+1,sizeof(char));

        /* create output string */
        if (json != NULL) {
            snprintf(json, length+1, "{\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d}", numR, numC, numP, numG);
        }
    } else {
        json = strdup("{}");
    }

    return json;
}

char* SVGtoJSONext(const char* filename) {
    char *json = NULL;
    char *json_contents = NULL;
    char *list_contents = NULL;
    int length = 0;
    int numR = 0;
    int numC = 0;
    int numP = 0;
    int numG = 0;
    List *list = NULL;
    SVGimage *image = NULL;

    image = createSVGimage((char *)filename);

    if (image != NULL) {
        /* determine total number of children */
        list = getRects((SVGimage *)image);
        numR = getLength(list);
        freeList(list);

        list = getCircles((SVGimage *)image);
        numC = getLength(list);
        freeList(list);
        
        list = getPaths((SVGimage *)image);
        numP = getLength(list);
        freeList(list);

        list = getGroups((SVGimage *)image);
        numG = getLength(list);
        freeList(list);

        /* determine length of output string */
        length = snprintf(NULL, 0, "\"title\":\"%s\",\"description\":\"%s\",\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d", image->title, image->description, numR, numC, numP, numG);

        /* allocate for length + 1 (trailing 0x00) */
        json_contents = calloc(length+1,sizeof(char));

        if (json_contents != NULL) {
            snprintf(json_contents, length+1, "\"title\":\"%s\",\"description\":\"%s\",\"numRect\":%d,\"numCirc\":%d,\"numPaths\":%d,\"numGroups\":%d", image->title, image->description, numR, numC, numP, numG);
        }

        /* now add the individual groups */
        if (numR > 0) {
            list_contents = rectListToJSON(((SVGimage *)image)->rectangles);
            json_contents = realloc(json_contents, strlen(json_contents) + 2 + strlen(list_contents) + strlen("\"rects\":"));

            if (json_contents != NULL) {
                snprintf(json_contents+strlen(json_contents), 2 + strlen(list_contents) + strlen("\"rects\":"), ",\"rects\":%s", list_contents);
            }

            free(list_contents);
        }

        if (numC > 0) {
            list_contents = circListToJSON(((SVGimage *)image)->circles);
            json_contents = realloc(json_contents, strlen(json_contents) + 2 + strlen(list_contents) + strlen("\"circs\":"));

            if (json_contents != NULL) {
                snprintf(json_contents+strlen(json_contents), 2 + strlen(list_contents) + strlen("\"circs\":"), ",\"circs\":%s", list_contents);
            }

            free(list_contents);
        }

        if (numP > 0) {
            list_contents = pathListToJSON(((SVGimage *)image)->paths);
            json_contents = realloc(json_contents, strlen(json_contents) + 2 + strlen(list_contents) + strlen("\"paths\":"));

            if (json_contents != NULL) {
                snprintf(json_contents+strlen(json_contents), 2 + strlen(list_contents) + strlen("\"paths\":"), ",\"paths\":%s", list_contents);
            }

            free(list_contents);
        }

        if (numG > 0) {
            list_contents = groupListToJSON(((SVGimage *)image)->groups);
            json_contents = realloc(json_contents, strlen(json_contents) + 2 + strlen(list_contents) + strlen("\"groups\":"));

            if (json_contents != NULL) {
                snprintf(json_contents+strlen(json_contents), 2 + strlen(list_contents) + strlen("\"groups\":"), ",\"groups\":%s", list_contents);
            }

            free(list_contents);
        }

        /* create output string */
        json = calloc(strlen(json_contents)+3, sizeof(char));

        if (json != NULL) {
            snprintf(json, strlen(json_contents)+3, "{%s}", json_contents);
        }


        /* delete image again to prevent memory leaks */
        deleteSVGimage(image);
    } else {
        json = strdup("{}");
    }

    return json;
}

char* attrListToJSON(const List *list) {
    char *json = NULL;
    ListIterator iter;
    Attribute *attr = NULL;
    char *tmp = NULL;
    bool first = true;

    if ((list != NULL) && (getLength((List *)list) > 0)) {
        iter = createIterator((List *)list);

        json = strdup("[");

        if (json != NULL) {

            while ((attr = (Attribute *)nextElement(&iter)) != NULL) {
                tmp = attrToJSON(attr);

                if (first == false) {
                    /* reserve space for attr JSON string + '",' + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+2);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+2, ",%s", tmp);
                    }
                } else {
                    /* reserve space for attr JSON string + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+1);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+1, "%s", tmp);
                    }
                }

                free(tmp);
                
                first = false;
            }

            json = realloc(json, strlen(json)+2);
            snprintf(json+strlen(json), 2, "]");
        } 
    } else {
        json = strdup("[]");
    }

    return json;
}

char* circListToJSON(const List *list) {
    char *json = NULL;
    ListIterator iter;
    Circle *circ = NULL;
    char *tmp = NULL;
    bool first = true;

    if ((list != NULL) && (getLength((List *)list) > 0)) {
        iter = createIterator((List *)list);

        json = strdup("[");

        if (json != NULL) {

            while ((circ = (Circle *)nextElement(&iter)) != NULL) {
                tmp = circleToJSON(circ);

                if (first == false) {
                    /* reserve space for attr JSON string + ',' + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+2);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+2, ",%s", tmp);
                    }
                } else {
                    /* reserve space for attr JSON string + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+1);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+1, "%s", tmp);
                    }
                }

                free(tmp);
                
                first = false;
            }

            json = realloc(json, strlen(json)+2);
            snprintf(json+strlen(json), 2, "]");
        } 
    } else {
        json = strdup("[]");
    }

    return json;
}

char* rectListToJSON(const List *list) {
    char *json = NULL;
    ListIterator iter;
    Rectangle *rect = NULL;
    char *tmp = NULL;
    bool first = true;

    if ((list != NULL) && (getLength((List *)list) > 0)) {
        iter = createIterator((List *)list);

        json = strdup("[");

        if (json != NULL) {

            while ((rect = (Rectangle *)nextElement(&iter)) != NULL) {
                tmp = rectToJSON(rect);

                if (first == false) {
                    /* reserve space for attr JSON string + ',' + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+2);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+2, ",%s", tmp);
                    }
                } else {
                    /* reserve space for attr JSON string + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+1);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+1, "%s", tmp);
                    }
                }

                free(tmp);
                
                first = false;
            }

            json = realloc(json, strlen(json)+2);
            snprintf(json+strlen(json), 2, "]");
        } 
    } else {
        json = strdup("[]");
    }

    return json;
}

char* pathListToJSON(const List *list) {
    char *json = NULL;
    ListIterator iter;
    Path *path = NULL;
    char *tmp = NULL;
    bool first = true;

    if ((list != NULL) && (getLength((List *)list) > 0)) {
        iter = createIterator((List *)list);

        json = strdup("[");

        if (json != NULL) {

            while ((path = (Path *)nextElement(&iter)) != NULL) {
                tmp = pathToJSON(path);

                if (first == false) {
                    /* reserve space for attr JSON string + ',' + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+2);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+2, ",%s", tmp);
                    }
                } else {
                    /* reserve space for attr JSON string + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+1);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+1, "%s", tmp);
                    }
                }

                free(tmp);
                
                first = false;
            }

            json = realloc(json, strlen(json)+2);
            snprintf(json+strlen(json), 2, "]");
        } 
    } else {
        json = strdup("[]");
    }

    return json;

}

char* groupListToJSON(const List *list) {
    char *json = NULL;
    ListIterator iter;
    Group *group = NULL;
    char *tmp = NULL;
    bool first = true;

    if ((list != NULL) && (getLength((List *)list) > 0)) {
        iter = createIterator((List *)list);

        json = strdup("[");

        if (json != NULL) {

            while ((group = (Group *)nextElement(&iter)) != NULL) {
                tmp = groupToJSON(group);

                if (first == false) {
                    /* reserve space for attr JSON string + ',' + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+2);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+2, ",%s", tmp);
                    }
                } else {
                    /* reserve space for attr JSON string + terminating 0x00 */
                    json = realloc(json, strlen(json)+strlen(tmp)+1);

                    if (json != NULL) {
                        snprintf(json+strlen(json), strlen(tmp)+1, "%s", tmp);
                    }
                }

                free(tmp);
                
                first = false;
            }

            json = realloc(json, strlen(json)+2);
            snprintf(json+strlen(json), 2, "]");
        } 
    } else {
        json = strdup("[]");
    }

    return json;
}

SVGimage* JSONtoSVG(const char* svgString) {
    SVGimage *svg = NULL;
    char *tmp = NULL;

    if (svgString != NULL) {
        svg = calloc(sizeof(SVGimage), 1);

        if (svg != NULL)  {
            initialize_svg_strings(svg);
            initialize_svg_lists(svg);

            /* now parse the JSON string */
            tmp = getStrValueByName(svgString, "title");
            
            if (tmp != NULL) {
                /* copy max 255 bytes, there needs to be space for a trailing 0x00 */
                strncpy(svg->title, tmp, 255);
                free(tmp);
            }

            tmp = getStrValueByName(svgString, "descr");

            if (tmp != NULL) {
                /* copy max 255 bytes, there needs to be space for a trailing 0x00 */
                strncpy(svg->description, tmp, 255);
                free(tmp);
            }
        }
    }

    return svg;
}

Rectangle *JSONtoRect(const char *svgString) {
    Rectangle *rect = NULL;
    float ftmp = 0.0;
    char *tmp = NULL;

    if (svgString != NULL) {
        rect = calloc(1, sizeof(Rectangle));

        if (rect != NULL) {
            init_svg_rectangle(rect);

            ftmp = getFloatValueByName(svgString, "x");

            if (isnan(ftmp) == 0) {
                rect->x = ftmp;
            }

            ftmp = getFloatValueByName(svgString, "y");

            if (isnan(ftmp) == 0) {
                rect->y = ftmp;
            }

            ftmp = getFloatValueByName(svgString, "w");

            if (isnan(ftmp) == 0) {
                rect->width = ftmp;
            }

            ftmp = getFloatValueByName(svgString, "h");

            if (isnan(ftmp) == 0) {
                rect->height = ftmp;
            }

            tmp = getStrValueByName(svgString, "units");

            if (tmp != NULL) {
                strncpy(rect->units, tmp, 49);
                free(tmp);
            }
        }
    }

    return rect;
}

Circle *JSONtoCircle(const char *svgString) {
    Circle *circ = NULL;
    float ftmp = 0.0;
    char *tmp = NULL;

    if (svgString != NULL) {
        circ = calloc(1, sizeof(Circle));

        if (circ != NULL) {
            init_svg_circle(circ);

            ftmp = getFloatValueByName(svgString, "cx");

            if (isnan(ftmp) == 0) {
                circ->cx = ftmp;
            }

            ftmp = getFloatValueByName(svgString, "cy");

            if (isnan(ftmp) == 0) {
                circ->cy = ftmp;
            }

            ftmp = getFloatValueByName(svgString, "r");

            if (isnan(ftmp) == 0) {
                circ->r = ftmp;
            }

            tmp = getStrValueByName(svgString, "units");

            if (tmp != NULL) {
                strncpy(circ->units,tmp, 49);
                free(tmp);
            }
        }
    }

    return circ;
}

char *getStrValueByName(const char *str, const char *name) {
    char *value = NULL;
    char *cur = NULL;
    char *cur_end = NULL;
    char *search_string = NULL;
    int len = 0;

    len = snprintf(NULL, 0, "%s\":\"", name);
    search_string = malloc((len+1)*sizeof(char));

    if (search_string != NULL) {
        snprintf(search_string, len, "%s\":\"", name);

        if ((str != NULL) && (name != NULL)) {
            /* determine position of first " */
            cur = strstr(str, search_string)+strlen(search_string)+1;

            if ((cur != NULL) && ((cur-str) < strlen(str))) {
                /* determine position of second " */
                cur_end = strstr(cur, "\"");

                if ((cur_end != NULL) && ((cur_end - cur) > 0)) {
                    /* reserver appropriate space for variable name */
                    value = calloc(cur_end - cur, sizeof(char));

                    if (value != NULL) {
                        /* copy name there */
                        strncpy(value, cur, cur_end - cur);
                    }
                }
            }
        }

        free(search_string);
    }

    return value;
}

float getFloatValueByName(const char *str, const char *name) {
    float value = 0.0/0.0; /* produces a NaN as invalid result for the function */
    char *cur = NULL;
    char *search_string = NULL;
    int len = 0;

    len = snprintf(NULL, 0, "%s\":", name);
    search_string = malloc((len+1)*sizeof(char));

    if (search_string != NULL) {
        snprintf(search_string, len, "%s\":", name);

        if ((str != NULL) && (name != NULL)) {
            /* determine position of first " */
            cur = strstr(str, search_string)+strlen(search_string)+1;

            if ((cur != NULL) && ((cur-str) < strlen(str))) {
                sscanf(cur, "%f", &value);
            }
        }

        free(search_string);
    }

    return value;
}

void addRectangle(char *filename, char *rectJSON) {
    SVGimage *img = NULL;
    Rectangle *rect = NULL;

    img = createSVGimage(filename);

    if (img != NULL) {
        rect = JSONtoRect(rectJSON);

        if (rect != NULL) {
            insertBack(img->rectangles, rect);

            writeSVGimage(img, filename);
            deleteSVGimage(img);    
        }
    }
}

void addCircle(char *filename, char *circleJSON) {
    SVGimage *img = NULL;
    Circle *circ = NULL;

    img = createSVGimage(filename);

    if (img != NULL) {
        circ = JSONtoCircle(circleJSON);

        if (circ != NULL) {
            insertBack(img->circles, circ);

            writeSVGimage(img, filename);
            deleteSVGimage(img);    
        }
    }
}

