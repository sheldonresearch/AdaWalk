This is effectively the 'dict.c' program, but modified so that if the
file mrc2.dct has been compressed by the unix-compatible 'compress'
program, it still works transparently.  This means that the 12Mb file
can be stored as 2.5Mb and retain full functionality.

Please pass it around people who you know are using mrc2.dct for field
trials, and if there are no problems I recommend you replace the dict.c
in the archive with this one - getting 10Mb of disk space back is worth
the effort :-)

-------------cut here-----cut here-----cut here-----cut here-----
#!/bin/sh
# shar:	Shell Archiver
#	Run the following text with /bin/sh to create:
#	dict.c #	zcat.c #	zlib.c #	zlib.h 
cat - << \SHAR_EOF > dict.c
/*#define COMPRESSION*/     /* Allows you to 'compress mrc2.dct' */

/* Program to select words from MRC psycholinguistic database */
/* Mike Wilson 24 July 1986 MRC Applied Psychology Unit, Cambridge*/

/* A few modifications for portability, but mainly for working with
   compressed files, by G.Toal <gtoal@ed.ac.uk> 13/11/90.
 */

#define DATAFILE "mrc2.dct"
/*#define DATAFILE "short" ... this is redundant with compression! */
#define MAXREC 200

#include <stdio.h>
#ifdef COMPRESSION
#include "zlib.h"
#include "zlib.c"       /* Saves linking */
#else
#include <string.h>
#define ZFILE FILE
#define zfgets safefgets
#define zfopen fopen

#ifndef __STDC__
char *safefgets(line, len, zfp)
   char *line;
   int len;
   FILE *zfp;
#else
char *safefgets(char *line, int len, FILE *zfp)
#endif
{
/* Supply my own in case problems with lack of cr/lf on dos if file
   had been copied from unix... */
int c, pos = 0;

   for (;;) {
      c = fgetc(zfp);
      if (c == EOF)
         return (NULL);
      c &= 255;
      if (c == 13)
         continue;
      line[pos] = c;
      if (pos + 1 == len) {
         /* Too long! */
         break;
      }
      pos++;
      if (c == '\n')
         break;
   }
   line[pos] = '\0';
   /* fprintf(stderr, "<<%s>>\n", line); */
   return (line);
}

#endif
/* #define DEBUG */

/* Comment this out if strchr isn't found at link time */
#define index(s,c) strchr(s,c)
#ifdef __STDC__
#include <stdlib.h>
#endif

#ifndef __STDC__
int itoaa( /* int, int, char* */);
bad( /* char *, char * */ );
int inrange( /* int, int, int */ );
int linrange( /* long, long, long */ );
int outstring( /* char, char[] */ );
usage();
#else
int itoaa(int s, int e, char *line);
void bad(char *v, char *s);
int inrange(int imax, int val, int imin);
int linrange(long lmax, long val, long lmin);
int outstring(char string[], char c);
void usage(void);
#endif

#ifndef __STDC__
int main(argc, argv)
   int argc;
   char *argv[];
#else
int main(int argc, char *argv[])
#endif

