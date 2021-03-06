/*
 * ProviewR   Open Source Process Control.
 * Copyright (C) 2005-2018 SSAB EMEA AB.
 *
 * This file is part of ProviewR.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ProviewR. If not, see <http://www.gnu.org/licenses/>
 *
 * Linking ProviewR statically or dynamically with other modules is
 * making a combined work based on ProviewR. Thus, the terms and
 * conditions of the GNU General Public License cover the whole
 * combination.
 *
 * In addition, as a special exception, the copyright holders of
 * ProviewR give you permission to, from the build function in the
 * ProviewR Configurator, combine ProviewR with modules generated by the
 * ProviewR PLC Editor to a PLC program, regardless of the license
 * terms of these modules. You may copy and distribute the resulting
 * combined work under the terms of your choice, provided that every
 * copy of the combined work is accompanied by a complete copy of
 * the source code of ProviewR (the version used to produce the
 * combined work), being distributed under the terms of the GNU
 * General Public License plus this exception.
 */

/*
* Filename:
*   tools_msg2cmsg.c
*
* Revision history:
*   Rev     Edit    Date        Who  Comment
*   ------- ----    ----------  ---- -------
*   X0.1       1    1996-10-01  ML   Created
*
* Description:	Creates a .c_msg-file and a .h-file from a .msg-file
*/

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pwr_lst.h"
#include "pwr_msg.h"
#include "msg2cmsg.h"

#define MSG_NEW_STRING(str)                                                    \
  strncpy(malloc(strlen(str) + 1), str, strlen(str) + 1)

typedef struct s_FacilityCB sFacilityCB;
typedef struct s_MsgCB sMsgCB;

struct s_MsgCB {
  struct LstHead MsgL;
  msg_sMsg m;
  msg_eSeverity Severity;
};

struct s_FacilityCB {
  struct LstHead FacL;
  struct LstHead MsgH;
  msg_sFacility f;
};

/* defined by lexyy.c */
extern int lineno;

static int SyntaxError;

static struct LstHead lFacH;

/*
 * Local functions
 */

static void WriteFiles(char* fname, FILE* cfp, FILE* hfp);

static void TranslateFormatSpec(const char* msgstr, char** transstr);

/*
* Main
*/
int main(int argc, char** argv)
{
  extern FILE* yyin;
  extern int yylex();

  FILE* cfp = NULL;
  FILE* hfp = NULL;

  if (argc != 4) {
    printf("Usage: co_msg2cmsg msg-file c_msg-file h_file\n");
    exit(2);
  }

  if (!(yyin = fopen(argv[1], "r"))) {
    printf("Can't open input file: %s\n", argv[1]);
    exit(2);
  }

  if (!(cfp = fopen(argv[2], "w"))) {
    printf("Can't open c_msg-output file: %s\n", argv[2]);
    fclose(yyin);
    exit(2);
  }

  if (!(hfp = fopen(argv[3], "w"))) {
    printf("Can't open h-output file: %s\n", argv[3]);
    fclose(yyin);
    fclose(cfp);
    exit(2);
  }

  LstInit(&lFacH);
  SyntaxError = 0;
  lineno = 1;
  yylex();

  if (!SyntaxError) {
    char fname[256];
    char* p;

    if ((p = strrchr(argv[2], '/')))
      strncpy(fname, p + 1, sizeof(fname));
    else
      strncpy(fname, argv[2], sizeof(fname));

    if ((p = strchr(fname, '.')))
      *p = '\0';

    WriteFiles(fname, cfp, hfp);
  }

  fclose(yyin);
  fclose(cfp);
  fclose(hfp);

  exit(EXIT_SUCCESS);
}

/*
 * Routines which are called by lexyy.c
 */

void lex_FacName(const char* FacName)
{
  sFacilityCB* facp = (sFacilityCB*)calloc(1, sizeof(sFacilityCB));

  LstInit(&facp->MsgH);
  facp->f.FacName = MSG_NEW_STRING(FacName);

  LstInsert(&lFacH, &facp->FacL);
}

void lex_FacNum(int FacNum)
{
  LstEntry(lFacH.prev, sFacilityCB, FacL)->f.FacNum = FacNum;
}

void lex_FacPrefix(const char* Prefix)
{
  LstEntry(lFacH.prev, sFacilityCB, FacL)->f.Prefix = MSG_NEW_STRING(Prefix);
}

void lex_MsgName(const char* MsgName)
{
  sMsgCB* msgp = (sMsgCB*)calloc(1, sizeof(sMsgCB));
  int i;
  int len = strlen(MsgName);
  msgp->m.MsgName = malloc(len + 1);

  for (i = 0; i < len; i++)
    msgp->m.MsgName[i] = toupper(MsgName[i]);
  msgp->m.MsgName[i] = '\0';

  LstInsert(&LstEntry(lFacH.prev, sFacilityCB, FacL)->MsgH, &msgp->MsgL);
}

