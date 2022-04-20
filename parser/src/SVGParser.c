#define _POSIX_C_SOURCE = 200809L
#define _GNU_SOURCE

#include "assert.h"

#include <LinkedListAPI.h>
#include <ListHelpers.h>
#include <SVGHelpers.h>
#include <SVGParser.h>
#include <SVGShapeHelpers.h>
#include <SVGValidation.h>
#include <XMLGeneration.h>
#include <SVGAttributes.h>

#include <string.h>
#include <math.h>
#include <stdlib.h>

/* private functions */
void deleteRectangleStub(void *data);
void deleteCircleStub(void *data);
void deleteGroupStub(void *data);
void deletePathStub(void *data);

void append_list_to_list(List *list, List *listToAdd) {
    ListIterator iter;
    void *element;

    if ((list != NULL) && (listToAdd != NULL)) {
        iter = createIterator(listToAdd);

        while ((element = (void *)nextElement(&iter)) != NULL) {
            insertBack(list, element);
        }
    }
}

void free_list_keep_data(List *list) {
    if (list == NULL){
		return;
	}
	
	if (list->head == NULL && list->tail == NULL){
        free(list);
		return;
	}
	
	Node* tmp;
	
	while (list->head != NULL){
		tmp = list->head;
		list->head = list->head->next;
		free(tmp);
	}
	
	list->head = NULL;
	list->tail = NULL;
	list->length = 0;

    free(list);
}

List* find_elements(List * list, bool (*customCompare)(const void* first,const void* second), char* (*printFunction)(void* toBePrinted),void (*deleteFunction)(void* toBeDeleted),int (*compareFunction)(const void* first,const void* second), const void* searchRecord){
    List *result = NULL;

	if (customCompare == NULL)
		return NULL;

    result = initializeList(printFunction, deleteFunction, compareFunction);

	ListIterator itr = createIterator(list);

	void* data = nextElement(&itr);
	while (data != NULL)
	{
		if (customCompare(data, searchRecord))
            insertBack(result, data);
			//return data;

		data = nextElement(&itr);
	}

	return result;
}

/**
 * print_element_names:
 * @a_node: the initial xml node to consider.
 *
 * Prints the names of the all the xml elements
 * that are siblings or children of a given xml node.
 */
void print_element_names(xmlNode * a_node) {
    xmlNode *cur_node = NULL;

    for (cur_node = a_node; cur_node != NULL; cur_node = cur_node->next) {
        if (cur_node->type == XML_ELEMENT_NODE) {
            printf("node type: Element, name: %s\n", cur_node->name);
        }

        // Uncomment the code below if you want to see the content of every node.

        if (cur_node->content != NULL ){
            printf("  content: %s\n", cur_node->content);
        }

        // Iterate through every attribute of the current node
        xmlAttr *attr;
        for (attr = cur_node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;
            char *attrName = (char *)attr->name;
            char *cont = (char *)(value->content);
            printf("\tattribute name: %s, attribute value = %s\n", attrName, cont);
        }

        print_element_names(cur_node->children);
    }
}

void initialize_svg_strings(SVGimage *svg) {
    if (svg != NULL) {
        memset(svg->namespace, 0x00, 256);
        memset(svg->title, 0x00, 256);
        memset(svg->description, 0x00, 256);
    }
}

void initialize_svg_lists(SVGimage *svg) {
    if (svg != NULL) {
        /* Rectangles */

        /* free the list, if there is any */
        if (svg->rectangles != NULL) {
            freeList(svg->rectangles);
        }

        /* initialize new list */
        svg->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);

        /* Circles */

        /* free the list, if there is any */
        if (svg->circles != NULL) {
            freeList(svg->circles);
        }

        /* initialize new list */
        svg->circles = initializeList(circleToString, deleteCircle, compareCircles);

        /* Paths */

        /* free the list, if there is any */
        if (svg->paths != NULL) {
            freeList(svg->paths);
        }

        /* initialize new list */
        svg->paths = initializeList(pathToString, deletePath, comparePaths);

        /* Groups */

        /* free the list, if there is any */
        if (svg->groups != NULL) {
            /* groups may contain further groups. The function pointer
             * to delete elements is doing the recursive calls to
             * properly free nested groups */
            freeList(svg->groups);
        }

        /* initialize new list */
        svg->groups = initializeList(groupToString, deleteGroup, compareGroups);

        /* Attributes */

        /* free the list, if there is any */
        if (svg->otherAttributes != NULL) {
            freeList(svg->otherAttributes);
        }

        /* initialize new list */
        svg->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    }
}

void initialize_lists(Group *grp) {
    if (grp != NULL) {
        /* Rectangles */

        /* free the list, if there is any */
        if (grp->rectangles != NULL) {
            freeList(grp->rectangles);
        }

        /* initialize new list */
        grp->rectangles = initializeList(rectangleToString, deleteRectangle, compareRectangles);

        /* Circles */

        /* free the list, if there is any */
        if (grp->circles != NULL) {
            freeList(grp->circles);
        }

        /* initialize new list */
        grp->circles = initializeList(circleToString, deleteCircle, compareCircles);

        /* Paths */

        /* free the list, if there is any */
        if (grp->paths != NULL) {
            freeList(grp->paths);
        }

        /* initialize new list */
        grp->paths = initializeList(pathToString, deletePath, comparePaths);

        /* Groups */

        /* free the list, if there is any */
        if (grp->groups != NULL) {
            /* groups may contain further groups. The function pointer
             * to delete elements is doing the recursive calls to
             * properly free nested groups */
            freeList(grp->groups);
        }

        /* initialize new list */
        grp->groups = initializeList(groupToString, deleteGroup, compareGroups);

        /* Attributes */

        /* free the list, if there is any */
        if (grp->otherAttributes != NULL) {
            freeList(grp->otherAttributes);
        }

        /* initialize new list */
        grp->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    }
}