{

int i, j, nf, wt, w, newinfile, nlet, nphon, nsyl, kfncats, kfnsamp;
int fam, conc, imag, meanc, meanp, aoa, brownf, whole_line;
static int outa, outb, outc, outd, oute, outf, outg, outh, outi, outj;
static int outk, outl, outm, outn, outo, outp, outq, outr, outs, outt;
static int outu, outv, outw, outx, outy, outz;
int nlet_max, nlet_min, nphon_max, nphon_min, nsyl_max, nsyl_min, max_kfncats;
int min_kfncats, brownf_max, brownf_min, max_kfnsamp, min_kfnsamp, fam_max;
int fam_min, conc_max, conc_min, imag_max, imag_min, meanc_max, meanc_min;
int meanp_max, meanp_min, aoa_max, aoa_min, alph_max, alph_min, status_max;
int status_min;
long kffreq_max, kffreq, t_l_max, t_l_min, min_kffreq, t_l;
char tq2, wtype, pdwtype, alphsyl, status, varp, irreg, outtag, parse, plural;
char cap;
char c, *p, *s, *infile, *PSin, *PSout, *PDin, *PDout, *ALPHin, *ALPHout;
char *STATUSin, *STATUSout, *TYPEin, *TYPEout, *VARin, *VARout, *IRREGin;
char *IRREGout, *CAPin, *CAPout, word[40], phon[60], dphon[60], stress[10];
char *temp;
char line[MAXREC];      /* input line */
ZFILE *zfp;

/*default maxima and minima for values*/

   PSin = "NJVARCUIPO ";
   PSout = "#";
   PDin = "NJVO ";
   PDout = "#";
   TYPEin = "Q2 ";
   TYPEout = "#";
   VARin = "BO ";
   VARout = "#";
   IRREGin = "ZBYPN ";
   IRREGout = "#";
   CAPin = " C";
   CAPout = "#";
   STATUSin = "$ACDEFHOPQRSW ";
   STATUSout = "#";
   ALPHin = "SPHAT ";
   ALPHout = "#";

   whole_line = 0;

   nlet_max = 99;
   nlet_min = 0;
   nphon_max = 99;
   nphon_min = 0;
   nsyl_max = 9;
   nsyl_min = 0;
   kffreq_max = 99999L;
   min_kffreq = 0L;
   max_kfncats = 99;
   min_kfncats = 0;
   t_l_max = 999999L;
   t_l_min = 0L;
   max_kfnsamp = 999;
   min_kfnsamp = 0;
   brownf_max = 9999;
   brownf_min = 0;
   fam_max = 999;
   fam_min = 0;
   conc_max = 999;
   conc_min = 0;
   imag_max = 999;
   imag_min = 0;
   meanc_max = 999;
   meanc_min = 0;
   meanp_max = 999;
   meanp_min = 0;
   aoa_max = 999;
   aoa_min = 0;
   parse = 'W';
   alph_max = 90;
   alph_min = 32;
   status_max = 90;
   status_min = 32;

   if (argc < 2) {
      usage();
      exit(1);
   }
   while (argc > 1) {

      s = (*++argv);

      if ((i = strcmp("-File", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -File\n");
            exit(1);
         } else {
            infile = (*argv);
            newinfile = 1;
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-entry", s)) == 0) {
         whole_line = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-A", s)) == 0) {
         outa = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-B", s)) == 0) {
         outb = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-C", s)) == 0) {
         outc = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-D", s)) == 0) {
         outd = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-E", s)) == 0) {
         oute = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-F", s)) == 0) {
         outf = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-G", s)) == 0) {
         outg = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-H", s)) == 0) {
         outh = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-I", s)) == 0) {
         outi = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-J", s)) == 0) {
         outj = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-K", s)) == 0) {
         outk = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-L", s)) == 0) {
         outl = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-M", s)) == 0) {
         outm = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-N", s)) == 0) {
         outn = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-O", s)) == 0) {
         outo = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-P", s)) == 0) {
         outp = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-Q", s)) == 0) {
         outq = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-R", s)) == 0) {
         outr = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-S", s)) == 0) {
         outs = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-T", s)) == 0) {
         outt = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-U", s)) == 0) {
         outu = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-V", s)) == 0) {
         outv = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-W", s)) == 0) {
         outw = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-X", s)) == 0) {
         outx = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-Y", s)) == 0) {
         outy = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-Z", s)) == 0) {
         outz = 1;
         --argc;
         continue;
      }
      if ((i = strcmp("-nletmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -nletmax\n");
            exit(1);
         } else {
            nlet_max = atoi(*argv);
            if (nlet_max <= 0)
               bad(*argv, "nletmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-nletmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -nletmin\n");
            exit(1);
         } else {
            nlet_min = atoi(*argv);
            if (nlet_min < 0)
               bad(*argv, "nletmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-nphonmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -nphonmax\n");
            exit(1);
         } else {
            nphon_max = atoi(*argv);
            if (nphon_max <= 0)
               bad(*argv, "nphonmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-nphonmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -nphonmin\n");
            exit(1);
         } else {
            nphon_min = atoi(*argv);
            if (nphon_min <= 0)
               bad(*argv, "nphonmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-nsylmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -nsylmax\n");
            exit(1);
         } else {
            nsyl_max = atoi(*argv);
            if (nsyl_max <= 0)
               bad(*argv, "nsylmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-nsylmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -nsylmin\n");
            exit(1);
         } else {
            nsyl_min = atoi(*argv);
            if (nsyl_min <= 0)
               bad(*argv, "nsylmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-kffreqmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -kffreqmax\n");
            exit(1);
         } else {
            kffreq_max = (long) atoi(*argv);
            /* should be atol if there were one... */
            if (kffreq_max <= 0L)
               bad(*argv, "kffreqmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-kffreqmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -kffreqmin\n");
            exit(1);
         } else {
            min_kffreq = (long) atoi(*argv);
            if (min_kffreq <= 0L)
               bad(*argv, "minkffreq");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-kfncatsmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -kfncats\n");
            exit(1);
         } else {
            max_kfncats = atoi(*argv);
            if (max_kfncats <= 0)
               bad(*argv, "maxkfncats");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-kfncatsmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -kfncatsmin\n");
            exit(1);
         } else {
            min_kfncats = atoi(*argv);
            if (min_kfncats <= 0)
               bad(*argv, "minkfncats");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-t-lmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -t-lmax\n");
            exit(1);
         } else {
            t_l_max = (long) atoi(*argv);
            if (t_l_max <= 0L)
               bad(*argv, "t-lmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-t-lmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -t-lmin\n");
            exit(1);
         } else {
            t_l_min = (long) atoi(*argv);
            if (t_l_min <= 0L)
               bad(*argv, "t-lmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-kfnsampmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -kfnsampmax\n");
            exit(1);
         } else {
            max_kfnsamp = atoi(*argv);
            if (max_kfnsamp <= 0)
               bad(*argv, "kfnsampmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-kfnsampmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -kfnsampmin\n");
            exit(1);
         } else {
            min_kfnsamp = atoi(*argv);
            if (min_kfnsamp <= 0)
               bad(*argv, "kfnsampmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-brownfmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -brownfmax\n");
            exit(1);
         } else {
            brownf_max = atoi(*argv);
            if (brownf_max <= 0)
               bad(*argv, "brownfmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-brownfmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -brownfmin\n");
            exit(1);
         } else {
            brownf_min = atoi(*argv);
            if (brownf_min <= 0)
               bad(*argv, "brownfmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-fammax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -fammax\n");
            exit(1);
         } else {
            fam_max = atoi(*argv);
            if (fam_max <= 0)
               bad(*argv, "fammax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-fammin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -fammin\n");
            exit(1);
         } else {
            fam_min = atoi(*argv);
            if (fam_min <= 0)
               bad(*argv, "fammin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-concmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -concmax\n");
            exit(1);
         } else {
            conc_max = atoi(*argv);
            if (conc_max <= 0)
               bad(*argv, "concmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-concmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -concmin\n");
            exit(1);
         } else {
            conc_min = atoi(*argv);
            if (conc_min <= 0)
               bad(*argv, "concmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-imagmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -imagmax\n");
            exit(1);
         } else {
            imag_max = atoi(*argv);
            if (imag_max <= 0)
               bad(*argv, "imagmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-imagmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -imagmin\n");
            exit(1);
         } else {
            imag_min = atoi(*argv);
            if (imag_min <= 0)
               bad(*argv, "imagmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-meancmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -meancmax\n");
            exit(1);
         } else {
            meanc_max = atoi(*argv);
            if (meanc_max <= 0)
               bad(*argv, "meancmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-meancmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -meancmin\n");
            exit(1);
         } else {
            meanc_min = atoi(*argv);
            if (meanc_min <= 0)
               bad(*argv, "meancmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-meanpmax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -meanpmax\n");
            exit(1);
         } else {
            meanp_max = atoi(*argv);
            if (meanp_max <= 0)
               bad(*argv, "meanpmax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-meanpmin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -meanpmin\n");
            exit(1);
         } else {
            meanp_min = atoi(*argv);
            if (meanp_min <= 0)
               bad(*argv, "meanpmin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-aoamax", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -aoamax\n");
            exit(1);
         } else {
            aoa_max = atoi(*argv);
            if (aoa_max <= 0)
               bad(*argv, "aoamax");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-aoamin", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -aoamin\n");
            exit(1);
         } else {
            aoa_min = atoi(*argv);
            if (aoa_min <= 0)
               bad(*argv, "aoamin");
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+PS", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +PS\n");
            exit(1);
         } else {
            i = 0;
            temp = (*argv);
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'N' && 
                   (temp[i]) != 'J' &&
                   (temp[i]) != 'V' &&
                   (temp[i]) != 'A' &&
                   (temp[i]) != 'R' &&
                   (temp[i]) != 'C' &&
                   (temp[i]) != 'U' &&
                   (temp[i]) != 'I' &&
                   (temp[i]) != 'P' &&
                   (temp[i]) != 'O')
                  fprintf(stderr, "Wrong argument to +PS: %c\n", (temp[i]));
               i++;
            }
            strcpy(PSin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-PS", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -PS\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'N' &&
                   (temp[i]) != 'J' &&
                   (temp[i]) != 'V' &&
                   (temp[i]) != 'A' &&
                   (temp[i]) != 'R' &&
                   (temp[i]) != 'C' &&
                   (temp[i]) != 'U' &&
                   (temp[i]) != 'I' &&
                   (temp[i]) != 'P' &&
                   (temp[i]) != 'O')
                  fprintf(stderr, "Wrong argument to -PS: %c \n", (temp[i]));
               i++;
            }
            strcpy(PSout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+PD", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +PD\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'N' &&
                   (temp[i]) != 'J' &&
                   (temp[i]) != 'V' &&
                   (temp[i]) != 'O')
                  fprintf(stderr, "Wrong argument to +PD: %c \n", (temp[i]));
               i++;
            }
            strcpy(PDin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-PD", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -PD\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'N' &&
                   (temp[i]) != 'J' &&
                   (temp[i]) != 'V' &&
                   (temp[i]) != 'O')
                  fprintf(stderr, "Wrong argument to -PD: %c \n", (temp[i]));
               i++;
            }
            strcpy(PDout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+ALPH", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +ALPH\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'S' &&
                   (temp[i]) != 'H' &&
                   (temp[i]) != 'A' &&
                   (temp[i]) != 'T' &&
                   (temp[i]) != 'P')
                  fprintf(stderr, "Wrong argument to +ALPH: %c \n", (temp[i]));
               i++;
            }
            strcpy(ALPHin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-ALPH", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -ALPH\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'S' && (temp[i]) != 'H' && (temp[i]) != 'A'
                   && (temp[i]) != 'T' && (temp[i]) != 'P')
                  fprintf(stderr, "Wrong argument to -ALPH: %c \n", (temp[i]));
               i++;
            }
            strcpy(ALPHout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+IRREG", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +IRREG\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'Z' && (temp[i]) != 'Y' && (temp[i]) != 'B'
                   && (temp[i]) != 'P' && (temp[i]) != 'N')
                  fprintf(stderr, "Wrong argument to +IRREG: %c \n",(temp[i]));
               i++;
            }
            strcpy(IRREGin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-IRREG", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -IRREG\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'Y' && (temp[i]) != 'B' && (temp[i]) != 'Z'
                   && (temp[i]) != 'P' && (temp[i]) != 'N')
                  fprintf(stderr, "Wrong argument to -IRREG: %c \n",(temp[i]));
               i++;
            }
            strcpy(IRREGout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+TYPE", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +TYPE\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'Q' && (temp[i]) != '2')
                  fprintf(stderr, "Wrong argument to +TYPE: %c \n", (temp[i]));
               i++;
            }
            strcpy(TYPEin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-TYPE", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -TYPE\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'Q' && (temp[i]) != '2')
                  fprintf(stderr, "Wrong argument to -TYPE: %c \n", (temp[i]));
               i++;
            }
            strcpy(TYPEout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+VAR", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +VAR\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'B' && (temp[i]) != 'O')
                  fprintf(stderr, "Wrong argument to +VAR: %c \n", (temp[i]));
               i++;
            }
            strcpy(VARin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-VAR", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -VAR\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'B' && (temp[i]) != 'O')
                  fprintf(stderr, "Wrong argument to -VAR: %c \n", (temp[i]));
               i++;
            }
            strcpy(VARout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+STATUS", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +STATUS\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != '$' && (temp[i]) != 'A' && (temp[i]) != 'C'
                 && (temp[i]) != 'D' && (temp[i]) != 'E' && (temp[i]) != 'F'
                 && (temp[i]) != 'H' && (temp[i]) != 'O' && (temp[i]) != 'P'
                 && (temp[i]) != 'Q' && (temp[i]) != 'R' && (temp[i]) != 'S'
                   && (temp[i]) != 'W')
                  fprintf(stderr, "Wrong argument to +STATUS: %c \n",
                    (temp[i]));
               i++;
            }
            strcpy(STATUSin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-STATUS", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -STATUS\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != '$' && (temp[i]) != 'A' && (temp[i]) != 'C'
                 && (temp[i]) != 'D' && (temp[i]) != 'E' && (temp[i]) != 'F'
                 && (temp[i]) != 'H' && (temp[i]) != 'O' && (temp[i]) != 'P'
                 && (temp[i]) != 'Q' && (temp[i]) != 'R' && (temp[i]) != 'S'
                   && (temp[i]) != 'W')
                  fprintf(stderr, "Wrong argument to -STATUS: %c \n",
                    (temp[i]));
               i++;
            }
            strcpy(STATUSout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("+CAP", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to +CAP\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'C')
                  fprintf(stderr, "Wrong argument to +CAP: %c \n", (temp[i]));
               i++;
            }
            strcpy(CAPin, temp);
            --argc;
         }
         --argc;
         continue;
      }
      if ((i = strcmp("-CAP", s)) == 0) {
         if ((argc - 1) == 1 || (*++argv)[0] == '-') {
            fprintf(stderr, "no argument given to -CAP\n");
            exit(1);
         } else {
            i = 0;
            temp = *argv;
            while ((temp[i]) != '\0') {
               if ((temp[i]) != 'C')
                  fprintf(stderr, "Wrong argument to -CAP: %c \n", (temp[i]));
               i++;
            }
            strcpy(CAPout, temp);
            --argc;
         }
         --argc;
         continue;
      }
      fprintf(stderr, "Illegal Switch %s\n", s);
      exit(1);
   }                            /* end while */

