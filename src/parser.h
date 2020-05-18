#include "rule.h"

/** A parser for a string containing a list of rules. */
typedef struct rule_parser {
    char *str;  // string to parse containing rules
    char *pos;  // the current position in the string
} RULE_PARSER;

/** Initialize a new rule parser. */
RULE_PARSER *init_rule_parser(char *str);

/** Free the memory associated with a rule parser. Does not free the parsed string. */
// void free_rule_parser(RULE_PARSER *rparser);

/** Get the next rule, or NULL if there are no rules remaining. */
RULE *get_next_rule(RULE_PARSER *rparser);