ContentType get_content_type(xmlNode *node) {
    ContentType type = CT_Unknown;

    if (node != NULL) {
        if (xmlStrEqual(node->name, (const xmlChar *)"svg")) {
            type = CT_Svg;
        } else if (xmlStrEqual(node->name, (const xmlChar *)"title")) {
            type = CT_Title;
        } else if (xmlStrEqual(node->name, (const xmlChar *)"desc")) {
            type = CT_Desc;
        } else if (xmlStrEqual(node->name, (const xmlChar *)"rect")) {
            type = CT_Rect;
        } else if (xmlStrEqual(node->name, (const xmlChar *)"circle")) {
            type = CT_Circle;
        } else if (xmlStrEqual(node->name, (const xmlChar *)"path")) {
            type = CT_Path;
        } else if (xmlStrEqual(node->name, (const xmlChar *)"g")) {
            type = CT_Group;
        }
    }

    return type;
}

void parse_xml_tree(xmlNode *root, SVGimage *img, Group *grp) {
    xmlNode *cur_node = NULL;
    ContentType type = CT_Unknown;
    List *attributes = NULL;
    char *attributeDesc = NULL;
    Rectangle *rect = NULL;
    Circle *circle = NULL;
    Path *path = NULL;
    Group *group = NULL;

    if ((root != NULL) && (img != NULL)) {
        for (cur_node = root; cur_node != NULL; cur_node = cur_node->next) {
            /* printf("node: %s -> content type: %d\n", cur_node->name, get_content_type(cur_node)); */
            /* read all attributes of the current tag into a list */
            attributes = read_attributes(cur_node);

            type = get_content_type(cur_node);

            switch (type) {
            case CT_Unknown:
                /* do nothing */
                break;
            case CT_Svg:
                read_namespace(cur_node, img);

                parse_svg_attribute_list(img, attributes);
                break;
            case CT_Title:
                read_string(cur_node, img->title, 256);
                break;
            case CT_Desc:
                read_string(cur_node, img->description, 256);
                break;
            case CT_Rect:
                rect = calloc(1, sizeof(Rectangle));

                if (rect != NULL) {
                    init_svg_rectangle(rect);
                    parse_rectangle_attribute_list(rect, attributes);

                    /* if there is a group given (for nested elements),
                     * add rectangle to group, otherwise to SVGimage directly */
                    if (grp != NULL) {
                        insertBack(grp->rectangles, rect);
                    } else {
                        insertBack(img->rectangles, rect);
                    }

                    rect = NULL;
                }

                break;
            case CT_Circle:
                circle = calloc(1, sizeof(Circle));

                if (circle != NULL) {
                    init_svg_circle(circle);
                    parse_circle_attribute_list(circle, attributes);

                    /* if there is a group given (for nested elements),
                     * add circle to group, otherwise to SVGimage directly */
                    if (grp != NULL) {
                        insertBack(grp->circles, circle);
                    } else {
                        insertBack(img->circles, circle);
                    }

                    circle = NULL;
                }

                break;
            case CT_Path:
                path = calloc(1, sizeof(Path));

                if (path != NULL) {
                    init_svg_path(path);
                    parse_path_attribute_list(path, attributes);

                    /* if there is a group given (for nested elements),
                     * add path to group, otherwise to SVGimage directly */
                    if (grp != NULL) {
                        insertBack(grp->paths, path);
                    } else {
                        insertBack(img->paths, path);
                    }

                    path = NULL;
                }

                break;
            case CT_Group:
                group = calloc(1, sizeof(Group));

                if (group != NULL) {
                    initialize_lists(group);
                    parse_group_attribute_list(group, attributes);

                    /* if there is a group given (for nested elements),
                     * add path to group, otherwise to SVGimage directly */
                    if (grp != NULL) {
                        insertBack(grp->groups, group);
                    } else {
                        insertBack(img->groups, group);
                    }

                }
                break;
            default:
                /* do nothing */
                break;
            }

            /* if there is a list of attributes, delete it here */
            if (attributes != NULL) {
                /* attributeDesc = toString(attributes); */
                /* printf("attributes: %s\n", attributeDesc); */
                freeList(attributes);
                free(attributeDesc);
            }

            if (cur_node->children) {
                if (group != NULL) {
                    parse_xml_tree(cur_node->children, img, group);
                } else {
                    parse_xml_tree(cur_node->children, img, NULL);
                }
            }

            group = NULL;
        }
    }
}

void read_string(xmlNode *node, char *dest, int length) {
    if ((node != NULL) && (dest != NULL) && (length > 0)) {
        /* copy n-2 characters and set n-1 as terminating 0x00
         * to avoid buffer overflows */
        if (node->content != NULL) {
            strncpy(dest, (char *)(node->content), length-2);

            /* if node content is too long, ensure a terminating 0x00 */
            if (strlen((char *)(node->content)) >= length-2) {
                dest[length-1] = 0x00;
            }
        } else if ((node->children != NULL) && (node->children->content != NULL)) {
            strncpy(dest, (char *)(node->children->content), length-2);

            /* if node content is too long, ensure a terminating 0x00 */
            if (strlen((char *)(node->children->content)) >= length-2) {
                dest[length-1] = 0x00;
            }
        }
    }
}

void read_namespace(xmlNode *node, SVGimage *img) {
    if ((node != NULL) && (img != NULL)) {
        /* copy n-2 characters and set n-1 as terminating 0x00
         * to avoid buffer overflows */
        strncpy(img->namespace, (char *)(node->ns->href), 254);

        if (strlen((char *)(node->ns->href)) == 254) {
            img->namespace[255] = 0x00;
        }
    }
}

List *read_attributes(xmlNode *node) {
    List *attributes = NULL;
    xmlAttr *attr;
    Attribute *a = NULL;

    if (node != NULL) {
        // Iterate through every attribute of the current node
        attributes = initializeList(attributeToString, deleteAttribute, compareAttributes);

        for (attr = node->properties; attr != NULL; attr = attr->next)
        {
            xmlNode *value = attr->children;

            a = malloc(sizeof(Attribute));

            if (a != NULL) {
                /* printf("found attribute: %s -> %s\n", attr->name, value->content); */
                a->name = strdup((char *)attr->name);
                a->value = strdup((char *)value->content);
                insertBack(attributes, a);
            }
        }
    }

    return attributes;
}

Attribute *copy_attribute(Attribute *a) {
    Attribute *b = NULL;

    if (a != NULL) {
        b = malloc(sizeof(Attribute));

        if (b != NULL) {
            b->name = strdup(a->name);
            b->value = strdup(a->value);
        }
    }

    return b;
}

