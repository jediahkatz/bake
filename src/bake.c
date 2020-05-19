#include <stdio.h>
#include <search.h>
#include <string.h>
#include "parser.h"

void main(int argc, char **argv) {
    // Read bakefile into string buffer
    FILE *f = fopen("bakefile.txt", "rb");
    if (f == NULL) {
        perror("Can't open bakefile:");
    }
    // Get length of file and count number of ':'
    int c, fsize = 0, numrules = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == ':') numrules++;
        fsize++;
    }
    rewind(f); 

    char *bakefile = malloc(fsize + 1);
    fread(bakefile, 1, fsize, f);
    fclose(f);

    bakefile[fsize] = 0;

    // Extract CLI target if applicable
    char *bake_target = NULL;
    if (argc == 2) {
        bake_target = argv[1];
    } else if (argc > 2) {
        printf("Error: too many arguments to bake");
        abort();
    }

    // Parse bakefile into rules and map targets to rule
    RULE_PARSER *rparser = init_rule_parser(bakefile);
    RULE *rule, *rule_to_bake = NULL;
    hcreate(numrules);
    while ((rule = get_next_rule(rparser)) != NULL) {
        // Save the target rule specified in CLI args (or the first rule if none specified)
        if (rule_to_bake == NULL && (bake_target == NULL || !strcmp(rule->target, bake_target))) {
            rule_to_bake = rule;
        }
        ENTRY entry = { .key = rule->target, .data = rule };
        // TODO: check for duplicates?
        hsearch(entry, ENTER);
    }

    
}