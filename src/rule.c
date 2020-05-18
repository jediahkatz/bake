#include "rule.h"
#include <stdlib.h>
#include <stdio.h>

RULE *init_rule(char *target, PREREQS_HEAD *prereqs_list, char *recipe) {
    RULE *rule = (RULE *) malloc(sizeof(RULE));
    rule->target = target;
    rule->prereqs_list = prereqs_list;
    rule->recipe = recipe;
}

void print_rule(RULE *rule) {
    if (rule == NULL) {
        printf("NULL RULE\n");
    } else {
        printf("\nTARGET: %s\nRECIPE: %s\n", rule->target, rule->recipe);
    }
}