#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**************************************************************
  CmdParser format

  -'|' の手前にある文字列は <unistd.h> のgetoptと同じ
   フォーマットです.拡張として':'1個につき引数1個を取ること
   ができ,複数の引数を取るオプションも定義できます.
  -'|' の後にある文字列は複数文字で構成される識別子を持つ
   オプションを定義することができます.
   '='のあとが引数になっており,同様に':'で引数の個数を
   取ることができます.'*'を書くと任意個の引数となります.
   実際の入力では引数間を','で区切ります.


  -Example
    CmdParser cmd = createCmdParser("abcd:ef::|hoge/fuga=:/piyo=*");

    $> ./tako -a -d ika -f ebi kai -fuga=test1 -piyo=aka,ao,midori others

**************************************************************/

typedef struct optlist_t* OptList;
typedef struct cmdparser_t* CmdParser;
struct cmdparser_t {
    int argc;
    char **argvs;
    OptList optlist;
    OptList optarg;
    OptList optopt;
    int optind;
    int opterr;
};

void printAllArguments(int argc, char *argvs[]);
CmdParser createCmdParser(int argc, char *argvs[], const char *optstr);
int getoptCmd(CmdParser cp);
CmdParser destroyCmdParser(CmdParser cp);
void printOptList(CmdParser cp);
void printopt(CmdParser cp, int optnum);