#ifdef DEBUG
   printf(" nlet-max: %d\n ", nlet_max);
   printf(" nlet-min: %d\n ", nlet_min);
   printf(" %d\n ", nphon_max);
   printf(" %d\n ", nphon_min);
   printf(" %d\n ", nsyl_max);
   printf(" %d\n ", nsyl_min);
   printf(" kffmax: %ld\n ", kffreq_max);
   printf(" kffmin: %ld\n ", min_kffreq);
   printf(" %d\n ", max_kfncats);
   printf(" %d\n ", min_kfncats);
   printf(" %ld\n ", t_l_max);
   printf(" %ld\n ", t_l_min);
   printf(" %d\n ", max_kfnsamp);
   printf(" %d\n ", min_kfnsamp);
   printf(" %d\n ", fam_max);
   printf(" %d\n ", fam_min);
   printf(" %d\n ", conc_max);
   printf(" %d\n ", conc_min);
   printf(" %d\n ", imag_max);
   printf(" %d\n ", imag_min);
   printf(" %d\n ", meanc_max);
   printf(" %d\n ", meanc_min);
   printf(" %d\n ", meanp_max);
   printf(" %d\n ", meanp_min);
   printf(" %d\n ", aoa_max);
   printf(" %d\n ", aoa_min);


#endif

