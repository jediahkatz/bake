#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "parser.h"

RULE_PARSER *init_rule_parser(char *str) {
    RULE_PARSER *rparser = (RULE_PARSER *) malloc(sizeof(RULE_PARSER));
    if (rparser == NULL) {
        printf("Malloc failed\n");
        abort();
    }
    rparser->str = str;
    rparser->pos = str;
}

RULE *get_next_rule(RULE_PARSER *rparser) {
    char *startptr = rparser->pos;
    char *endptr;
    int len;
    // Leading whitespace
    while (isspace(*startptr) || *startptr == '\n') {
        startptr++;
    }
    if (*startptr == '\0') {
        // Invalid format
        return NULL;
    }

    // Extract target filename before colon
    // TODO: is trailing whitespace allowed?
    endptr = startptr;
    while (*endptr != ':') {
        endptr++;
        if (*endptr == '\0') {
            // Invalid format
            return NULL;
        }
    }
    len = endptr - startptr + 1;
    char *target = (char *) malloc(len);
    if (target == NULL) {
        printf("Malloc failed\n");
        abort();
    }

    memcpy(target, startptr, len-1);
    target[len-1] = '\0';
    // TODO: validate target name

    PREREQS_HEAD *prereqs = (PREREQS_HEAD *) malloc(sizeof(PREREQS_HEAD));
    SLIST_INIT(prereqs);

    PREREQ *prereq_node;
    char *prereq_name;

    while (*startptr != '\n') {
        if (*startptr == '\0') {
            // Invalid format
            return NULL;
        }

        startptr++;

        // Extract next prerequisite filename
        while (isspace(*startptr)) {
            startptr++;
            if (*startptr == '\0') {
                // Invalid format
                return NULL;
            }
        }

        endptr = startptr;
        while (!isspace(*endptr) && *endptr != '\n') {
            endptr++;
            if (*endptr == '\0') {
                // Invalid format
                printf("huh?\n");
                return NULL;
            }
        }
        len = endptr - startptr + 1;
        prereq_name = (char *) malloc(len);
        if (prereq_name == NULL) {
            printf("Malloc failed\n");
            abort();
        }
        memcpy(prereq_name, startptr, len-1);
        prereq_name[len-1] = '\0';
        // TODO: validate prereq name
        
        prereq_node = (PREREQ *) malloc(sizeof(PREREQ));
        if (prereq_node == NULL) {
            printf("Malloc failed\n");
            abort();
        }
        prereq_node->filename = prereq_name;
        SLIST_INSERT_HEAD(prereqs, prereq_node, entries);

        startptr = endptr;
    }

    // Extract the recipe
    startptr = endptr + 1;
    endptr = startptr;
    while (*endptr != '\n' && *endptr != '\0') {
        endptr++;
    }

    len = endptr - startptr + 1;
    char *recipe = (char *) malloc(len);
    if (recipe == NULL) {
        printf("Malloc failed");
        abort();
    }
    memcpy(recipe, startptr, len-1);
    recipe[len-1] = '\0';

    rparser->pos = endptr;
    RULE *rule = init_rule(target, prereqs, recipe);

    return rule;
}

void main(char **args, int argc) {
    RULE_PARSER *rparser = init_rule_parser("foo: bar.c\n cc -o foo bar.c");
    RULE *rule = get_next_rule(rparser);
    print_rule(rule);
}