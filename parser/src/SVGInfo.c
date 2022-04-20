#include <SVGParser.h>

/* private functions */
int num_rectangles_in_group(Group *grp);
int num_circles_in_group(Group *grp);
int num_paths_in_group(Group *grp);
int num_groups_in_group(Group *grp);

int num_rectangles_in_group(Group *grp) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if (grp != NULL) {
        result = getLength(grp->rectangles);

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_rectangles_in_group((Group *)element);
        }
    }

    return result;
}

int num_circles_in_group(Group *grp) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if (grp != NULL) {
        result = getLength(grp->circles);

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_circles_in_group((Group *)element);
        }
    }

    return result;
}

int num_paths_in_group(Group *grp) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if (grp != NULL) {
        result = getLength(grp->paths);

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_paths_in_group((Group *)element);
        }
    }

    return result;
}

int num_groups_in_group(Group *grp) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;

    if (grp != NULL) {
        result = getLength(grp->groups);

        iter = createIterator(grp->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_groups_in_group((Group *)element);
        }
    }

    return result;
}

int num_rectangles_in_image(char *filename) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;
    SVGimage *img;

    img = createSVGimage(filename);

    if (img != NULL) {
        result = getLength(img->rectangles);

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_rectangles_in_group((Group *)element);
        }

        deleteSVGimage(img);
    }

    return result;
}

int num_circles_in_image(char *filename) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;
    SVGimage *img;

    img = createSVGimage(filename);

    if (img != NULL) {
        result = getLength(img->circles);

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_circles_in_group((Group *)element);
        }

        deleteSVGimage(img);
    }

    return result;
}

int num_paths_in_image(char *filename) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;
    SVGimage *img;

    img = createSVGimage(filename);

    if (img != NULL) {
        result = getLength(img->paths);

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_paths_in_group((Group *)element);
        }

        deleteSVGimage(img);
    }

    return result;
}

int num_groups_in_image(char *filename) {
    int result = 0;
    ListIterator iter;
    void *element = NULL;
    SVGimage *img;

    img = createSVGimage(filename);

    if (img != NULL) {
        result = getLength(img->groups);

        iter = createIterator(img->groups);

        while ((element = nextElement(&iter)) != NULL) {
            result += num_groups_in_group((Group *)element);
        }

        deleteSVGimage(img);
    }

    return result;
}