/* open database file */
   if (newinfile != 1)
      infile = DATAFILE;

   if (
        (
          zfp
          =
          zfopen(infile, "r")
        ) 
        ==
        0 /* NULL */
      ) {
      fprintf(stderr, " can't open  %s  as data file \n", infile);
      exit(1);
   };
   /* read database lines until EOF */
   while (
             zfgets(line, 124, zfp)
             !=
             0 /* NULL */
         ) {
      /* parse line */
      nlet = itoaa(0, 1, line); /* number of letters */
      nphon = itoaa(2, 3, line);/* number of phomemes */
      nsyl = line[4] - '0';     /* number of syllables in word */
      kffreq = (long) itoaa(5, 9, line);        /* k & f frequency */
      kfncats = itoaa(10, 11, line);    /* no of k&f categories */
      kfnsamp = itoaa(12, 14, line);    /* no of k&f samples */
      t_l = (long) itoaa(15, 20, line); /* Thorndyke-Lorge freq */
      brownf = itoaa(21, 24, line);     /* Brown's Spoken frequency */
      fam = itoaa(25, 27, line);/* Familiarity */
      conc = itoaa(28, 30, line);       /* Concreteness */
      imag = itoaa(31, 33, line);       /* Imagery */
      meanc = itoaa(34, 36, line);      /* mean Colerado meaningfulness */
      meanp = itoaa(37, 39, line);      /* mean pavio meaningfulness */
      aoa = itoaa(40, 42, line);/* mean pavio meaningfulness */
      tq2 = line[43];           /* type */
      wtype = line[44];         /* part of speech */
      pdwtype = line[45];       /* PD part of speech */
      alphsyl = line[46];       /* Alphasyllable */
      status = line[47];        /* Status */
      varp = line[48];          /* Varient phoneme */
      cap = line[49];
      irreg = line[50];         /* irregular plural */
      w = 51;
      wt = 0;
      while (line[w] != '|') {
         word[wt] = line[w];    /* words itself */
         w++;
         wt++;
      }
      word[wt] = '\0';
      ++w;
      wt = 0;
      while (line[w] != '|') {
         phon[wt] = line[w];    /* phonetic transcription */
         w++;
         wt++;
      }
      phon[wt] = '\0';
      wt = 0;
      ++w;
      while (line[w] != '|') {
         dphon[wt] = line[w];   /* edited phon transcription */
         w++;
         wt++;
      }
      dphon[wt] = '\0';
      wt = 0;
      ++w;
      while (line[w] != '|' && line[w] != '\0') {
         stress[wt] = line[w];  /* stress pattern */
         w++;
         wt++;
      }
      stress[wt] = '\0';

/* test this line to see if it fits within the required limits */

#ifdef DEBUG
      printf(" before test nlet: %d\n ", nlet);
      printf("nphon %d\n ", nphon);
      printf("nsyl %d\n ", nsyl);
      printf("kffreq %ld\n ", kffreq);
      printf("kfncats %d\n ", kfncats);
      printf("t_l %ld\n ", t_l);
      printf("brownf %d\n ", brownf);
      printf("kfnsamp %d\n ", kfnsamp);
      printf("fam %d\n ", fam);
      printf("conc %d\n ", conc);
      printf("imag %d\n ", imag);
      printf("meanc %d\n ", meanc);
      printf("meanp %d\n ", meanp);
      printf("aoa %d\n ", aoa);
      printf("cap %c\n ", cap);
#endif

      if (inrange(nlet_max, nlet, nlet_min) &&
          inrange(nphon_max, nphon, nphon_min) &&
          inrange(nsyl_max, nsyl, nsyl_min) &&
          linrange(kffreq_max, kffreq, min_kffreq) &&
          inrange(max_kfncats, kfncats, min_kfncats) &&
          inrange(max_kfnsamp, kfnsamp, min_kfnsamp) &&
          linrange(t_l_max, t_l, t_l_min) &&
          inrange(brownf_max, brownf, brownf_min) &&
          inrange(fam_max, fam, fam_min) &&
          inrange(conc_max, conc, conc_min) &&
          inrange(imag_max, imag, imag_min) &&
          inrange(meanc_max, meanc, meanc_min) &&
          inrange(meanp_max, meanp, meanp_min) &&
          inrange(aoa_max, aoa, aoa_min)) {
         if (
             index(PSin, wtype) &&
             outstring(PSout, wtype) &&
             index(PDin, pdwtype) &&
             outstring(PDout, pdwtype) &&
             index(TYPEin, tq2) &&
             outstring(TYPEout, tq2) &&
             index(VARin, varp) &&
             outstring(VARout, varp) &&
             index(STATUSin, status) &&
             outstring(STATUSout, status) &&
             index(IRREGin, irreg) &&
             outstring(IRREGout, irreg) &&
             index(CAPin, cap) &&
             outstring(CAPout, cap) &&
             index(ALPHin, alphsyl) &&
             outstring(ALPHout, alphsyl)) {

/* items that pass here fit the criteria and should be output in some form*/

            if (outa == 1)
               printf(" %d ", nlet);
            if (outb == 1)
               printf(" %d ", nphon);
            if (outc == 1)
               printf(" %d ", nsyl);
            if (outd == 1)
               printf(" %ld ", kffreq);
            if (oute == 1)
               printf(" %d ", kfncats);
            if (outf == 1)
               printf(" %d ", kfnsamp);
            if (outg == 1)
               printf(" %ld ", t_l);
            if (outh == 1)
               printf(" %d ", brownf);
            if (outi == 1)
               printf(" %d ", fam);
            if (outj == 1)
               printf(" %d ", conc);
            if (outk == 1)
               printf(" %d ", imag);
            if (outl == 1)
               printf(" %d ", meanc);
            if (outm == 1)
               printf(" %d ", meanp);
            if (outn == 1)
               printf(" %d ", aoa);
            if (outo == 1)
               printf(" %c ", tq2);
            if (outp == 1)
               printf(" %c ", wtype);
            if (outq == 1)
               printf(" %c ", pdwtype);
            if (outr == 1)
               printf(" %c ", alphsyl);
            if (outs == 1)
               printf(" %c ", status);
            if (outt == 1)
               printf(" %c ", varp);
            if (outu == 1)
               printf(" %c ", cap);
            if (outv == 1)
               printf(" %c ", irreg);
            if (outw == 1)
               printf(" %s ", word);
            if (outx == 1)
               printf(" %s ", phon);
            if (outy == 1)
               printf(" %s ", dphon);
            if (outz == 1)
               printf(" %s ", stress);
            if (whole_line != 1)
               printf("\n");
            else
               printf("%s", line);
         }
      }
   }
   return (0);
}

#ifndef __STDC__
int itoaa(s, e, line)   /* returns value for variable */
   int s, e;
   char *line;
#else
int itoaa(int s, int e, char *line)
#endif
{
int i, n;

   n = 0;
   for (i = s; i <= e; i++)
      n = 10 * n + line[i] - '0';
   return (n);
}

