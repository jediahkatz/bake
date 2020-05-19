#include <stdlib.h>
#include <stdio.h>
#include <search.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include "parser.h"

/** Recursively bake all dependencies. Then, if this target was out-of-date
 *  (i.e., it does not exist or any of its dependencies or descendants are newer),
 *  run the recipe for rule associated with the current target.
 *  Returns 1 if this target was out-of-date.
 */
int bake(char *target) {
    ENTRY search_entry = {.key = target};
    ENTRY *entry = hsearch(search_entry, FIND);
    RULE *rule;
    // Without a rule, the target file is only out-of-date if it doesn't exist
    int target_exists = (access(target, F_OK ) != -1);
    int out_of_date = !target_exists;
    struct stat target_attr, prereq_attr;
    if (target_exists) {
        stat(target, &target_attr);
    }
    if (entry != NULL) {
        rule = entry->data;
        PREREQ *prereq;
        // TODO: prevent duplicate calls to bake (i.e. if underlying graph is not a tree)
        SLIST_FOREACH(prereq, rule->prereqs_list, entries) {
            // The target file is out-of-date if any of its prerequisites are...
            out_of_date |= bake(prereq->filename);
            // ...or if a prerequisite's file was modified after the target file
            if (target_exists && (access(prereq->filename, F_OK ) != -1)) {
                stat(prereq->filename, &prereq_attr);
                out_of_date |= (difftime(prereq_attr.st_mtime, target_attr.st_mtime) > 0);
            }
        }
    }
    if (out_of_date && entry != NULL) {
        printf("%s\n", rule->recipe);
        system(rule->recipe);
    }
    if (out_of_date) {
        printf("Re-baking %s\n", target);
    }
    return out_of_date;
}

int main(int argc, char **argv) {
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
    free(rparser->str);
    free(rparser);

    if (!bake(bake_target)) {
        printf("bake: '%s' is up to date.\n", bake_target);
    }
    
    // Note: this does not free the RULE structs
    // But program exits now so just let the OS do it...?
    hdestroy();
    return 0;
}