#include "cmdparser.h"

struct optlist_t {
    char *opt;
    int arg;
    OptList next;
};

void printAllArguments(int argc, char *argvs[])
{
    for (int i = 0; i < argc; i++) {
        printf("%d: %s\n", i, argvs[i]);
    }
}

OptList optlistnode(const char *str, size_t n, int arg, OptList next)
{
    OptList ret = (OptList)malloc(sizeof(struct optlist_t));
    ret->opt = (char *)malloc(n+1);
    memcpy(ret->opt, str, n);
    ret->opt[n] = '\0';
    ret->arg = arg;
    return ret;
}

OptList insertOptList(OptList x, const char *str, size_t n, int arg)
{
    if (x == NULL) return optlistnode(str, n, arg, NULL);
    x->next = optlistnode(str, n, arg, x->next);
    return x->next;
}

OptList destroyOptList(OptList xs)
{
    OptList next;
    while (xs != NULL) {
        next = xs->next;
        free(xs->opt);
        free(xs);
        xs = next;
    }
    return NULL;
}

char *popOptList(OptList *xs)
{
    if (xs == NULL) return NULL;
    if (*xs == NULL) return NULL;

    OptList next = (*xs)->next;
    char *ret = (*xs)->opt;
    free(*xs);
    *xs = next;
    return ret;
}

OptList lastOptList(OptList cp)
{
    if (cp == NULL) return NULL;
    while (cp->next != NULL) {
        cp = cp->next;
    }
    return cp;
}

OptList elemOptList(OptList cp, int index)
{
    if (cp == NULL) return NULL;

    for (int i = 0; cp != NULL; i++) {
        if (i == index) return cp;
        cp = cp->next;
    }
    return NULL;
}

void printOptList(CmdParser cp)
{
    if (cp == NULL) return;
    OptList xs = cp->optlist;
    while (xs != NULL) {
        printf("-%s:%d\n", xs->opt, xs->arg);
        xs = xs->next;
    }
}

void printOptArgs(CmdParser cp)
{
    if (cp == NULL) return;
    OptList xs = cp->optarg;
    if (xs == NULL) return;
    printf("%s", xs->opt);
    xs = xs->next;
    while (xs != NULL) {
        printf(",%s", xs->opt);
        xs = xs->next;
    }
}

void printopt(CmdParser cp, int optnum)
{
    if (cp == NULL) return;
    OptList opt = elemOptList(cp->optlist, optnum);
    if (opt == NULL) {
        printf("Index out of range.\n");
        return;
    }

    printf("Option: -%s\n", opt->opt);
    printf("Arguments: ");
    printOptArgs(cp);
    putchar('\n');
}

int countColon(const char *optstr)
{
    int i = 0;
    while (optstr[i] == ':') i++;
    return i;
}

OptList parseOptString(const char *optstr)
{
    char *cursol = (char *)optstr;
    OptList ret = NULL;
    OptList last = NULL;
    int fst = 1;
    int argnum = 0;
    while (*cursol != '\0') {
        if (*cursol == '|') {
            break;
        }
        argnum = countColon(cursol + 1);
        last = insertOptList(last, cursol, 1, argnum);
        if (fst) { ret = last; fst = 0; }
        cursol += argnum + 1;
    }

    //return ret;

    int i = 0;
    if (*cursol == '|') cursol++;
    while (cursol[i] != '\0') {
        if (cursol[i] == '/') {
            if (i == 0)
                goto PARSE_ERROR;
            last = insertOptList(last, cursol, i, 0);
            if (fst) { ret = last; fst = 0; }
            cursol += i + 1;
            argnum = 0;
            i = 0;
        } else if (cursol[i] == '=') {
            if (i == 0)
                goto PARSE_ERROR;
            if (cursol[i+1] == '*') {
                argnum = -1;
                if (cursol[i+2] != '\0' && cursol[i+2] != '/')
                    goto PARSE_ERROR;
            } else {
                argnum = countColon(cursol + i + 1);
                if (cursol[i+argnum+1] != '\0' && cursol[i+argnum+1] != '/') 
                    goto PARSE_ERROR;
            }
            last = insertOptList(last, cursol, i, argnum);
            if (fst) { ret = last; fst = 0; }
            cursol += (argnum == -1) ? i + 3 : i + argnum + 2;
            argnum = 0;
            i = 0;
        }
        i++;
    }

    return ret;
PARSE_ERROR:
    destroyOptList(ret);
    return NULL;
}

CmdParser createCmdParser(int argc, char *argvs[], const char *optstr)
{
    CmdParser ret = (CmdParser)malloc(sizeof(struct cmdparser_t));
    ret->argc = argc;
    ret->argvs = argvs;
    ret->optind = 1;
    ret->optlist = parseOptString(optstr);
    if (ret->optlist == NULL) {
        free(ret);
        return NULL;
    }
    ret->optarg = NULL;
    return ret;
}