Attribute *create_attribute_text(char *name, char *content) {
    Attribute *result = NULL;

    if ((name != NULL) && (content != NULL)) {
        result = malloc(sizeof(Attribute));

        if (result != NULL) {
            result->name = strdup(name);
            result->value = strdup(content);
        }
    }

    return result;
}

char *append_float_attribute_to_string(char *str, char *name, float value) {
    int requiredLength = 0;
    requiredLength = snprintf(NULL, 0, "%s: %f\n", name, value);
    str = realloc(str, strlen(str) + requiredLength + 1);

    if (str != NULL) {
        snprintf(str+strlen(str), requiredLength + 1, "%s: %f\n", name, value);
    }

    return str;
}

char *append_string_attribute_to_string(char *str, char *name, char *value) {
    int requiredLength = 0;
    requiredLength = snprintf(NULL, 0, "%s: %s\n", name, value);

    str = realloc(str, strlen(str) + requiredLength + 1);

    if (str != NULL) {
        snprintf(str+strlen(str), requiredLength + 1, "%s: %s\n", name, value);
    }

    return str;
}

char *append_string_to_string(char *str, char *prefix, char *str2, char *postfix) {
    int requiredLength = 0;
    requiredLength = snprintf(NULL, 0, "%s%s%s", prefix, str2, postfix);

    str = realloc(str, strlen(str) + requiredLength + 1);

    if (str != NULL) {
        snprintf(str+strlen(str), requiredLength + 1, "%s%s%s", prefix, str2, postfix);
    }

    return str;
}

List *get_rectangles(Group *grp) {
    List *result = NULL;
    ListIterator iter;
    List *tmp = NULL;
    void *element = NULL;

    /* initialize this immediately with an empty list. makes it easier
     * to combine lists of recursive calls without extra code to sort
     * out NULL pointers */
    result = initializeList(rectangleToString, deleteRectangle, compareRectangles);

    if (grp != NULL) {
        if (grp->rectangles != NULL) {
            iter = createIterator(grp->rectangles);

            while ((element = nextElement(&iter)) != NULL) {
                insertBack(result, element);
            }
        }

        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_rectangles(element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

List *get_circles(Group *grp) {
    List *result = NULL;
    ListIterator iter;
    List *tmp = NULL;
    void *element = NULL;

    /* initialize this immediately with an empty list. makes it easier
     * to combine lists of recursive calls without extra code to sort
     * out NULL pointers */
    result = initializeList(circleToString, deleteCircle, compareCircles);

    if (grp != NULL) {
        if (grp->circles != NULL) {
            iter = createIterator(grp->circles);

            while ((element = nextElement(&iter)) != NULL) {
                insertBack(result, element);
            }
        }

        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_circles(element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

List *get_paths(Group *grp) {
    List *result = NULL;
    ListIterator iter;
    List *tmp = NULL;
    void *element = NULL;

    /* initialize this immediately with an empty list. makes it easier
     * to combine lists of recursive calls without extra code to sort
     * out NULL pointers */
    result = initializeList(pathToString, deletePath, comparePaths);

    if (grp != NULL) {
        if (grp->paths != NULL) {
            iter = createIterator(grp->paths);

            while ((element = nextElement(&iter)) != NULL) {
                insertBack(result, element);
            }
        }

        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_rectangles(element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

List *get_groups(Group *grp) {
    List *result = NULL;
    ListIterator iter;
    List *tmp = NULL;
    void *element = NULL;

    /* initialize this immediately with an empty list. makes it easier
     * to combine lists of recursive calls without extra code to sort
     * out NULL pointers */
    result = initializeList(groupToString, deleteGroup, compareGroups);

    if (grp != NULL) {
        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                insertBack(result, element);
            }
        }

        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_groups(element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

int num_rects_with_area_in_group(Group *grp, float area) {
    int result = 0;
    Rectangle *rect = NULL;
    List *tmp = NULL;
    ListIterator iter;
    void *element = NULL;

    if ((grp != NULL) && (area >= 0.0)) {
        rect = calloc(1, sizeof(Rectangle));

        if (rect != NULL) {
            init_svg_rectangle_with_area(rect, area);

            if (grp->rectangles != NULL) {
                tmp = find_elements(grp->rectangles, compare_rectangle_area, rectangleToString, deleteRectangle, compareRectangles, rect);

                if (tmp != NULL) {
                    result = getLength(tmp);
                    free_list_keep_data(tmp);
                }
            }

            deleteRectangle(rect);
        }

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_rects_with_area_in_group((Group *)element, area);
        }
    }

    return result;

}

int num_circles_with_area_in_group(Group *grp, float area) {
    int result = 0;
    Circle *circ = NULL;
    List *tmp = NULL;
    ListIterator iter;
    void *element = NULL;

    if ((grp != NULL) && (area >= 0.0)) {
        circ = calloc(1, sizeof(Circle));

        if (circ != NULL) {
            init_svg_circle_with_area(circ, area);

            if (grp->circles != NULL) {
                tmp = find_elements(grp->circles, compare_circle_area, circleToString, deleteCircle, compareCircles, circ);

                if (tmp != NULL) {
                    result = getLength(tmp);
                    free_list_keep_data(tmp);
                }

                deleteCircle(circ);
            }
        }

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_circles_with_area_in_group((Group *)element, area);
        }
    }

    return result;
}

int num_paths_with_data_in_group(Group *grp, char* data) {
    int result = 0;
    Path *path = NULL;
    List *tmp = NULL;
    ListIterator iter;
    void *element = NULL;

    if ((grp != NULL) && (data != NULL)) {
        path = calloc(1, sizeof(Path));

        if (path != NULL) {
            init_svg_path_with_data(path, data);

            if (grp->paths != NULL) {
                tmp = find_elements(grp->paths, compare_paths, pathToString, deletePath, comparePaths, path);

                if (tmp != NULL) {
                    result = getLength(tmp);
                    free_list_keep_data(tmp);
                }

                deletePath(path);
            }
        }

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_paths_with_data_in_group((Group *)element, data);
        }
    }

    return result;
}

int num_groups_with_length_in_group(Group *grp, int len) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if ((grp != NULL) && (len >= 0)) {
        if (group_length(grp) == len) {
            result = 1;
        }

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_groups_with_length_in_group((Group *)element, len);
        }
    }

    return result;
}

int num_attributes_in_group(Group *grp) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if (grp != NULL) {
        if (grp->otherAttributes != NULL) {
            result += getLength(grp->otherAttributes);
        }

        if (grp->rectangles != NULL) {
            iter = createIterator(grp->rectangles);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_rectangle((Rectangle *)element);
            }
        }

        if (grp->circles != NULL) {
            iter = createIterator(grp->circles);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_circle((Circle *)element);
            }
        }

        if (grp->paths != NULL) {
            iter = createIterator(grp->paths);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_path((Path *)element);
            }
        }

        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_group((Group *)element);
            }
        }
    }

    return result;
}