void lex_MsgText(const char* Text)
{
  struct LstHead* ml = LstEntry(lFacH.prev, sFacilityCB, FacL)->MsgH.prev;

  TranslateFormatSpec(
      Text, &LstEntry(ml, sMsgCB, MsgL)->m.MsgTxt); /* convert any VMS-style form spec */
}

void lex_MsgSeverity(msg_eSeverity Severity)
{
  struct LstHead* ml = LstEntry(lFacH.prev, sFacilityCB, FacL)->MsgH.prev;
  LstEntry(ml, sMsgCB, MsgL)->Severity = Severity;
}

void lex_LexError(int Lineno, char* Str)
{
  printf("LexError: line %d, '%s'\n", Lineno, Str);
  SyntaxError = 1;
}

/*
 * Local routines
 */
static void WriteFiles(char* fname, FILE* cfp, FILE* hfp)
{
  struct LstHead * fl;
  struct LstHead * ml;
  int idx;
  int facid;
  char prefix[32];
  char name[64];
  char msgName[64];
  int msg;

  fprintf(hfp, "#ifndef %s_h\n", fname);
  fprintf(hfp, "#define %s_h\n\n", fname);

  LstForEach(fl, &lFacH) {
    facid = 0x800 + LstEntry(fl, sFacilityCB, FacL)->f.FacNum;
    snprintf(name, sizeof(name), "%s_FACILITY", LstEntry(fl, sFacilityCB, FacL)->f.FacName);
    fprintf(hfp, "#define %-29s %9d /* x%08x */\n", name, facid, facid);
    facid = facid << 16;

    if (LstEntry(fl, sFacilityCB, FacL)->f.Prefix)
      strncpy(prefix, LstEntry(fl, sFacilityCB, FacL)->f.Prefix, sizeof(prefix));
    else
      snprintf(prefix, sizeof(prefix), "%s_", LstEntry(fl, sFacilityCB, FacL)->f.FacName);

    snprintf(msgName, sizeof(msgName), "%smsg", LstEntry(fl, sFacilityCB, FacL)->f.FacName);
    fprintf(cfp, "static msg_sMsg %s[] = {\n", msgName);

    idx = 1;
    LstForEach(ml, &LstEntry(fl, sFacilityCB, FacL)->MsgH) {
      if (idx != 1)
        fprintf(cfp, ",\n");

      msg = facid + 0x8000 + (idx << 3) + LstEntry(ml, sMsgCB, MsgL)->Severity;
      snprintf(name, sizeof(name), "%s%s", prefix, LstEntry(ml, sMsgCB, MsgL)->m.MsgName);
      fprintf(hfp, "#define %-29s %9.9d /* x%08x */\n", name, msg, msg);
      fprintf(cfp, "\t{\"%s\", \"%s\"}", LstEntry(ml, sMsgCB, MsgL)->m.MsgName,
          LstEntry(ml, sMsgCB, MsgL)->m.MsgTxt);
      idx++;
    }
    fprintf(cfp, "\n};\n\n");

    fprintf(cfp, "static msg_sFacility %sfacility[] = {\n\t",
        LstEntry(fl, sFacilityCB, FacL)->f.FacName);
    fprintf(cfp, "{%d, \"%s\", \"%s\", MSG_NOF(%s), %s}\n",
        LstEntry(fl, sFacilityCB, FacL)->f.FacNum, LstEntry(fl, sFacilityCB, FacL)->f.FacName, prefix, msgName, msgName);
    fprintf(cfp, "};\n\n");
  }

  fprintf(hfp, "\n#endif\n");
}

/*
 * TranslateFormatSpec
 *
 *  search the msgstr and replace any occurance of VMS-like specifiers
 *  (!AF !UL !XL, are replaced by %s %u %x)
 *
 */
static void TranslateFormatSpec(const char* msgstr, char** transstr)
{
  char* l;
  const char* m = msgstr + 1; /* Skip '<' */
  int extra = 0;
  int len = 0;

  /* '"' will be substituted with  '\"', allocate space for it */
  while (*m) {
    if (*m == '"')
      extra++;
    m++;
    len++;
  }

  *transstr = (char*)malloc(len + extra + 1);
  l = *transstr;
  m = msgstr + 1; /* Skip '<' */

  while (*m != '\0') {
    if (*m == '"') {
      *l++ = '\\';
      *l++ = '"';
      m++;
    } else if (*m++ == '!') {
      /*
       * could be a format spec, if so we also skip '!'
       */
      if (*m == 'A' && *(m + 1) == 'F') {
        *l++ = '%';
        m++;
        *l++ = 's';
        m++;
      } else if (*m == 'U' && *(m + 1) == 'L') {
        *l++ = '%';
        m++;
        *l++ = 'u';
        m++;
      } else if (*m == 'X' && *(m + 1) == 'L') {
        *l++ = '%';
        m++;
        *l++ = 'x';
        m++;
      } else
        *l++ = '!'; /* nope, put back the '!' */

    } else /* just copy */
      *l++ = *(m - 1);
  }
  *(l - 1) = '\0'; /* Skip '>' */
}
