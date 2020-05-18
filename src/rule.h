#include <sys/queue.h>

/** A prerequisite of a rule, as an slist entry. **/
typedef struct prereq {
    char *filename; // the prerequisite file name
    SLIST_ENTRY(prereq) entries;
} PREREQ;

typedef SLIST_HEAD(prereqs_head, prereq) PREREQS_HEAD;

/** Represents a rule in the Bakefile. */
typedef struct rule {
    char *target;   // the (single) target of the rule, which may be phony
    PREREQS_HEAD *prereqs_list;
    char *recipe;
} RULE;

/** Initialize a new rule with given target, empty list of prereqs. */
RULE *init_rule(char *target);