/** Function to create an SVG object based on the contents of an SVG file.
 *@pre File name cannot be an empty string or NULL.
       File represented by this name must exist and must be readable.
 *@post Either:
        A valid SVGimage has been created and its address was returned
		or 
		An error occurred, and NULL was returned
 *@return the pinter to the new struct or NULL
 *@param fileName - a string containing the name of the SVG file
**/
SVGimage* createSVGimage(char* fileName) {
    SVGimage *img = NULL;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if ((fileName != NULL) && (strlen(fileName) > 0)) {
        LIBXML_TEST_VERSION

        doc = xmlReadFile(fileName, NULL, 0);

        if (doc != NULL) {
            root_element = xmlDocGetRootElement(doc);

            img = calloc(sizeof(SVGimage), 1);

            if (img != NULL)  {
                initialize_svg_strings(img);
                initialize_svg_lists(img);
                parse_xml_tree(root_element, img, NULL);
                //print_element_names(root_element);
            }

            xmlFreeDoc(doc);
        }

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();
    }

    return img;
}

/** Function to create a string representation of an SVG object.
 *@pre SVGimgage exists, is not null, and is valid
 *@post SVGimgage has not been modified in any way, and a string representing the SVG contents has been created
 *@return a string contaning a humanly readable representation of an SVG object
 *@param obj - a pointer to an SVG struct
**/
char* SVGimageToString(SVGimage* img) {
    char *desc = NULL;
    char *elementDesc = NULL;
    ListIterator iter;
    void *element = NULL;

    if (img != NULL) {
        desc = strdup("");
        desc = append_string_to_string(desc, "svg->namespace:\t", img->namespace, "\n");
        desc = append_string_to_string(desc, "svg->title:\t", img->title, "\n");
        desc = append_string_to_string(desc, "svg->desc:\t", img->description, "\n");

        if (img->rectangles != NULL) {
            iter = createIterator(img->rectangles);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = rectangleToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (img->circles != NULL) {
            iter = createIterator(img->circles);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = circleToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (img->paths != NULL) {
            iter = createIterator(img->paths);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = pathToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (img->otherAttributes != NULL) {
            iter = createIterator(img->otherAttributes);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = attributeToString((Attribute *)element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (img->groups != NULL) {
            iter = createIterator(img->groups);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = groupToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }
    }

    return desc;
}

/** Function to delete image content and free all the memory.
 *@pre SVGimgage  exists, is not null, and has not been freed
 *@post SVSVGimgageG  had been freed
 *@return none
 *@param obj - a pointer to an SVG struct
**/
void deleteSVGimage(SVGimage* img) {
    if (img != NULL) {
        if (img->rectangles != NULL) {
            /* delete list of rectangles */
            freeList(img->rectangles);
        }

        if (img->circles != NULL) {
            /* delete list of circles */
            freeList(img->circles);
        }

        if (img->paths != NULL) {
            /* delete list of paths */
            freeList(img->paths);
        }

        if (img->groups != NULL) {
            /* delete list of groups */
            freeList(img->groups);
        }

        if (img->otherAttributes != NULL) {
            /* delete list of other attributes */
            freeList(img->otherAttributes);
        }

        /* finally free SVGimage itself */
        free(img);
    }
}

/* For the four "get..." functions below, make sure you return a list of opinters to the existing structs 
 - do not allocate new structs.  They all share the same format, and only differ in the contents of the lists 
 they return.
 
 *@pre SVGimgage exists, is not null, and has not been freed
 *@post SVGimgage has not been modified in any way
 *@return a newly allocated List of components.  While the List struct itself is new, the components in it are just pointers
  to the ones in the image.

 The list must me empty if the element is not found - do not return NULL

 *@param obj - a pointer to an SVG struct
 */

// Function that returns a list of all rectangles in the image.  
List* getRects(SVGimage* img) {
    List *result = NULL;
    ListIterator iter;
    void *element = NULL;
    List *tmp = NULL;

    if (img != NULL) {
        result = initializeList(rectangleToString, deleteRectangleStub, compareRectangles);

        iter = createIterator(img->rectangles);

        while ((element = nextElement(&iter)) != NULL) {
            insertBack(result, (Rectangle *)element);
        }

        if (img->groups != NULL) {
            iter = createIterator(img->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_rectangles((Group *)element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

// Function that returns a list of all circles in the image.  
List* getCircles(SVGimage* img) {
    List *result = NULL;
    ListIterator iter;
    void *element = NULL;
    List *tmp = NULL;

    if (img != NULL) {
        result = initializeList(circleToString, deleteCircleStub, compareCircles);

        iter = createIterator(img->circles);

        while ((element = nextElement(&iter)) != NULL) {
            insertBack(result, (Circle *)element);
        }

        if (img->groups != NULL) {
            iter = createIterator(img->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_circles((Group *)element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

// Function that returns a list of all groups in the image.  
List* getGroups(SVGimage* img) {
    List *result = NULL;
    ListIterator iter;
    void *element = NULL;
    List *tmp = NULL;

    if (img != NULL) {
        result = initializeList(groupToString, deleteGroupStub, compareGroups);

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            insertBack(result, (Group *)element);
        }

        if (img->groups != NULL) {
            iter = createIterator(img->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_groups((Group *)element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}

// Function that returns a list of all paths in the image.  
List* getPaths(SVGimage* img) {
    List *result = NULL;
    ListIterator iter;
    void *element = NULL;
    List *tmp = NULL;

    if (img != NULL) {
        result = initializeList(pathToString, deletePathStub, comparePaths);

        iter = createIterator(img->paths);

        while ((element = nextElement(&iter)) != NULL) {
            insertBack(result, (Path *)element);
        }

        if (img->groups != NULL) {
            iter = createIterator(img->groups);

            while ((element = nextElement(&iter)) != NULL) {
                tmp = get_paths((Group *)element);
                append_list_to_list(result, tmp);
                free_list_keep_data(tmp);
            }
        }
    }

    return result;
}


/* For the four "num..." functions below, you need to search the SVG image for components  that match the search 
  criterion.  You may wish to write some sort of a generic searcher fucntion that accepts an image, a predicate function,
  and a dummy search record as arguments.  We will discuss such search functions in class

 NOTE: For consistency, use the ceil() function to round the floats up to the nearest integer once you have computed 
 the number you need.  See A1 Module 2 for details.

 *@pre SVGimgage exists, is not null, and has not been freed.  The search criterion is valid
 *@post SVGimgage has not been modified in any way
 *@return an int indicating how many objects matching the criterion are contained in the image
 *@param obj - a pointer to an SVG struct
 *@param 2nd - the second param depends on the function.  See details below
 */   

// Function that returns the number of all rectangles with the specified area
int numRectsWithArea(SVGimage* img, float area) {
    int result = 0;
    Rectangle *rect = NULL;
    List *tmp = NULL;
    ListIterator iter;
    void *element = NULL;

    if ((img != NULL) && (area >= 0.0)) {
        rect = calloc(1, sizeof(Rectangle));

        if (rect != NULL) {
            init_svg_rectangle_with_area(rect, area);

            if (img->rectangles != NULL) {
                tmp = find_elements(img->rectangles, compare_rectangle_area, rectangleToString, deleteRectangle, compareRectangles, rect);

                if (tmp != NULL) {
                    result = getLength(tmp);
                    free_list_keep_data(tmp);
                }
            }

            deleteRectangle(rect);
        }

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_rects_with_area_in_group((Group *)element, area);
        }
    }

    return result;
}

// Function that returns the number of all circles with the specified area
int numCirclesWithArea(SVGimage* img, float area) {
    int result = 0;
    Circle *circ = NULL;
    List *tmp = NULL;
    ListIterator iter;
    void *element = NULL;

    if ((img != NULL) && (area >= 0.0)) {
        circ = calloc(1, sizeof(Circle));

        if (circ != NULL) {
            init_svg_circle_with_area(circ, area);

            if (img->circles != NULL) {
                tmp = find_elements(img->circles, compare_circle_area, circleToString, deleteCircle, compareCircles, circ);

                if (tmp != NULL) {
                    result = getLength(tmp);
                    free_list_keep_data(tmp);
                }

                deleteCircle(circ);
            }
        }

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_circles_with_area_in_group((Group *)element, area);
        }
    }

    return result;
}

// Function that returns the number of all paths with the specified data - i.e. Path.data field
int numPathsWithdata(SVGimage* img, char* data) {
    int result = 0;
    Path *path = NULL;
    List *tmp = NULL;
    ListIterator iter;
    void *element = NULL;

    if ((img != NULL) && (data != NULL)) {
        path = calloc(1, sizeof(Path));

        if (path != NULL) {
            init_svg_path_with_data(path, data);

            if (img->paths != NULL) {
                tmp = find_elements(img->paths, compare_paths, pathToString, deletePath, comparePaths, path);

                if (tmp != NULL) {
                    result = getLength(tmp);
                    free_list_keep_data(tmp);
                }

                deletePath(path);
            }
        }

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_paths_with_data_in_group((Group *)element, data);
        }
    }

    return result;
}

// Function that returns the number of all groups with the specified length - see A1 Module 2 for details
int numGroupsWithLen(SVGimage* img, int len) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if ((img != NULL) && (len >= 0)) {
        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_groups_with_length_in_group((Group *)element, len);
        }
    }

    return result;
}

/*  Function that returns the total number of Attribute structs in the SVGimage - i.e. the number of Attributes
    contained in all otherAttributes lists in the structs making up the SVGimage
    *@pre SVGimgage  exists, is not null, and has not been freed.  
    *@post SVGimage has not been modified in any way
    *@return the total length of all attribute structs in the SVGimage
    *@param obj - a pointer to an SVG struct
*/
int numAttr(SVGimage* img) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if (img != NULL) {
        if (img->otherAttributes != NULL) {
            result += getLength(img->otherAttributes);
        }

        if (img->rectangles != NULL) {
            iter = createIterator(img->rectangles);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_rectangle((Rectangle *)element);
            }
        }

        if (img->circles != NULL) {
            iter = createIterator(img->circles);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_circle((Circle *)element);
            }
        }

        if (img->paths != NULL) {
            iter = createIterator(img->paths);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_path((Path *)element);
            }
        }

        if (img->groups != NULL) {
            iter = createIterator(img->groups);

            while ((element = nextElement(&iter)) != NULL) {
                result += num_attributes_in_group((Group *)element);
            }
        }
    }

    return result;
}


/* ******************************* List helper functions  - MUST be implemented *************************** */

void deleteAttribute( void* data) {
    if (data != NULL) {
        if (((Attribute *)data)->name != NULL) {
            free(((Attribute *)data)->name);
        }

        if (((Attribute *)data)->value != NULL) {
            free(((Attribute *)data)->value);
        }

        free(data);
    }
}

char* attributeToString( void* data) {
    char *desc = NULL;
    unsigned int descLength = 0;
    Attribute *a = NULL;

    if (data != NULL) {
        /* shortcut for easier syntax */
        a = (Attribute *)data;

        descLength = strlen(a->name) + 3;
        desc = calloc(descLength, sizeof(char));

        if (desc != NULL) {
            snprintf(desc, strlen(a->name) + 3, "%s: ", a->name);
        }

        /* if there is a value, reserve space for it and print it */
        if (a->value != NULL) {
            descLength += strlen(a->value)+1;
            desc = realloc(desc, descLength);

            if (desc != NULL) {
                snprintf(desc+strlen(desc), strlen(a->value)+1, "%s\n", a->value);
            }
        } else {
            /* else, add "null" (after reserving space) to indicate a missing value */
            descLength += strlen("null\n");
            desc = realloc(desc, descLength);

            if (desc != NULL) {
                snprintf(desc+strlen(desc), strlen("null\n")+1, "null\n");
            }
        }
    }

    return desc;
}

int compareAttributes(const void *first, const void *second) {
    int result = 0;
    Attribute *a = NULL;
    Attribute *b = NULL;

    if ((first != NULL) && (second != NULL)) {
        /* shortcuts for easier syntax */
        a = (Attribute *)first;
        b = (Attribute *)second;

        /* first compare by the name */
        if ((a->name != NULL) && (b->name != NULL)) {
            result = strcmp(a->name, b->name);
        }

        /* if equal (not sure, if this should even happen), compare by value */
        if ((result == 0) && (a->value != NULL) && (b->value != NULL)) {
            result = strcmp(a->value, b->value);
        }
    }

    return result;
}

void deleteGroup(void* data) {
    Group *grp = NULL;

    if (data != NULL) {
        /* shortcut for easier syntax */
        grp = (Group *)data;

        if (grp->rectangles != NULL) {
            freeList(grp->rectangles);
        }

        if (grp->circles != NULL) {
            freeList(grp->circles);
        }

        if (grp->paths != NULL) {
            freeList(grp->paths);
        }

        if (grp->otherAttributes != NULL) {
            freeList(grp->otherAttributes);
        }

        /* Attention! This causes a recursive call to deleteGroup! */
        if (grp->groups != NULL) {
            freeList(grp->groups);
        }

        free(data);
    }
}

void deleteGroupStub(void *data) {

}

char* groupToString( void* data) {
    char *desc = NULL;
    char *elementDesc = NULL;
    Group *grp = NULL;
    ListIterator iter;
    void *element = NULL;

    if (data != NULL) {
        /* shortcut for easier syntax */
        grp = (Group *)data;
        desc = strdup("group:\n");

        /* first print attributes, since they belong to the group
         * tag, otherwise, the output looks weird */
        if (grp->otherAttributes != NULL) {
            iter = createIterator(grp->otherAttributes);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = attributeToString((Attribute *)element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (grp->rectangles != NULL) {
            iter = createIterator(grp->rectangles);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = rectangleToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (grp->circles != NULL) {
            iter = createIterator(grp->circles);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = circleToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (grp->paths != NULL) {
            iter = createIterator(grp->paths);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = pathToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }

        if (grp->groups != NULL) {
            iter = createIterator(grp->groups);

            while ((element = nextElement(&iter)) != NULL) {
                elementDesc = groupToString(element);
                desc = append_string_to_string(desc, "\t", elementDesc, "\n");
                free(elementDesc);
            }
        }
    }

    return desc;
}

int compareGroups(const void *first, const void *second) {
    return 0;
}

void deleteRectangle(void* data) {
    if (data != NULL) {
        if (((Rectangle *)data)->otherAttributes != NULL) {
            freeList(((Rectangle *)data)->otherAttributes);
        }

        free(data);
    }
}

void deleteRectangleStub(void *data) {

}

char* rectangleToString(void* data) {
    char *desc = NULL;
    Rectangle *r = NULL;
    char *otherAttr = NULL;
    ListIterator iter;
    void *element = NULL;

    if (data != NULL) {
        /* shortcut for easier syntax */
        r = (Rectangle *)data;

        desc = strdup("rectangle:\n");

        desc = append_float_attribute_to_string(desc, "\tx", r->x);
        desc = append_float_attribute_to_string(desc, "\ty", r->y);
        desc = append_float_attribute_to_string(desc, "\twidth", r->width);
        desc = append_float_attribute_to_string(desc, "\theight", r->height);

        if (r->units != NULL) {
            desc = append_string_attribute_to_string(desc, "\tunits", r->units);
        }

        if (r->otherAttributes != NULL) {
            iter = createIterator(r->otherAttributes);

            while ((element = nextElement(&iter)) != NULL) {
                otherAttr = attributeToString((Attribute *)element);
                desc = append_string_to_string(desc, "\t", otherAttr, "\n");
                free(otherAttr);
            }
        }
    }

    return desc;
}

int compareRectangles(const void *first, const void *second) {
    return 0;
}

void deleteCircle(void* data) {
    if (data != NULL) {
        if (((Circle *)data)->otherAttributes != NULL) {
            freeList(((Circle *)data)->otherAttributes);
        }

        free(data);
    }
}

void deleteCircleStub(void *data) {

}

char* circleToString(void* data) {
    char *desc = NULL;
    Circle *c = NULL;
    char *otherAttr = NULL;
    ListIterator iter;
    void *element = NULL;

    if (data != NULL) {
        /* shortcut for easier syntax */
        c = (Circle *)data;

        desc = strdup("circle:\n");

        desc = append_float_attribute_to_string(desc, "\tcx", c->cx);
        desc = append_float_attribute_to_string(desc, "\tcy", c->cy);
        desc = append_float_attribute_to_string(desc, "\tr", c->r);

        if (c->units != NULL) {
            desc = append_string_attribute_to_string(desc, "\tunits", c->units);
        }

        if (c->otherAttributes != NULL) {
            iter = createIterator(c->otherAttributes);

            while ((element = nextElement(&iter)) != NULL) {
                otherAttr = attributeToString((Attribute *)element);
                desc = append_string_to_string(desc, "\t", otherAttr, "\n");
                free(otherAttr);
            }
        }
    }

    return desc;
}

int compareCircles(const void *first, const void *second) {
    return 0;
}

void deletePath(void* data) {
    if (data != NULL) {
        if (((Path *)data)->data != NULL) {
            free(((Path *)data)->data);
        }

        if (((Path *)data)->otherAttributes != NULL) {
            freeList(((Path *)data)->otherAttributes);
        }

        free(data);
    }
}

void deletePathStub(void *data) {

}

char* pathToString(void* data) {
    char *desc = NULL;
    Path *p = NULL;
    char *otherAttr = NULL;
    ListIterator iter;
    void *element = NULL;

    if (data != NULL) {
        /* shortcut for easier syntax */
        p = (Path *)data;

        desc = strdup("path:\n");

        desc = append_string_attribute_to_string(desc, "\tdata", p->data);

        if (p->otherAttributes != NULL) {
            iter = createIterator(p->otherAttributes);

            while ((element = nextElement(&iter)) != NULL) {
                otherAttr = attributeToString((Attribute *)element);
                desc = append_string_to_string(desc, "\t", otherAttr, "\n");
                free(otherAttr);
            }
        }
    }

    return desc;
}

int comparePaths(const void *first, const void *second) {
    return 0;
}

/* initializes a Rectangle structure with default values */
void init_svg_rectangle(Rectangle *rect) {
    if (rect != NULL) {
        rect->x = 0.0;
        rect->y = 0.0;
        rect->width = 0.0;
        rect->height = 0.0;
        memset(rect->units, 0x00, 50);

        if (rect->otherAttributes != NULL) {
            freeList(rect->otherAttributes);
        }

        rect->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    }
}

/* initializes a Circle structure with default values */
void init_svg_circle(Circle *circ) {
    if (circ != NULL) {
        circ->cx = 0.0;
        circ->cy = 0.0;
        circ->r = 0.0;
        memset(circ->units, 0x00, 50);

        if (circ->otherAttributes != NULL) {
            freeList(circ->otherAttributes);
        }

        circ->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    }
}

/* initializes a Path structure with default values */
void init_svg_path(Path *path) {
    if (path != NULL) {
        if (path->data != NULL) {
            free(path->data);
        }

        path->data = calloc(1,sizeof(char));

        if (path->otherAttributes != NULL) {
            freeList(path->otherAttributes);
        }

        path->otherAttributes = initializeList(attributeToString, deleteAttribute, compareAttributes);
    }
}

void init_svg_rectangle_with_area(Rectangle *rect, float area) {
    if (rect != NULL) {
        init_svg_rectangle(rect);

        rect->width = sqrtf(area);
        rect->height = sqrtf(area);
    }
}

void init_svg_circle_with_area(Circle *circ, float area) {
    if (circ != NULL) {
        init_svg_circle(circ);

        circ->r = sqrtf(area/M_PI);
    }
}

void init_svg_path_with_data(Path *path, char *data) {
    if ((path != NULL) && (data != NULL)) {
        init_svg_path(path);

        if (path->data != NULL) {
            free(path->data);
        }

        path->data = strdup(data);
    }
}

void parse_rectangle_attribute_list(Rectangle *rect, List *attributes) {
    ListIterator iter;
    void *element = NULL;
    Attribute *attr = NULL;

    if ((rect != NULL) && (attributes != NULL)) {
        iter = createIterator(attributes);

        while ((element = nextElement(&iter)) != NULL) {
            /* special cases for attributes that are represented by
             * member variables */
            if (!strcmp(((Attribute *)element)->name, "x")) {
                read_number_and_unit(((Attribute *)element)->value, &rect->x, rect->units);
                /* sscanf(((Attribute *)element)->value, "%f", &rect->x); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "y")) {
                read_number_and_unit(((Attribute *)element)->value, &rect->y, rect->units);
                /* sscanf(((Attribute *)element)->value, "%f", &rect->y); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "width")) {
                read_number_and_unit(((Attribute *)element)->value, &rect->width, rect->units);
                /* sscanf(((Attribute *)element)->value, "%f", &rect->width); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "height")) {
                read_number_and_unit(((Attribute *)element)->value, &rect->height, rect->units);
                /* sscanf(((Attribute *)element)->value, "%f", &rect->height); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "units")) {
                sscanf(((Attribute *)element)->value, "%49s", rect->units);
                continue;
            }

            /* general case, copy element to attribute list of rectangle */
            attr = copy_attribute((Attribute *)element);
            insertBack(rect->otherAttributes, attr);
        }
    }
}

void parse_circle_attribute_list(Circle *circle, List *attributes) {
    ListIterator iter;
    void *element = NULL;
    Attribute *attr = NULL;

    if ((circle != NULL) && (attributes != NULL)) {
        iter = createIterator(attributes);

        while ((element = nextElement(&iter)) != NULL) {
            /* special cases for attributes that are represented by
             * member variables */
            if (!strcmp(((Attribute *)element)->name, "cx")) {
                read_number_and_unit(((Attribute *)element)->value, &circle->cx, circle->units);
                /* sscanf(((Attribute *)element)->value, "%f", &circle->cx); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "cy")) {
                read_number_and_unit(((Attribute *)element)->value, &circle->cy, circle->units);
                /* sscanf(((Attribute *)element)->value, "%f", &circle->cy); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "r")) {
                read_number_and_unit(((Attribute *)element)->value, &circle->r, circle->units);
                /* sscanf(((Attribute *)element)->value, "%f", &circle->r); */
                continue;
            }

            if (!strcmp(((Attribute *)element)->name, "units")) {
                sscanf(((Attribute *)element)->value, "%49s", circle->units);
                continue;
            }

            /* general case, copy element to attribute list of rectangle */
            attr = copy_attribute((Attribute *)element);
            insertBack(circle->otherAttributes, attr);
        }
    }
}

void parse_path_attribute_list(Path *path, List *attributes) {
    ListIterator iter;
    void *element = NULL;
    Attribute *attr = NULL;

    if ((path != NULL) && (attributes != NULL)) {
        iter = createIterator(attributes);

        while ((element = nextElement(&iter)) != NULL) {
            /* special cases for attributes that are represented by
             * member variables */
            if (!strcmp(((Attribute *)element)->name, "d")) {
                if (path->data != NULL) {
                    free(path->data);
                }

                path->data = strdup(((Attribute *)element)->value);
                continue;
            }

            /* general case, copy element to attribute list of rectangle */
            attr = copy_attribute((Attribute *)element);
            insertBack(path->otherAttributes, attr);
        }
    }
}

void parse_svg_attribute_list(SVGimage *img, List *attributes) {
    ListIterator iter;
    void *element = NULL;
    Attribute *attr = NULL;

    if ((img != NULL) && (attributes != NULL)) {
        iter = createIterator(attributes);

        while ((element = nextElement(&iter)) != NULL) {
            /* special cases for attributes that are represented by
             * member variables */
            /* if (!strcmp(((Attribute *)element)->name, "width")) { */
            /*     /1* add number and units as separate attributes *1/ */

            /*     /1* if there is a unit given, unit_pos is != NULL after this call *1/ */
            /*     strtof(((Attribute *)element)->value, &unit_pos); */

            /*     if (unit_pos != NULL) { */
            /*         /1* reserve space for the length from ->value until unit_pos */
            /*          * + 1 byte as terminating 0x00 *1/ */
            /*         tmp_number_char = calloc(unit_pos - ((Attribute *)element)->value + 1, sizeof(char)); */

            /*         if (tmp_number_char != NULL) { */
            /*             strncpy(tmp_number_char, ((Attribute *)element)->value, unit_pos - ((Attribute *)element)->value); */
            /*             attr = create_attribute_text("width", tmp_number_char); */
            /*             insertBack(img->otherAttributes, attr); */

            /*             unit_pos = NULL; */
            /*             free(tmp_number_char); */
            /*         } */

            /*     } else { */
            /*         attr = create_attribute_text("width", ((Attribute *)element)->value); */
            /*         insertBack(img->otherAttributes, attr); */
            /*     } */

            /*     continue; */
            /* } */

            /* if (!strcmp(((Attribute *)element)->name, "height")) { */
            /*     /1* add number and units as separate attributes *1/ */

            /*     /1* if there is a unit given, unit_pos is != NULL after this call *1/ */
            /*     strtof(((Attribute *)element)->value, &unit_pos); */

            /*     if (unit_pos != NULL) { */
            /*         /1* reserve space for the length from ->content until unit_pos */
            /*          * + 1 byte as terminating 0x00 *1/ */
            /*         tmp_number_char = calloc(unit_pos - ((Attribute *)element)->value + 1, sizeof(char)); */

            /*         if (tmp_number_char != NULL) { */
            /*             strncpy(tmp_number_char, ((Attribute *)element)->value, unit_pos - ((Attribute *)element)->value); */
            /*             attr = create_attribute_text("height", tmp_number_char); */
            /*             insertBack(img->otherAttributes, attr); */

            /*             unit_pos = NULL; */
            /*             free(tmp_number_char); */
            /*         } */

            /*     } else { */
            /*         attr = create_attribute_text("height", ((Attribute *)element)->value); */
            /*         insertBack(img->otherAttributes, attr); */
            /*     } */
            /*     continue; */
            /* } */

            /* general case, copy element to attribute list of rectangle */
            attr = copy_attribute((Attribute *)element);
            insertBack(img->otherAttributes, attr);
        }
    }
}

void parse_group_attribute_list(Group *group, List *attributes) {
    ListIterator iter;
    void *element = NULL;
    Attribute *attr = NULL;

    if ((group != NULL) && (attributes != NULL)) {
        iter = createIterator(attributes);

        while ((element = nextElement(&iter)) != NULL) {
            /* a group does not have direct member variables,
             * only the generic attribute list */

            /* general case, copy element to attribute list of rectangle */
            attr = copy_attribute((Attribute *)element);
            insertBack(group->otherAttributes, attr);
        }
    }
}

float calc_rectangle_area(Rectangle *rect) {
    float result = 0.0;

    if (rect != NULL) {
        result = rect->width * rect->height;
    }

    return result;
}

float calc_circle_area(Circle *circ) {
    float result = 0.0;

    if (circ != NULL) {
        result = powf(circ->r, 2.0) * M_PI;
    }

    return result;
}

bool compare_rectangle_area(const void *rect1, const void *rect2) {
    bool result = false;

    if ((rect1 != NULL) && (rect2 != NULL)) {
        if ((int)ceil(calc_rectangle_area((Rectangle *)rect1)) == 
                (int)ceil(calc_rectangle_area((Rectangle *)rect2))) {
            result = true;
        }
    }

    return result;
}

bool compare_circle_area(const void *circ1, const void *circ2) {
    bool result = false;

    if ((circ1 != NULL) && (circ2 != NULL)) {
        if (ceil(calc_circle_area((Circle *)circ1)) == 
                ceil(calc_circle_area((Circle *)circ2))) {
            result = true;
        }
    }

    return result;
}

bool compare_paths(const void *path1, const void *path2) {
    bool result = false;

    if ((path1 != NULL) && (path2 != NULL)) {
        if (strcmp(((Path *)path1)->data, ((Path *)path2)->data) == 0) {
            result = true;
        }
    }

    return result;
}

bool compare_attribute_name(const void *attr1, const void *attr2) {
    bool result = false;

    if ((attr1 != NULL) && (attr2 != NULL)) {
        if (strcmp(((Attribute *)attr1)->name, ((Attribute *)attr2)->name) == 0) {
            result = true;
        }
    }

    return result;
}

int group_length(Group *grp) {
    int result = 0;

    if (grp != NULL) {
        if (grp->rectangles != NULL) {
            result += getLength(grp->rectangles);
        }

        if (grp->circles != NULL) {
            result += getLength(grp->circles);
        }

        if (grp->paths != NULL) {
            result += getLength(grp->paths);
        }

        if (grp->groups != NULL) {
            result += getLength(grp->groups);
        }
    }

    return result;
}

int num_attributes_in_rectangle(Rectangle *rect) {
    int result = 0;

    if ((rect != NULL) && (rect->otherAttributes != NULL)) {
        result = getLength(rect->otherAttributes);
    }

    return result;
}

int num_attributes_in_circle(Circle *circ) {
    int result = 0;

    if ((circ != NULL) && (circ->otherAttributes != NULL)) {
        result = getLength(circ->otherAttributes);
    }

    return result;
}

int num_attributes_in_path(Path *path) {
    int result = 0;

    if ((path != NULL) && (path->otherAttributes != NULL)) {
        result = getLength(path->otherAttributes);
    }

    return result;
}

void read_number_and_unit(char *input, float *number, char *unit) {
    char *endptr = NULL;

    if ((input != NULL) && (number != NULL)) {
        *number = strtof(input, &endptr);

        if ((unit != NULL) && (endptr != NULL)) {
            strcpy(unit, endptr);
        }
    }
}