#ifndef __STDC__
bad(v, s)
   char *v;
   char *s;
#else
void bad(char *v, char *s)
#endif
{
   printf("\"%s\": bad %s specification\n", v, s);
   exit(0);
}


#ifndef __STDC__
int inrange(imax, val, imin)
   int imax, val, imin;
#else
int inrange(int imax, int val, int imin)
#endif
{
   if (imax >= val && imin <= val)
      return (1);
   else
      return (0);
}

#ifndef __STDC__
int linrange(lmax, val, lmin)
   long lmax, val, lmin;
#else
int linrange(long lmax, long val, long lmin)
#endif
{
   if (lmax >= val && lmin <= val)
      return (1);
   else
      return (0);
}

#ifndef __STDC__
int outstring(string, c)
   char c, string[];
#else
int outstring(char string[], char c)
#endif
{

   if (index(string, c) != 0)
      return (0);
   else
      return (1);
}

#ifndef __STDC__
usage()
#else
void usage(void)
#endif
{
   fprintf(stderr, "Usage: dict -a N [ ... -z N] -A [... V][X]\n");
   fprintf(stderr, "\n");
   fprintf(stderr, "MRC Psycholinguistic Database - mrc2\n");
   fprintf(stderr, "see QJEP 1981, 33A, 497-505 \n");
   fprintf(stderr, "\n");
   fprintf(stderr, "-File filename - non-default input file\n\n");
   fprintf(stderr, "Output selection:\n");
   fprintf(stderr, "-A NLET number of letters\n");
   fprintf(stderr, "-B NPHON number of phonemes\n");
   fprintf(stderr, "-C NSYL number of syllables\n");
   fprintf(stderr, "-D K-F-FREQ Kucera-Francis written frequency\n");
   fprintf(stderr, "-E K-F-NCATS Kucera-Francis number of categories\n");
   fprintf(stderr, "-F K-F-NSAMP Kucera-Francis number of samples\n");
   fprintf(stderr, "-G T-L-FREQ Thorndyke-Lorge written Frequency\n");
   fprintf(stderr, "-H BROWNF Brown verbal frequency\n");
   fprintf(stderr, "-I FAM Familiarity\n");
   fprintf(stderr, "-J CONC Concreteness\n");
   fprintf(stderr, "-K IMAG Imagery\n");
   fprintf(stderr, "-L MEANC Mean Colerado (Meaningfulness)\n");
   fprintf(stderr, "-M MEANP Mean Pavio (Meaningfulness)\n");
   fprintf(stderr, "-N AOA Age of Aquisition\n");
   fprintf(stderr, "-O TQ2 Type\n");
   fprintf(stderr, "-P WTYPE   Part of Speech\n");
   fprintf(stderr, "-Q PDWTYPE Common part of speech\n");
   fprintf(stderr, "-R ALPHSYL Alphasyllable\n");
   fprintf(stderr, "-S STATUS\n");
   fprintf(stderr, "-T VAR Varient phoneme\n");
   fprintf(stderr, "-U CAP Capitalisation of words\n");
   fprintf(stderr, "-V IRREG Irregular plural\n");
   fprintf(stderr, "-W WORD\n");
   fprintf(stderr, "-X PHON Phonetic transcription\n");
   fprintf(stderr, "-Y DPHON Edited phonetic transcription\n");
   fprintf(stderr, "-Z STRESS\n");
   fprintf(stderr, "-entry The whole entry\n");
   fprintf(stderr, "\n");

   fprintf(stderr, "Restriction by category\n\n");
   fprintf(stderr, "You specify the max and min on each scale\n \n");

   fprintf(stderr, "-nletmax N  max length \n");
   fprintf(stderr, "-nletmin N  min length\n");
   fprintf(stderr, "-nphonmax N  max num phonemes\n");
   fprintf(stderr, "-nphonmin N  min num phonemes\n");
   fprintf(stderr, "-nsylmax N  max num syllables\n");
   fprintf(stderr, "-nsylmin N  min num syllables\n");
   fprintf(stderr, "-kffreqmax N  max kf frequency\n");
   fprintf(stderr, "-kffreqmin N  min kf frequency  > 0\n");
   fprintf(stderr, "-kfncatsmax N  max kf number categories\n");
   fprintf(stderr, "-kfncatsmin N  min kf number categories\n");
   fprintf(stderr, "-kfnsampmax N  max kf number samples\n");
   fprintf(stderr, "-kfnsampmin N  min kf number samples\n");
   fprintf(stderr, "-t-lmax N  max thorndyke-lorge\n");
   fprintf(stderr, "-t-lmin N  min thorndyke-lorge\n");
   fprintf(stderr, "-brownfmax N  max Brown verbal frequency\n");
   fprintf(stderr, "-brownfmin N  min Brown verbal frequency\n");
   fprintf(stderr, "-fammax N  max familiarity  < 700\n");
   fprintf(stderr, "-fammin N  min familiarity  > 100\n");
   fprintf(stderr, "-concmax N  max concreteness < 700\n");
   fprintf(stderr, "-concmin N  min concreteness > 100\n");
   fprintf(stderr, "-imagmax N  max imagability  < 700\n");
   fprintf(stderr, "-imagmin N  min imagability  > 100\n");
   fprintf(stderr, "-meancmax N  max mean Colorado meaningfulness < 700\n");
   fprintf(stderr, "-meancmin N  min mean Colorado meaningfulness > 700\n");
   fprintf(stderr, "-meanpmax N  max mean pavio meaningfulness < 700\n");
   fprintf(stderr, "-meanpmin N  min pavio meaningfulness > 100\n");
   fprintf(stderr, "-aoamax N  max age of aquisition  < 700\n");

   fprintf(stderr, "-aoamin N  min age of aquisition  > 100\n");
   fprintf(stderr,"\n Select entries which include (+) or exclude (-) values");
   fprintf(stderr, " (arguments) for\n linguistic properties (flags)\n \n");
   fprintf(stderr, "-+TYPE Q2\n");
   fprintf(stderr, "-+IRREG ZBYPN\n");
   fprintf(stderr, "-+VAR BO\n");
   fprintf(stderr, "-+ALPH SPHAT\n");
   fprintf(stderr, "-+STATUS $ACDEFHOPQRSW\n");
   fprintf(stderr, "-+PS NJVARCUIPO\n");
   fprintf(stderr, "-+PD NJVO\n");
   fprintf(stderr, "-+CAP C\n");
   exit(0);
}
SHAR_EOF
cat - << \SHAR_EOF > zcat.c
#include <stdio.h>
#include "zlib.h"
/*#include "zlib.c"*/   /* Written so it can be either included or linked in */

