#include <XMLGeneration.h>
#include <LinkedListAPI.h>

xmlDocPtr createDocument(SVGimage *img) {
    xmlDocPtr doc = NULL;

    if (img != NULL) {
        doc = xmlNewDoc((xmlChar *)"1.0");
        doc->standalone = 0;
        xmlCreateIntSubset(doc, (xmlChar *)"svg", (xmlChar *)"-//W3C//DTD SVG 1.1//EN",
                (xmlChar *)"http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd");

        addSVGImage(doc, img);
    }

    return doc;
}

void addSVGImage(xmlDocPtr doc, SVGimage *img) {
    xmlNodePtr root;
    xmlNsPtr namespace;

    if ((doc != NULL) && (img != NULL)) {
        root = xmlNewNode(NULL, (xmlChar *)"svg");

        /* define namespace */
        printf("namespace in struct: %s\n", img->namespace);
        namespace = xmlNewNs(root, (xmlChar *)img->namespace, NULL);
        xmlSetNs(root, namespace);

        if (strlen(img->title) > 0) {
            xmlNewChild(root, NULL, (xmlChar *)"title", (xmlChar *)img->title);
        }

        if (strlen(img->description) > 0) {
            xmlNewChild(root, NULL, (xmlChar *)"desc", (xmlChar *)img->description);
        }

        /* set attributes here */
        addAttributes(root, img->otherAttributes);

        xmlDocSetRootElement(doc, root);

        /* add other elements in the correct order
         * rect -> circle -> path -> g */
        addRectangles(root, img->rectangles);
        addCircles(root, img->circles);
        addPaths(root, img->paths);
        addGroups(root, img->groups);
    }
}

void addAttributes(xmlNodePtr node, List *attributes) {
    ListIterator iter;
    Attribute *element = NULL;

    if ((node != NULL) && (attributes != NULL)) {
        iter = createIterator(attributes);

        while ((element = (Attribute *)nextElement(&iter)) != NULL) {
            xmlNewProp(node, (xmlChar *)element->name, (xmlChar *)element->value);
        }
    }
}

void addRectangles(xmlNodePtr node, List *rectangles) {
    xmlNodePtr child = NULL;
    ListIterator iter;
    ListIterator attrIter;
    Rectangle *rect = NULL;
    Attribute *attr = NULL;

    if ((node != NULL) && (rectangles != NULL)) {
        iter = createIterator(rectangles);
        while ((rect = (Rectangle *)nextElement(&iter)) != NULL) {
            child = xmlNewChild(node, NULL, (xmlChar *)"rect", NULL);

            /* if units are given, they are added within the
             * addFloatProperty function */
            addFloatProperty(child, "x", rect->x, rect->units);
            addFloatProperty(child, "y", rect->y, rect->units);
            addFloatProperty(child, "width", rect->width, rect->units);
            addFloatProperty(child, "height", rect->height, rect->units);

            attrIter = createIterator(rect->otherAttributes);

            while ((attr = (Attribute *)nextElement(&attrIter)) != NULL) {
                xmlNewProp(child, (xmlChar *)attr->name, (xmlChar *)attr->value);
            }
        }
    }
}

void addCircles(xmlNodePtr node, List *circles) {
    xmlNodePtr child = NULL;
    ListIterator iter;
    ListIterator attrIter;
    Circle *circ = NULL;
    Attribute *attr = NULL;

    if ((node != NULL) && (circles != NULL)) {
        iter = createIterator(circles);
        while ((circ = (Circle *)nextElement(&iter)) != NULL) {
            child = xmlNewChild(node, NULL, (xmlChar *)"circle", NULL);

            /* if units are given, they are added within the
             * addFloatProperty function */
            addFloatProperty(child, "cx", circ->cx, circ->units);
            addFloatProperty(child, "cy", circ->cy, circ->units);
            addFloatProperty(child, "r", circ->r, circ->units);

            attrIter = createIterator(circ->otherAttributes);

            while ((attr = (Attribute *)nextElement(&attrIter)) != NULL) {
                xmlNewProp(child, (xmlChar *)attr->name, (xmlChar *)attr->value);
            }
        }
    }
}

void addPaths(xmlNodePtr node, List *paths) {
    xmlNodePtr child = NULL;
    ListIterator iter;
    ListIterator attrIter;
    Path *path = NULL;
    Attribute *attr = NULL;

    if ((node != NULL) && (paths != NULL)) {
        iter = createIterator(paths);
        while ((path = (Path *)nextElement(&iter)) != NULL) {
            child = xmlNewChild(node, NULL, (xmlChar *)"path", NULL);

            xmlNewProp(child, (xmlChar *)"d", (xmlChar *)path->data);

            attrIter = createIterator(path->otherAttributes);

            while ((attr = (Attribute *)nextElement(&attrIter)) != NULL) {
                xmlNewProp(child, (xmlChar *)attr->name, (xmlChar *)attr->value);
            }
        }
    }
}

void addGroups(xmlNodePtr node, List *groups) {
    xmlNodePtr child = NULL;
    ListIterator iter;
    ListIterator attrIter;
    Group *group = NULL;
    Attribute *attr = NULL;

    if ((node != NULL) && (groups != NULL)) {
        iter = createIterator(groups);
        while ((group = (Group *)nextElement(&iter)) != NULL) {
            child = xmlNewChild(node, NULL, (xmlChar *)"g", NULL);

            attrIter = createIterator(group->otherAttributes);

            while ((attr = (Attribute *)nextElement(&attrIter)) != NULL) {
                xmlNewProp(child, (xmlChar *)attr->name, (xmlChar *)attr->value);
            }

            /* add other elements in the correct order
             * rect -> circle -> path -> g */
            addRectangles(child, group->rectangles);
            addCircles(child, group->circles);
            addPaths(child, group->paths);

            /* Attention! recursive call! */
            addGroups(child, group->groups);
        }
    }
}

void addFloatProperty(xmlNodePtr node, char *name, float value, char *units) {
    int length = 0;
    char *tmp = NULL;

    /* the sprintf calls within this function use %g instead of %f.
     * this leads to the shortest possible numbers.
     * 15 will become "15" instead of "15.000000" */

    if ((node != NULL) && (name != NULL)) {
        if (units != NULL) {
            length = snprintf(NULL, 0, "%f%s", value, units)+1;
            tmp = calloc(length, sizeof(char));
        } else {
            length = snprintf(NULL, 0, "%f", value)+1;
            tmp = calloc(length, sizeof(char));
        }

        if (tmp != NULL) {
            if (units != NULL) {
                snprintf(tmp, length, "%f%s", value, units);
            } else {
                snprintf(tmp, length, "%f", value);
            }

            xmlNewProp(node, (xmlChar *)name, (xmlChar *)tmp);
            free(tmp);
        }
    }
}