OptList optsearch(CmdParser cp, const char *opt, size_t n, int *index)
{
    if (cp == NULL) return NULL;

    if (index != NULL) *index = 0;
    OptList xs = cp->optlist;
    while (xs != NULL) {
        if (strncmp(xs->opt, opt, n) == 0) {
            return xs;
        }
        (*index)++;
        xs = xs->next;
    }
    return NULL;
}

int optdevider(char c)
{
    switch (c) {
        case '\0':
        case '=':
            return 1;
        default:
            break;
    }
    return 0;
}

int argdevider(char c)
{
    switch (c) {
        case '\0':
        case ',':
            return 1;
        default:
            break;
    }
    return 0;
}

int getoptlen(const char *str)
{
    int i = 0;
    while (!optdevider(str[i])) i++;
    return i;
}

int getarglen(const char *str)
{
    int i = 0;
    while (!argdevider(str[i])) i++;
    return i;
}

int singlecharopt(const char *str)
{
    if (strlen(str) == 1) return 1;
    return 0;
}

int getoptCmd(CmdParser cp)
{
    if (cp == NULL) return -1;
    //printOptList(cp);

    cp->optarg = destroyOptList(cp->optarg);

    OptList optinfo;
    char *opthead;
    int optlen;
    int arglen;
    int i;
    OptList arglast = NULL;
    int optindex;
    while (cp->optind < cp->argc) {
        if (cp->argvs[cp->optind][0] == '-') {
            opthead = &(cp->argvs[cp->optind][1]);
            //printf("option:%s\n", opthead);
            optlen = getoptlen(opthead);
            optinfo = optsearch(cp, opthead, optlen, &optindex);
            cp->optind++;
            if (optinfo != NULL) {
                if (singlecharopt(optinfo->opt)) {
                    for (i = 0; i < optinfo->arg; i++) {
                        if (cp->optind + i >= cp->argc) {
                            fprintf(stderr, "Too few arguments. '%s' needs %d arguments.\n", 
                                    optinfo->opt, optinfo->arg);
                            return -1;
                        }
                        opthead = cp->argvs[cp->optind+i];
                        if (*opthead == '-') {
                            fprintf(stderr, "Too few arguments. '%s' needs %d arguments.\n", 
                                    optinfo->opt, optinfo->arg);
                            return -1;
                        }
                        arglast = insertOptList(arglast, opthead, strlen(opthead), 0);
                        if (i == 0) cp->optarg = arglast;
                    }
                    cp->optind += i;
                    return optindex;
                } else {
                    //printf("multicharopt\n");
                    opthead += optlen;
                    if (optinfo->arg == -1) {
                        if (*opthead == '=') {
                            opthead++;
                        }
                        i = 0;
                        while (*opthead == ',') opthead++;
                        while (*opthead != '\0') {
                            arglen = getarglen(opthead);
                            arglast = insertOptList(arglast, opthead, arglen, 0);
                            if (i == 0) { cp->optarg = arglast; i++; }
                            opthead += arglen;
                            while (*opthead == ',') opthead++;
                        }
                    } else if (optinfo->arg > 0) {
                        if (*opthead != '=') {
                            fprintf(stderr, "Too few arguments. '%s' needs %d arguments.\n", 
                                    optinfo->opt, optinfo->arg);
                            return -1;
                        }
                        opthead++;
                        i = 0;
                        while (*opthead == ',') opthead++;
                        while (*opthead != '\0') {
                            if (i >= optinfo->arg) break;
                            arglen = getarglen(opthead);
                            arglast = insertOptList(arglast, opthead, arglen, 0);
                            if (i == 0) { cp->optarg = arglast; }
                            i++;
                            opthead += arglen;
                            while (*opthead == ',') opthead++;
                        }
                        if (i < optinfo->arg) {
                            fprintf(stderr, "Too few arguments. '%s' needs %d arguments.\n", 
                                    optinfo->opt, optinfo->arg);
                            return -1;
                        }
                    }
                    return optindex;
                }
            } else {
                fprintf(stderr, "No such option: %s\n", opthead);
                return -1;
            }
        } else {
            //printf("other:%s\n", cp->argvs[cp->optind]);
            opthead = cp->argvs[cp->optind];
            if (cp->optopt == NULL) {
                if (cp->optopt == NULL) {
                    cp->optopt = insertOptList(NULL, opthead, strlen(opthead), 0);
                } else {
                    insertOptList(lastOptList(cp->optopt), opthead, strlen(opthead), 0);
                }
            }
            cp->optind++;
            return -2;
        }
    }
    return -1;
}

CmdParser destroyCmdParser(CmdParser cp)
{
    destroyOptList(cp->optlist);
    destroyOptList(cp->optarg);
    free(cp);
    return NULL;
}