/* This part is optional... you probably wouldn't do this in real life */
#define FILE ZFILE
#define fgetc(in) zfgetc(in)
#define fopen(f, m) zfopen(f, m)
#define fclose(f) zfclose(f)

#ifndef __STDC__
int main(argc, argv)
int argc;
char **argv;
#else
int main(int argc, char **argv)
#endif
{
  FILE *in;
  int i, c;

  if (argc == 1) {
    in = zfilter(stdin);
    for (c = fgetc(in); c != EOF; putchar(c), c = fgetc(in)) ;
    fclose(in);
  } else if (argc > 1) {
    for (i = 1; i < argc; i++) {
      in = fopen(argv[i], "r");
      if (in != NULL) {
        for (c = fgetc(in); c != EOF; putchar(c), c = fgetc(in)) ;
        fclose(in);
      } else {
       fprintf(stderr, "%s: cannot open %s\n", argv[0], argv[i]);
      }
    }
  }
  return(0);
}
SHAR_EOF
cat - << \SHAR_EOF > zlib.c
/*#define MAIN*/
/*int debug = 1;*/
/*#define DEBUG 1*/

/* These wondrous debugging macros helped me find the nasty bug which
   only manifested itself on msdos -- stackp has to be a long on msdos
   because the array it is indexing is 'huge' ... */
