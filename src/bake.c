#include <stdio.h>
#include <search.h>
#include <string.h>
#include <unistd.h>
#include "parser.h"

void main(int argc, char **argv) {
    // Read Bakefile into string buffer
    FILE *f = fopen("Bakefile", "rb");
    if (f == NULL) {
        perror("Can't open Bakefile:");
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
    RULE *rule;
    hcreate(numrules);
    while ((rule = get_next_rule(rparser)) != NULL) {
        // Target the first rule in the Bakefile if none was specified in CLI args
        if (bake_target == NULL) {
            bake_target = rule->target;
        }
        ENTRY entry = { .key = rule->target, .data = rule };
        // TODO: check for duplicates?
        hsearch(entry, ENTER);
    }

    bake(bake_target);
}

/** Recursively bake all dependencies. Then, if this target was out-of-date
 *  (i.e., it does not exist or any of its dependencies or descendants are newer),
 *  run the recipe for rule associated with the current target.
 *  Returns 1 if this target was out-of-date.
 */
int bake(char *target) {
    ENTRY entry = { .key = target };
    RULE *rule = hsearch(entry, FIND);
    int out_of_date = 0;
    if (rule != NULL) {
        PREREQ *prereq;
        SLIST_FOREACH(prereq, rule->prereqs_list, entries) {
            out_of_date |= bake(prereq->filename);
        }
    }
    // Without a rule, the target file is only out-of-date if it doesn't exist
    out_of_date |= (access(target, F_OK ) == -1);
    if (out_of_date && rule != NULL) {
        system(rule->recipe);
    }
    return out_of_date;
}