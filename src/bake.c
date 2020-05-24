#define _GNU_SOURCE
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
int bake(char *target, struct hsearch_data *rule_table, struct hsearch_data *memo_table) {
    ENTRY search_entry = {.key = target};
    ENTRY *entry;
    hsearch_r(search_entry, FIND, &entry, memo_table);
    if (entry != NULL) {
        printf("Already baked %s (%sout-of-date)", target, entry->data ? "" : "not ");
        return *((int *) entry->data);
    }

    hsearch_r(search_entry, FIND, &entry, rule_table);
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
            out_of_date |= bake(prereq->filename, rule_table, memo_table);
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
    ENTRY memo_entry = {.key = target, .data=(int *) &out_of_date};
    ENTRY *garbage;
    hsearch_r(memo_entry, ENTER, &garbage, memo_table);
    return out_of_date;
}

int main(int argc, char **argv) {
    // Read Bakefile into string buffer
    FILE *f = fopen("Bakefile", "rb");
    if (f == NULL) {
        perror("Can't open Bakefile:");
    }
    // Get length of file and count number of ':'
    int c, fsize = 0, n_targets = 0;
    while ((c = fgetc(f)) != EOF) {
        if (c == ':') n_targets++;
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
    struct hsearch_data rule_table = (struct hsearch_data) {0};
    hcreate_r(n_targets, &rule_table);
    int n_prerequisites = 0;
    while ((rule = get_next_rule(rparser, &n_prerequisites)) != NULL) {
        // Target the first rule in the Bakefile if none was specified in CLI args
        if (bake_target == NULL) {
            bake_target = rule->target;
        }
        ENTRY entry = { .key = rule->target, .data = rule };
        // TODO: check for duplicates?
        ENTRY *garbage;
        hsearch_r(entry, ENTER, &garbage, &rule_table);
    }
    free(rparser->str);
    free(rparser);

    // This table keeps memoizes the return value of baking targets 
    // (and prerequisites that do not appear as targets in the Bakefile)
    struct hsearch_data memo_table = (struct hsearch_data) {0};
    hcreate_r(n_prerequisites, &memo_table);
    if (!bake(bake_target, &rule_table, &memo_table)) {
        printf("bake: '%s' is up to date.\n", bake_target);
    }
    
    // Note: this does not free the RULE structs
    // But program exits now so just let the OS do it...?
    hdestroy_r(&rule_table);
    return 0;
}