#ifdef DEBUG
#define TRACT(lev, stmnt) \
  if (lev <= debug) fprintf(stderr, "%d: %s\n", __LINE__, #stmnt);
#define TRACE(lev, stmnt) \
  if (lev <= debug) fprintf(stderr, "%d: %s\n", __LINE__, #stmnt); stmnt
#define TRACA(lev, stmnt) \
  stmnt; if (lev <= debug) fprintf(stderr, "%d: %s\n", __LINE__, #stmnt);
#define TRACL(lev, var) \
  if (lev <= debug) fprintf(stderr, "%d: %s <- %ld\n", __LINE__, #var, var);
#else
#define TRACT(lev, stmnt)
#define TRACE(lev, stmnt) stmnt
#define TRACA(lev, stmnt) stmnt
#define TRACL(lev, var)
#endif
/* 
 *
 * Originally:
 *
 * compress.c - File compression ala IEEE Computer, June 1984.
 *
 * Authors:    Spencer W. Thomas       (decvax!harpo!utah-cs!utah-gr!thomas)
 *             Jim McKie               (decvax!mcvax!jim)
 *             Steve Davies            (decvax!vax135!petsd!peora!srd)
 *             Ken Turkowski           (decvax!decwrl!turtlevax!ken)
 *             James A. Woods          (decvax!ihnp4!ames!jaw)
 *             Joe Orost               (decvax!vax135!petsd!joe)
 *
 * $Header: zlib.c,v 4.1 90/11/12 14:52:24 gtoal Release $
 *
 * Graham Toal, 3rd September 1988.  My changes released to public domain.
 *                                   Updated Nov 90.
 *
 * The original decompress has been restructured so that data can be
 * fetched on demand a byte at a time.  This lets it be used as a filter
 * for programs which read large data files - you do not need the disk
 * space to decompress the input files first.
 *
 * (Incidentally, programs reading data off floppies will be speeded up
 *  because decompression is always faster than the equivalent amount
 *  of disk I/O).
 *
 * This implementation supplies 'z' versions of fopen, fputc, feof and fclose
 * to be used as direct substitutes for the originals; it would be cleaner
 * and more transparent if the decompress filter were hidden under the
 * real stdio procedures.  An extra call zfilter() is supplied to convert
 * an already-opened stream into a z-stream: see the example at the end
 * of this file.
 *
 * If a file opened by zfopen() was not compressed, the files contents are
 * still recovered correctly at the low expense of an extra procedure call
 * per byte.  This makes the routines more generally usable - they can be
 * left in production programs which can be speeded up in the field by
 * compressing selected input files(*); also, files can be compressed or
 * not selectively depending on whether the compression makes them
 * smaller or not - code accessing the files does not need to know.
 *
 * [(*) reading from a compressed file off floppy disk is faster than
 * reading from an uncompressed file. This probably isn't true of
 * hard disks though.]
 *
 * BUGS: Opening a file "r" will not do CR/LF processing on computers with
 *       this file structure.
 */

#include <stdio.h>
#include <string.h>
#ifdef __STDC__
#include <stdlib.h>
#else
#define size_t int
#endif
#include <ctype.h>

#ifdef MSDOS
#include <malloc.h>
#endif

#ifndef min
#define min(a,b)        ((a>b) ? b : a)
#endif
#define HSIZE           69001L  /* 95% occupancy */

/*
 * the next two codes should not be changed lightly, as they must not
 * lie within the contiguous general code space.
 */

#define FIRST  257L     /* first free entry */
#define CLEAR  256L     /* table clear output code */

#define BIT_MASK        0x1f
#define BLOCK_MASK      0x80
#define INIT_BITS       9       /* initial number of bits/code */

#define CHECK_GAP 10000L/* ratio check interval */

#include "zlib.h"
#define NOT_COMPRESSED 1
#define ALLOCATED 2

#ifndef __STDC__
static void decompress_more( /* register ZFILE *z */ );
static long getcode( /* register ZFILE *z */ );
#else
static void decompress_more(register ZFILE *z);
static long getcode(register ZFILE *z);
#endif

#ifndef __STDC__
ZFILE *zfopen(fileptr, how)
   char *fileptr;
   char *how;
#else
ZFILE *zfopen(char *fileptr, char *how)
#endif
{
register ZFILE *z;

   z = (ZFILE *) malloc(sizeof(ZFILE));
   z->flags = 0;
   z->maxbits = Z_BITS;         /* user settable max # bits/code */
   z->free_ent = 0;             /* first unused entry */
   z->block_compress = BLOCK_MASK;
   z->clear_flg = 0;
   z->init = 0;

   z->zeof = (0 != 0);
   z->c1 = EOF;
   z->c2 = EOF;
   z->bufput = 0;
   z->bufget = 0;
   z->bufend = Z_MAXBUF - 1;

   z->maxbits = Z_BITS;         /* user settable max # bits/code */

   /* Open input file */
   if (*how == 'r') {
      z->file = fopen(fileptr, "rb");
      if (z->file == NULL) {
char tempfname[256];

         strcpy(tempfname, fileptr);
         strcat(tempfname, ZEXT);
         z->file = fopen(tempfname, "rb");
      }
   } else {
      /* No compressed output yet, if ever...                  */
      /* Compress the file explicitly once it has been written */
      z->file = fopen(fileptr, how);
      z->flags |= NOT_COMPRESSED;
   }
   if (z->file == NULL) {
      free(z);
      z = NULL;
   }
   /* Check the magic number */
   if ((z != NULL) 
       && ((fgetc(z->file) != 0x1F) || (fgetc(z->file) != 0x9D))) {
      z->flags |= NOT_COMPRESSED;
      fclose(z->file);
      z->file = fopen(fileptr, how);
      if (z->file == NULL) {
         free(z);
         z = NULL;
      }
   }
   if ((z == NULL) || ((z->flags & NOT_COMPRESSED) != 0))
      return (z);
   z->maxbits = fgetc(z->file); /* set -b from file */
   z->block_compress = z->maxbits & BLOCK_MASK;
   z->maxbits &= BIT_MASK;
   if (z->maxbits > Z_BITS) {
      fprintf(stderr,
        "%s: compressed with %d bits; decompress can only handle %d bits\n",
              fileptr, z->maxbits, Z_BITS);
      exit(0);
   }
   return (z);
}

#ifndef __STDC__
ZFILE *zfilter(f)
   FILE *f;
#else
ZFILE *zfilter(FILE *f)
#endif
{
register ZFILE *z;

   z = (ZFILE *) malloc(sizeof(ZFILE));
   z->flags = 0;
   z->maxbits = Z_BITS;         /* user settable max # bits/code */
   z->free_ent = 0;             /* first unused entry */
   z->block_compress = BLOCK_MASK;
   z->clear_flg = 0;
   z->init = 0;

   z->zeof = (0 != 0);
   z->c1 = EOF;
   z->c2 = EOF;
   z->bufput = 0;
   z->bufget = 0;
   z->bufend = Z_MAXBUF - 1;

   z->maxbits = Z_BITS;         /* user settable max # bits/code */

   /* Open input file */
   z->file = f;
   if (z->file == NULL) {
      free(z);
      z = NULL;
   }
   /* Check the magic number */
   if (z != NULL) {
      z->c1 = fgetc(z->file);
      z->c2 = fgetc(z->file);
      if ((z->c1 != 0x1F) || (z->c2 != 0x9D)) {
         z->flags |= NOT_COMPRESSED;
      }
   }
   if ((z == NULL) || ((z->flags & NOT_COMPRESSED) != 0))
      return (z);
   z->maxbits = fgetc(z->file); /* set -b from file */
   z->block_compress = z->maxbits & BLOCK_MASK;
   z->maxbits &= BIT_MASK;
   if (z->maxbits > Z_BITS) {
      fprintf(stderr,
      "stdin compressed with %d bits; decompress can only handle %d bits\n",
              z->maxbits, Z_BITS);
      exit(0);
   }
   return (z);
}

#ifndef __STDC__
int zfgetc(z)
   ZFILE *z;
#else
int zfgetc(ZFILE *z)
#endif
{
int c;

   /*
      If buffer empty, and not end-of-file, call decompress_more(); return
      next in buffer.  
   */
   if ((z->flags & NOT_COMPRESSED) != 0) {
      if ((c = z->c1) >= 0) {
         z->c1 = z->c2;
         z->c2 = EOF;
         return (c);
      }
      return (fgetc(z->file));
   }
   if ((z->bufget == z->bufput) && (!z->zeof)) {
      decompress_more(z);
   }
   z->zeof = (z->bufput == z->bufget);
   if (z->zeof) {
      if ((z->flags & ALLOCATED) != 0) {
#ifdef MSDOS
         hfree(z->tab_suffixof);
         hfree(z->tab_prefixof);
#else
         free(z->tab_suffixof);
         free(z->tab_prefixof);
#endif
         z->flags &= (~ALLOCATED);
      }
      return (EOF);
   }
   c = z->buff[z->bufget];
   z->bufget++;
   return (c);
}

#ifndef __STDC__
int zfeof(z)
   ZFILE *z;
#else
int zfeof(ZFILE *z)
#endif
{
   if ((z->flags & NOT_COMPRESSED) != 0) {
      if (z->c1 != EOF) {
         return (0 != 0);
      }
      return (feof(z->file));
   }
   return (z->zeof);
}

#ifndef __STDC__
void zfclose(z)
   ZFILE *z;
#else
void zfclose(ZFILE *z)
#endif
{
   if (z == 0)
      return;
   if (z->zeof) {
      if ((z->flags & ALLOCATED) != 0) {
#ifdef MSDOS
         hfree(z->tab_suffixof);
         hfree(z->tab_prefixof);
#else
         free(z->tab_suffixof);
         free(z->tab_prefixof);
#endif
         z->flags &= (~ALLOCATED);
      }
   }
   free(z);
}

#ifndef __STDC__
char *zfgets(line, len, zfp)
   char *line;
   int len;
   ZFILE *zfp;
#else
char *zfgets(char *line, int len, ZFILE *zfp)
#endif
{
/* I *hope* this is what fgets does - I only added it
   here when I came across a program that needed it; I'm
   including the '\n' in the string. */
int c, pos = 0;

   for (;;) {
      c = zfgetc(zfp);
      if (c == EOF)
         return (NULL);
      c &= 255;
      line[pos] = (char) c;
      if (pos + 1 == len)       /* Too long! */
         break;
      pos++;
      if (c == '\n')
         break;
   }
   line[pos] = '\0';
   return (line);
}

#ifndef __STDC__
static void decompress_more(z)
   register ZFILE *z;
#else
static void decompress_more(register ZFILE *z)
#endif
{
   z->bufput = 0;
   z->bufget = 0;

   if (z->init != 0)
      goto resume;
   z->init = 1;

   z->offset = 0;
   z->size = 0;
#ifdef MSDOS
   z->tab_suffixof =
     (unsigned char PC_HUGE *) halloc(HSIZE, sizeof(unsigned char));
   z->tab_prefixof =
     (long PC_HUGE *) halloc(HSIZE, sizeof(long));
#else
   z->tab_suffixof =
    (unsigned char *) malloc((size_t) HSIZE * sizeof(unsigned char));
   z->tab_prefixof = (long *) malloc((size_t) HSIZE * sizeof(long));
#endif
   z->flags |= ALLOCATED;

   z->n_bits = INIT_BITS;
   z->maxcode = ((1L << (z->n_bits)) - 1L);
   for (z->code = 255L; z->code >= 0L; z->code--) {
      z->tab_prefixof[z->code] = 0L;
      z->tab_suffixof[z->code] = (unsigned char) z->code;
   }
   z->free_ent = ((z->block_compress) ? FIRST : 256L);

   z->finchar = z->oldcode = getcode(z);
   if (z->oldcode == -1L)
      return;                   /* EOF already? */
   if (z->finchar < 0L || z->finchar >= 256L)
      fprintf(stderr, "****\n");
   z->buff[z->bufput] = (char) (z->finchar & 0xff);
   z->bufput++;

   z->stackp = 1L << Z_BITS;    /* The 1L is for DOS huge arrays */

   while ((z->code = getcode(z)) != EOF) {
      if ((z->code == CLEAR) && z->block_compress) {
         for (z->code = 255; z->code >= 0; z->code--)
            z->tab_prefixof[z->code] = 0;
         z->clear_flg = 1;
         z->free_ent = FIRST - 1;
         if ((z->code = getcode(z)) == EOF)
            break;              /* O, untimely death! */
      }                         /* if */
      z->incode = z->code;
      if (z->code >= z->free_ent) {
         z->tab_suffixof[z->stackp] = (unsigned char) z->finchar;
         z->stackp += 1L;
         z->code = z->oldcode;
      }
      while (z->code >= 256L) {
         z->tab_suffixof[z->stackp] = z->tab_suffixof[z->code];
         z->stackp += 1L;
         z->code = z->tab_prefixof[z->code];
      }
      z->finchar = z->tab_suffixof[z->code];
      z->tab_suffixof[z->stackp] = (unsigned char) z->finchar;
      z->stackp += 1L;
      do {
long tmp;

         z->stackp -= 1L;
         tmp = z->tab_suffixof[z->stackp];
         z->buff[z->bufput++] = (unsigned char) (tmp & 255L);
         if (z->bufput == z->bufend) {
            return;             /* Logically a setjmp/longjump, but this is
                                   more portable */
      resume:;                  /* jumped to here -- is jumping into a loop
                                   safe? */
            /* - or should I use jumps for the loop too?      */
         }                      /* if */
      } while (z->stackp > (1L << Z_BITS));
      /* ^ This is why I changed stackp from a pointer. */
      /* Pointer comparisons can be dubious...          */
      if ((z->code = z->free_ent) < (1L << z->maxbits)) {
         z->tab_prefixof[z->code] = z->oldcode;
         z->tab_suffixof[z->code] = (unsigned char) z->finchar;
         z->free_ent = z->code + 1;
      }
      z->oldcode = z->incode;
   }                            /* while */
}                       /* decompress more */

static unsigned char rmask[9] =
{0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f, 0xff};

#ifndef __STDC__
static long getcode(z)
   register ZFILE *z;
#else
static long getcode(register ZFILE *z)
#endif
{                       /* Should be int!!! */
register long code;
register long r_off, bits;
register int bp;

   bp = 0;
   if (z->clear_flg != 0 ||
       z->offset >= z->size ||
       z->free_ent > z->maxcode) {
      if (z->free_ent > z->maxcode) {
         z->n_bits++;
         if (z->n_bits == z->maxbits) {
            z->maxcode = (1L << z->maxbits);    /* won't get any bigger now */
         } else {
            z->maxcode = ((1L << (z->n_bits)) - 1L);
         }
      }
      if (z->clear_flg != 0) {
         z->n_bits = INIT_BITS;
         z->maxcode = ((1L << (z->n_bits)) - 1L);
         z->clear_flg = 0;
      }
      z->size = fread(z->buf, 1, (size_t) z->n_bits, z->file);
      if (z->size <= 0) {
         fclose(z->file);
         return (EOF);          /* end of file */
      }
      z->offset = 0;
      z->size = (z->size << 3) - (z->n_bits - 1);
   }
   r_off = z->offset;
   bits = z->n_bits;
   bp = bp + ((int) r_off >> 3);
   r_off = r_off & 7;
   code = ((long) z->buf[bp++] >> r_off);
   bits = bits - 8 + r_off;
   r_off = 8 - r_off;           /* now, offset into code word */
   if (bits >= 8) {
      code = code | ((long) z->buf[bp++] << r_off);
      r_off = r_off + 8;
      bits = bits - 8;
   }
   code = code 
      | ((long) ((long) (z->buf[bp]) & (long) rmask[bits]) << (long) r_off);
   z->offset = z->offset + z->n_bits;
   return (code);
}

#ifdef MAIN

/* This part is optional... */
#define FILE ZFILE
#define fgetc(in) zfgetc(in)
#define fopen(f, m) zfopen(f, m)
#define fclose(f) zfclose(f)

#ifndef __STDC__
int main(argc, argv)
   int argc;
   char **argv;
#else
int main(int argc, char **argv)
#endif
{
FILE *in;
int i, c;

   if (argc == 1) {
      in = zfilter(stdin);
      for (c = fgetc(in); c != EOF; fputc(c, stderr), c = fgetc(in));
      zfclose(in);
   } else if (argc > 1) {
      for (i = 1; i < argc; i++) {
         in = fopen(argv[i], "r");
         if (in != NULL) {
            for (c = fgetc(in); c != EOF; fputc(c, stderr), c = fgetc(in));
            fclose(in);
         } else {
            fprintf(stderr, "%s: cannot open %s\n", argv[0], argv[i]);
         }
      }
   }
   return (0);
}

#endif
SHAR_EOF
cat - << \SHAR_EOF > zlib.h
#ifndef _ZLIB_H
#define _ZLIB_H 1

#ifdef MSDOS
#define PC_HUGE huge  /* Microsoft C and contemptibles */ 
#else
#define PC_HUGE
#endif


#define ZEXT ".Z"

#ifdef __arm
#undef ZEXT
#define ZEXT "-z"
#endif

typedef struct zfiletype {
#define Z_BITS 16
#define Z_MAXBUF 256
  FILE *file;
  int flags;
  int n_bits;                            /* number of bits/code */
  int maxbits;                           /* user settable max # bits/code */
  long maxcode;                           /* maximum code, given n_bits */
  long free_ent;                          /* first unused entry */
  int block_compress;
  int clear_flg;

  long stackp;
  long finchar;
  long code, oldcode, incode;
  int offset, size;
  unsigned char buf[Z_BITS]; /* Passed to getcode */
  unsigned char PC_HUGE *tab_suffixof;   /* There is a flag bit to say whether */
  long PC_HUGE *tab_prefixof;    /* these have been allocated.         */
  int init;

  int bufput, bufget, bufend;
  unsigned char buff[Z_MAXBUF];
  int c1, c2;
  int zeof;
} ZFILE;

#ifndef __STDC__
ZFILE *zfopen(/* char *fileptr, char *how */);
void zfclose(/* ZFILE *z */);
ZFILE *zfilter(/* FILE *f */);
int zfgetc(/* ZFILE *z */);
int zfeof(/* ZFILE *z */);
char *zfgets(/* char *line, int len, ZFILE *zfp */);
#else
ZFILE *zfopen(char *fileptr, char *how);
void zfclose(ZFILE *z);
ZFILE *zfilter(FILE *f);
int zfgetc(ZFILE *z);
int zfeof(ZFILE *z);
char *zfgets(char *line, int len, ZFILE *zfp);
#endif /* Not __STDC__ */
#endif
SHAR_EOF

