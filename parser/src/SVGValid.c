#include <SVGParser.h>

#include <SVGValidation.h>
#include <SVGHelpers.h>
#include <XMLGeneration.h>

SVGimage *createValidSVGimage(char* fileName, char* schemaFile) {
    SVGimage *img = NULL;
    xmlDoc *doc = NULL;
    xmlNode *root_element = NULL;

    if ((fileName != NULL) && (strlen(fileName) > 0) && 
        (schemaFile != NULL) && (strlen(schemaFile))) {

        LIBXML_TEST_VERSION

        doc = xmlReadFile(fileName, NULL, 0);

        if (doc != NULL) {
            if (validate_SVG_against_schema(doc, schemaFile) == true) {
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
        }

        /*
         *Free the global variables that may
         *have been allocated by the parser.
         */
        xmlCleanupParser();

    }

    return img;
}

bool writeSVGimage(SVGimage* doc, char* fileName) {
    bool result = false;
    int retCode = 0;
    xmlDocPtr xdoc = NULL;

    if ((doc != NULL) && (fileName != NULL)) {
        LIBXML_TEST_VERSION

        /* XML Tree needs to be generated first ! */
        xdoc = createDocument(doc);

        /* Now save it to a file */
        retCode = xmlSaveFormatFileEnc(fileName, xdoc, "UTF-8", 1);

        if (retCode > 0) {
            result = true;
        }

        xmlFreeDoc(xdoc);
    }

    return result;
}

bool validateSVGimage(SVGimage* doc, char* schemaFile) {
    bool result = false;
    xmlDocPtr xdoc = NULL;

    if ((doc != NULL) && (schemaFile != NULL)) {
        LIBXML_TEST_VERSION

        result = validate_SVG_contents(doc);

        if (result == true) {
            xdoc = createDocument(doc);
            result = validate_SVG_against_schema(xdoc, schemaFile);
        }
    }

    return result;
}

bool validateSVGimageWithPath(char *filename, char *schemaFile) {
    SVGimage *img;
    bool result = false;

    if ((filename != NULL) && (schemaFile != NULL)) {
        img = createValidSVGimage(filename, schemaFile);

        if (img != NULL) {
            result = true;
            deleteSVGimage(img);
        }
    }

    return result;
}

bool validate_SVG_against_schema (xmlDocPtr doc, char *schemaFile) {
    bool result = false;
    int validationResult = 0;
    xmlSchemaPtr schema = NULL;
    xmlSchemaParserCtxtPtr ctxt;
    xmlSchemaValidCtxtPtr validCtxt;
    
    if ((doc != NULL) && (schemaFile != NULL)) {
        xmlLineNumbersDefault(1);

        ctxt = xmlSchemaNewParserCtxt(schemaFile);

        xmlSchemaSetParserErrors(ctxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        schema = xmlSchemaParse(ctxt);
        xmlSchemaFreeParserCtxt(ctxt);

        validCtxt = xmlSchemaNewValidCtxt(schema);
        xmlSchemaSetValidErrors(validCtxt, (xmlSchemaValidityErrorFunc) fprintf, (xmlSchemaValidityWarningFunc) fprintf, stderr);
        validationResult = xmlSchemaValidateDoc(validCtxt, doc);

        if (validationResult == 0) {
            result = true;
        } else if (validationResult > 0) {
            /* document failed to validate */
        } else {
            /* internal error during validation */
        }

        xmlSchemaFreeValidCtxt(validCtxt);

        if (schema != NULL) {
            xmlSchemaFree(schema);
            xmlSchemaCleanupTypes();
        }
    }

    return result;
}

bool validate_SVG_contents (SVGimage *img) {
    /* ensure radii and length > 0 */
    /* ensure no NULL pointers */
    bool result = true;

    if (result == true) {
        result = validate_attribute_list(img->otherAttributes);
    }

    if ((result == true) && (img->rectangles != NULL)) {
        result = validate_rectangle_list(img->rectangles);
    } else {
        result = false;
    }

    if ((result == true) && (img->circles != NULL)) {
        result = validate_circle_list(img->circles);
    } else {
        result = false;
    }

    if ((result == true) && (img->paths != NULL)) {
        result = validate_path_list(img->paths);
    } else {
        result = false;
    }

    if ((result == true) && (img->groups != NULL)) {
        /* Attention! Recursive call hidden here ! */
        result = validate_group_list(img->groups);
    } else {
        result = false;
    }

    return result;
}

bool validate_rectangle (Rectangle *rect) {
    bool result = true;

    if ((rect->width < 0.0) || (rect->height < 0.0)) {
        result = false;
    }

    if (result == true) {
        if (rect->otherAttributes != NULL) {
            result = validate_attribute_list(rect->otherAttributes);
        } else {
            result = false;
        }
    }

    return result;
}

bool validate_rectangle_list (List *rectangles) {
    bool result = true;
    ListIterator iter;
    Rectangle *rect = NULL;

    iter = createIterator(rectangles);

    while ((rect = (Rectangle *)nextElement(&iter)) != NULL) {
        result = validate_rectangle(rect);

        if (result == false) {
            /* no need to continue upon the first negative result */
            break;
        }
    }

    return result;
}

bool validate_circle (Circle *circ) {
    bool result = true;

    if (circ->r < 0.0) {
        result = false;
    }

    if (result == true) {
        if (circ->otherAttributes != NULL) {
            result = validate_attribute_list(circ->otherAttributes);
        } else {
            result = false;
        }
    }

    return result;
}

bool validate_circle_list (List *circles) {
    bool result = true;
    ListIterator iter;
    Circle *circ = NULL;

    iter = createIterator(circles);

    while ((circ = (Circle *)nextElement(&iter)) != NULL) {
        result = validate_circle(circ);

        if (result == false) {
            /* no need to continue upon the first negative result */
            break;
        }
    }

    return result;
}

bool validate_path (Path *path) {
    bool result = true;

    if (path->data == NULL) {
        result = false;
    }

    if (result == true) {
        if (path->otherAttributes != NULL) {
            result = validate_attribute_list(path->otherAttributes);
        } else {
            result = false;
        }
    }

    return result;
}

bool validate_path_list (List *paths) {
    bool result = true;
    ListIterator iter;
    Path *path = NULL;

    iter = createIterator(paths);

    while ((path = (Path *)nextElement(&iter)) != NULL) {
        result = validate_path(path);

        if (result == false) {
            /* no need to continue upon the first negative result */
            break;
        }
    }

    return result;
}

bool validate_group (Group *group) {
    bool result = true;

    if (result == true) {
        result = validate_attribute_list(group->otherAttributes);
    }

    if ((result == true) && (group->rectangles != NULL)) {
        result = validate_rectangle_list(group->rectangles);
    } else {
        result = false;
    }

    if ((result == true) && (group->circles != NULL)) {
        result = validate_circle_list(group->circles);
    } else {
        result = false;
    }

    if ((result == true) && (group->paths != NULL)) {
        result = validate_path_list(group->paths);
    } else {
        result = false;
    }

    if ((result == true) && (group->groups != NULL)) {
        /* Attention! Recursive call hidden here ! */
        result = validate_group_list(group->groups);
    } else {
        result = false;
    }

    return result;
}

bool validate_group_list (List *groups) {
    bool result = true;
    ListIterator iter;
    Group *group = NULL;

    iter = createIterator(groups);

    while ((group = (Group *)nextElement(&iter)) != NULL) {
        result = validate_group(group);

        if (result == false) {
            /* no need to continue upon the first negative result */
            break;
        }
    }

    return result;
}

bool validate_attribute (Attribute *attr) {
    bool result = false;

    if ((attr != NULL) && (attr->name != NULL) && (attr->value != NULL)) {
        result = true;
    }

    /* An additional check for strlen(attr->name) > 0 would make a lot of sense,
     * but this is not specified in SVGParser.h. Maybe this is caught by the
     * validation against the schema file anyway. */

    return result;
}

bool validate_attribute_list (List *attributes) {
    bool result = true;
    ListIterator iter;
    Attribute *attr = NULL;

    iter = createIterator(attributes);

    while ((attr = (Attribute *)nextElement(&iter)) != NULL) {
        result = validate_attribute(attr);

        if (result == false) {
            /* no need to continue upon the first negative result */
            break;
        }
    }

    return result;
}

