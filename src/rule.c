#include "rule.h"
#include <stdlib.h>
#include <stdio.h>

RULE *init_rule(char *target, PREREQS_HEAD *prereqs_list, char *recipe) {
    RULE *rule = (RULE *) malloc(sizeof(RULE));
    rule->target = target;
    rule->prereqs_list = prereqs_list;
    rule->recipe = recipe;
    return rule;
}

void print_rule(RULE *rule) {
    if (rule == NULL) {
        printf("NULL RULE\n");
    } else {
        printf("TARGET: %s\n", rule->target);
        PREREQ *prereq;
        printf("PREREQS:\n");
        SLIST_FOREACH(prereq, rule->prereqs_list, entries) {
            printf("-- %s\n", prereq->filename);
        }
        printf("RECIPE: %s\n", rule->recipe);
    }
}