#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"

RULE_PARSER *init_rule_parser(char *str) {
    RULE_PARSER *rparser = (RULE_PARSER *) malloc(sizeof(RULE_PARSER));
    if (rparser == NULL) {
        printf("Malloc failed");
        abort();
    }
    rparser->str = str;
    rparser->pos = str;
}

RULE *get_next_rule(RULE_PARSER *rparser) {
    char *startptr = rparser->pos;
    char *endptr;
    int len;
    while (isspace(*startptr) || isnewline(*startptr)) {
        startptr++;
    }
    if (*startptr == NULL) {
        return NULL;
    }

    endptr = startptr;
    while (*endptr != ':') {
        endptr++;
    }
    len = endptr - startptr + 1;
    char *target = (char *) malloc(len);
    if (target == NULL) {
        printf("Malloc failed");
        abort();
    }
    memcpy(target, startptr, len-1);
    target[len-1] = NULL;
    // TODO: validate target name

    PREREQS_HEAD *prereqs;
    SLIST_INIT(prereqs);
    
    PREREQ *prereq_node;
    char *prereq_name;
    startptr = endptr + 1;
    while (!isnewline(startptr)) {
        while (isspace(*startptr)) {
            startptr++;
        }
        // TODO: what if we hit null?

        endptr = startptr;
        while (!isspace(endptr)) {
            endptr++;
        }
        len = endptr - startptr + 1;
        prereq_name = (char *) malloc(len);
        if (prereq_name == NULL) {
            printf("Malloc failed");
            abort();
        }
        memcpy(prereq_name, startptr, len-1);
        prereq_name[len-1] = NULL;
        
        prereq_node = (PREREQ *) malloc(sizeof(PREREQ));
        if (prereq_node == NULL) {
            printf("Malloc failed");
            abort();
        }
        prereq_node->filename = prereq_name;
        SLIST_INSERT_HEAD(prereqs, prereq_node, entries);
    }
    //RULE *r = init_rule(target);
    
}