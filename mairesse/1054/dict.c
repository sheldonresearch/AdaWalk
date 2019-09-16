
/* Program to select words from MRC psycholinguistic database */
/* Mike Wilson 24 July 1986 MRC Applied Psychology Unit, Cambridge*/

#define DATAFILE "mrc2.dct"
#define MAXREC 200

#include <stdio.h>
#include <strings.h>
/* #define DEBUG */

main(argc,argv)
int argc;
char *argv[];
{
	
int i,j, nf, wt, w, newinfile,nlet, nphon, nsyl, kfncats, kfnsamp,
	 fam, conc, imag, meanc, meanp, aoa, brownf, whole_line;
static int outa, outb, outc, outd, oute, outf, outg, outh, outi, outj, outk,
	outl, outm, outn, outo, outp, outq, outr, outs, outt, outu, 
	outv, outw, outx, outy, outz;
int nlet_max, nlet_min, nphon_max, nphon_min, nsyl_max, nsyl_min,
	max_kfncats, min_kfncats, brownf_max, brownf_min,
	max_kfnsamp, min_kfnsamp, fam_max, fam_min, conc_max, conc_min,
	imag_max, imag_min, meanc_max, meanc_min, meanp_max, meanp_min, 
	aoa_max, aoa_min, alph_max, alph_min, status_max, status_min;
long kffreq_max, kffreq, t_l_max, t_l_min, min_kffreq, t_l;
char tq2, wtype, pdwtype, alphsyl, status, varp, irreg, outtag,  parse, plural, cap;
char c, *p, *s, *infile, *PSin, *PSout, *PDin, *PDout, *ALPHin, *ALPHout,
*STATUSin, *STATUSout, *TYPEin, *TYPEout, *VARin, *VARout, *IRREGin, *IRREGout, *CAPin, *CAPout, word[40], phon[60], dphon[60], stress[10], *temp;
char line[MAXREC];  /*input line */
FILE *fopen(), *fp;

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

nlet_max =99;
nlet_min =0;
nphon_max =99;
nphon_min =0;
nsyl_max =9;
nsyl_min =0;
kffreq_max =99999;
min_kffreq =0;
max_kfncats =99;
min_kfncats =0;
t_l_max =999999;
t_l_min =0;
max_kfnsamp =999;
min_kfnsamp =0;
brownf_max = 9999;
brownf_min = 0;
fam_max =999;
fam_min =0;
conc_max =999;
conc_min =0;
imag_max =999;
imag_min =0;
meanc_max =999;
meanc_min =0;
meanp_max =999;
meanp_min =0;
aoa_max =999;
aoa_min =0;
parse = 'W';
alph_max = 90 ;
alph_min = 32;
status_max = 90;
status_min = 32;

if (argc < 2){
	usage();
	exit(1);}



	while(argc > 1 ){

		s = (*++argv);

		if  ((i =strcmp("-File", s)) ==0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -File\n");
				exit(1);
			}
			else {
				infile = (*argv);
				newinfile = 1;
				--argc;
			}
		}
		else if ((i = strcmp("-entry", s)) == 0)
			whole_line = 1;
		else if ((i = strcmp("-A", s)) == 0)
				outa = 1;
		else if ((i = strcmp("-B", s)) == 0)
				outb = 1;
		else if ((i = strcmp("-C", s)) == 0)
				outc = 1;
		else if ((i = strcmp("-D", s)) == 0)
				outd = 1;
		else if ((i = strcmp("-E", s)) == 0)
				oute = 1;
		else if ((i = strcmp("-F", s)) == 0)
				outf = 1;
		else if ((i = strcmp("-G", s)) == 0)
				outg = 1;
		else if ((i = strcmp("-H", s)) == 0)
				outh = 1;
		else if ((i = strcmp("-I", s)) == 0)
				outi = 1;
		else if ((i = strcmp("-J", s)) == 0)
				outj = 1;
		else if ((i = strcmp("-K", s)) == 0)
				outk = 1;
		else if ((i = strcmp("-L", s)) == 0)
				outl = 1;
		else if ((i = strcmp("-M", s)) == 0)
				outm = 1;
		else if ((i = strcmp("-N", s)) == 0)
				outn = 1;
		else if ((i = strcmp("-O", s)) == 0)
				outo = 1;
		else if ((i = strcmp("-P", s)) == 0)
				outp = 1;
		else if ((i = strcmp("-Q", s)) == 0)
				outq = 1;
		else if ((i = strcmp("-R", s)) == 0)
				outr = 1;
		else if ((i = strcmp("-S", s)) == 0)
				outs = 1;
		else if ((i = strcmp("-T", s)) == 0)
				outt = 1;
		else if ((i = strcmp("-U", s)) == 0)
				outu = 1;
		else if ((i = strcmp("-V", s)) == 0)
				outv = 1;
		else if ((i = strcmp("-W", s)) == 0)
				outw = 1;
		else if ((i = strcmp("-X", s)) == 0)
				outx = 1;
		else if ((i = strcmp("-Y", s)) == 0)
				outy = 1;
		else if ((i = strcmp("-Z", s)) == 0)
				outz = 1;
 		else if ((i = strcmp("-nletmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -nletmax\n");
				exit(1);
			}
			else {
				nlet_max = atoi(*argv);
					if (nlet_max <= 0)
						bad(*argv,"nletmax");
				--argc;
			}
		}
 		else if ((i = strcmp("-nletmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -nletmin\n");
				exit(1);
			}
			else {
				nlet_min = atoi(*argv);
					if (nlet_min < 0)
						bad(*argv,"nletmin");
				--argc;
			}
		}
 		else if ((i = strcmp("-nphonmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -nphonmax\n");
				exit(1);
			}
			else {
				nphon_max = atoi(*argv);
					if (nphon_max <= 0)
						bad(*argv,"nphonmax");
				--argc;
			}
		}

  		else if ((i = strcmp("-nphonmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -nphonmin\n");
				exit(1);
			}
			else {
				nphon_min = atoi(*argv);
					if (nphon_min <= 0)
						bad(*argv,"nphonmin");
				--argc;
			}
		}

 		else if ((i = strcmp("-nsylmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -nsylmax\n");
				exit(1);
			}
			else {
				nsyl_max = atoi(*argv);
					if (nsyl_max <= 0)
						bad(*argv,"nsylmax");
				--argc;
			}
		}

 		else if ((i = strcmp("-nsylmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -nsylmin\n");
				exit(1);
			}
			else {
				nsyl_min = atoi(*argv);
					if (nsyl_min <= 0)
						bad(*argv,"nsylmin");
				--argc;
			}
		}
		else if ((i = strcmp("-kffreqmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -kffreqmax\n");
				exit(1);
			}
			else {
				kffreq_max = atoi(*argv);
					if (kffreq_max <= 0)
						bad(*argv,"kffreqmax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-kffreqmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -kffreqmin\n");
				exit(1);
			}
			else {
				min_kffreq = atoi(*argv);
					if (min_kffreq <= 0)
						bad(*argv,"minkffreq");
				--argc;
			}
		}
 		else if ((i = strcmp("-kfncatsmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -kfncats\n");
				exit(1);
			}
			else {
				max_kfncats = atoi(*argv);
					if (max_kfncats <= 0)
						bad(*argv,"maxkfncats");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-kfncatsmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -kfncatsmin\n");
				exit(1);
			}
			else {
				min_kfncats = atoi(*argv);
					if (min_kfncats <= 0)
						bad(*argv,"minkfncats");
				--argc;
			}
		}


 
 		else if ((i = strcmp("-t-lmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -t-lmax\n");
				exit(1);
			}
			else {
				t_l_max = atoi(*argv);
					if (t_l_max <= 0)
						bad(*argv,"t-lmax");
				--argc;
			}
		}
 		else if ((i = strcmp("-t-lmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -t-lmin\n");
				exit(1);
			}
			else {
				t_l_min = atoi(*argv);
					if (t_l_min <= 0)
						bad(*argv,"t-lmin");
				--argc;
			}
		}
 		else if ((i = strcmp("-kfnsampmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -kfnsampmax\n");
				exit(1);
			}
			else {
				max_kfnsamp = atoi(*argv);
					if (max_kfnsamp <= 0)
						bad(*argv,"kfnsampmax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-kfnsampmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr,"no argument given to -kfnsampmin\n");
				exit(1);
			}
			else {
				min_kfnsamp = atoi(*argv);
					if (min_kfnsamp <= 0)
						bad(*argv,"kfnsampmin");
				--argc;
			}
		}
 		else if ((i = strcmp("-brownfmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -brownfmax\n");
				exit(1);
			}
			else {
				brownf_max = atoi(*argv);
					if (brownf_max <= 0)
						bad(*argv,"brownfmax");
				--argc;
			}
		}
 		else if ((i = strcmp("-brownfmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -brownfmin\n");
				exit(1);
			}
			else {
				brownf_min = atoi(*argv);
					if (brownf_min <= 0)
						bad(*argv,"brownfmin");
				--argc;
			}
		}
 		else if ((i = strcmp("-fammax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr,"no argument given to -fammax\n");
				exit(1);
			}
			else {
				fam_max = atoi(*argv);
					if (fam_max <= 0)
						bad(*argv,"fammax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-fammin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr,"no argument given to -fammin\n");
				exit(1);
			}
			else {
				fam_min = atoi(*argv);
					if (fam_min <= 0)
						bad(*argv,"fammin");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-concmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -concmax\n");
				exit(1);
			}
			else {
				conc_max = atoi(*argv);
					if (conc_max <= 0)
						bad(*argv,"concmax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-concmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr,"no argument given to -concmin\n");
				exit(1);
			}
			else {
				conc_min = atoi(*argv);
					if (conc_min <= 0)
						bad(*argv,"concmin");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-imagmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -imagmax\n");
				exit(1);
			}
			else {
				imag_max = atoi(*argv);
					if (imag_max <= 0)
						bad(*argv,"imagmax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-imagmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -imagmin\n");
				exit(1);
			}
			else {
				imag_min = atoi(*argv);
					if (imag_min <= 0)
						bad(*argv,"imagmin");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-meancmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -meancmax\n");
				exit(1);
			}
			else {
				meanc_max = atoi(*argv);
					if (meanc_max <= 0)
						bad(*argv,"meancmax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-meancmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -meancmin\n");
				exit(1);
			}
			else {
				meanc_min = atoi(*argv);
					if (meanc_min <= 0)
						bad(*argv,"meancmin");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-meanpmax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -meanpmax\n");
				exit(1);
			}
			else {
				meanp_max = atoi(*argv);
					if (meanp_max <= 0)
						bad(*argv,"meanpmax");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-meanpmin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -meanpmin\n");
				exit(1);
			}
			else {
				meanp_min = atoi(*argv);
					if (meanp_min <= 0)
						bad(*argv,"meanpmin");
				--argc;
			}
		}

 
 		else if ((i = strcmp("-aoamax", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -aoamax\n");
				exit(1);
			}
			else {
				aoa_max = atoi(*argv);
					if (aoa_max <= 0)
						bad(*argv,"aoamax");
				--argc;
			}
		}
 		else if ((i = strcmp("-aoamin", s)) == 0){
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -aoamin\n");
				exit(1);
			}
			else {
				aoa_min = atoi(*argv);
					if (aoa_min <= 0)
						bad(*argv,"aoamin");
				--argc;
			}
		}
		else if ((i = strcmp("+PS", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +PS\n");
				exit(1);
			}
			else {
				i=0;
				temp = (*argv);
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'N' && (temp [i]) != 'J' && (temp [i]) != 'V' && (temp [i]) != 'A' && (temp [i]) != 'R' && (temp [i]) != 'C' && (temp [i]) != 'U' && (temp [i]) != 'I' && (temp [i]) != 'P' && (temp [i]) != 'O' )
						fprintf(stderr, "Wrong argument to +PS: %c\n", (temp [i]));
				i++;
				}
				strcpy(PSin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-PS", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -PS\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'N' && (temp [i]) != 'J' && (temp [i]) != 'V' && (temp [i]) != 'A' && (temp [i]) != 'R' && (temp [i]) != 'C' && (temp [i]) != 'U' && (temp [i]) != 'I' && (temp [i]) != 'P' && (temp [i]) != 'O' )
						fprintf(stderr, "Wrong argument to -PS: %c \n", (temp [i]));
				i++;
				}
				strcpy(PSout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+PD", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +PD\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'N' && (temp [i]) != 'J' && (temp [i]) != 'V' && (temp [i]) != 'O'  )
						fprintf(stderr, "Wrong argument to +PD: %c \n", (temp [i]));
				i++;
				}
				strcpy(PDin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-PD", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-'){
				fprintf(stderr, "no argument given to -PD\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'N' && (temp [i]) != 'J' && (temp [i]) != 'V' && (temp [i]) != 'O' )
						fprintf(stderr, "Wrong argument to -PD: %c \n", (temp [i]));
				i++;
				}
				strcpy(PDout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+ALPH", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +ALPH\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'S' && (temp [i]) != 'H' && (temp [i]) != 'A' && (temp [i]) != 'T' && (temp [i]) != 'P'  )
						fprintf(stderr, "Wrong argument to +ALPH: %c \n", (temp [i]));
				i++;
				}
				strcpy(ALPHin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-ALPH", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -ALPH\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'S' && (temp [i]) != 'H' && (temp [i]) != 'A' && (temp [i]) != 'T' && (temp [i]) != 'P'  )
						fprintf(stderr, "Wrong argument to -ALPH: %c \n", (temp [i]));
				i++;
				}
				strcpy(ALPHout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+IRREG", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +IRREG\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'Z' && (temp [i]) != 'Y' && (temp [i]) != 'B' && (temp [i]) != 'P' && (temp [i]) != 'N'  )
						fprintf(stderr, "Wrong argument to +IRREG: %c \n", (temp [i]));
				i++;
				}
				strcpy(IRREGin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-IRREG", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -IRREG\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'Y' && (temp [i]) != 'B' && (temp [i]) != 'Z' && (temp [i]) != 'P' && (temp [i]) != 'N'  )
						fprintf(stderr, "Wrong argument to -IRREG: %c \n", (temp [i]));
				i++;
				}
				strcpy(IRREGout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+TYPE", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +TYPE\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'Q' && (temp [i]) != '2' )
						fprintf(stderr, "Wrong argument to +TYPE: %c \n", (temp [i]));
				i++;
				}
				strcpy(TYPEin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-TYPE", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -TYPE\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'Q' && (temp [i]) != '2'  )
						fprintf(stderr, "Wrong argument to -TYPE: %c \n", (temp [i]));
				i++;
				}
				strcpy(TYPEout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+VAR", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +VAR\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'B' && (temp [i]) != 'O' )
						fprintf(stderr, "Wrong argument to +VAR: %c \n", (temp [i]));
				i++;
				}
				strcpy(VARin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-VAR", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -VAR\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'B' && (temp [i]) != 'O'  )
						fprintf(stderr, "Wrong argument to -VAR: %c \n", (temp [i]));
				i++;
				}
				strcpy(VARout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+STATUS", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +STATUS\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != '$' && (temp [i]) != 'A' && (temp [i]) != 'C' && (temp [i]) != 'D' && (temp [i]) != 'E' && (temp [i]) != 'F' && (temp [i]) != 'H' && (temp [i]) != 'O' && (temp [i]) != 'P' && (temp [i]) != 'Q'&& (temp [i]) != 'R' && (temp [i]) != 'S'&& (temp [i]) != 'W')
						fprintf(stderr, "Wrong argument to +STATUS: %c \n", (temp [i]));
				i++;
				}
				strcpy(STATUSin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-STATUS", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -STATUS\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != '$' && (temp [i]) != 'A' && (temp [i]) != 'C' && (temp [i]) != 'D' && (temp [i]) != 'E' && (temp [i]) != 'F' && (temp [i]) != 'H' && (temp [i]) != 'O' && (temp [i]) != 'P' && (temp [i]) != 'Q'&& (temp [i]) != 'R' && (temp [i]) != 'S'&& (temp [i]) != 'W')
						fprintf(stderr, "Wrong argument to -STATUS: %c \n", (temp [i]));
				i++;
				}
				strcpy(STATUSout, temp);
				--argc;
			}
		}
		else if ((i = strcmp("+CAP", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to +CAP\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'C'  )
						fprintf(stderr, "Wrong argument to +CAP: %c \n", (temp [i]));
				i++;
				}
				strcpy(CAPin, temp);
				--argc;
			}
		}
		else if ((i = strcmp("-CAP", s)) == 0) {
			if ((argc -1) == 1 || (*++argv) [0] == '-' ){
				fprintf(stderr, "no argument given to -CAP\n");
				exit(1);
			}
			else {
				i=0;
				temp = *argv;
				while((temp [i]) != '\0'){
					if ((temp [i]) != 'C'  )
						fprintf(stderr, "Wrong argument to -CAP: %c \n", (temp [i]));
				i++;
				}
				strcpy(CAPout, temp);
				--argc;
			}
		}
		else {
			fprintf(stderr, "Illegal Switch %s\n", s);
			exit(1);
		}
		--argc;
	}
	
#ifdef DEBUG		
  		      printf(" nlet-max: %d\n ",  nlet_max );
  		      printf(" nlet-min: %d\n ",  nlet_min );
  		      printf(" %d\n ",  nphon_max);
  		      printf(" %d\n ",  nphon_min);
  		      printf(" %d\n ",  nsyl_max);
  		      printf(" %d\n ",  nsyl_min);
  		      printf(" kffmax: %d\n ",  kffreq_max );
  		      printf(" kffmin: %d\n ",  min_kffreq );
  		      printf(" %d\n ",  max_kfncats);
  		      printf(" %d\n ",  min_kfncats);
  		      printf(" %d\n ",  t_l_max);
  		      printf(" %d\n ",  t_l_min);
  		      printf(" %d\n ",  max_kfnsamp);
  		      printf(" %d\n ",  min_kfnsamp);
  		      printf(" %d\n ",  fam_max);
  		      printf(" %d\n ",  fam_min);
  		      printf(" %d\n ",  conc_max);
  		      printf(" %d\n ",  conc_min);
  		      printf(" %d\n ",  imag_max);
  		      printf(" %d\n ",  imag_min);
  		      printf(" %d\n ",  meanc_max);
  		      printf(" %d\n ",  meanc_min);
  		      printf(" %d\n ",  meanp_max);
  		      printf(" %d\n ",  meanp_min);
  		      printf(" %d\n ",  aoa_max );
  		      printf(" %d\n ",  aoa_min );
  		      

#endif

/* open database file */
                if (newinfile != 1)
			infile = DATAFILE;

		if ((fp = fopen (infile, "r")) == NULL) {
			fprintf(stderr, " can't open  %s  as data file \n", infile);
			exit(1);
		}

/* read database lines until EOF*/
	while (fgets(line, 124, fp) != NULL) {
/*parse line*/
			nlet = itoaa(0,1,line); /*number of letters */
			nphon = itoaa(2,3,line); /*number of phomemes */
			nsyl = line[4] - '0';	/* number of syllables in word*/
			kffreq = itoaa(5,9,line); /*k & f frequency*/
			kfncats = itoaa(10,11,line); /*no of k&f categories */
			kfnsamp = itoaa(12,14,line); /*no of k&f samples */
			t_l = itoaa(15,20,line); /*Thorndyke-Lorge freq*/
			brownf = itoaa(21,24,line); /*Brown's Spoken frequency */
			fam = itoaa(25,27,line); /*Familiarity*/
			conc = itoaa(28,30,line); /*Concreteness*/
			imag = itoaa(31,33,line); /*Imagery*/
			meanc = itoaa(34,36,line); /*mean Colerado meaningfulness*/
			meanp = itoaa(37,39,line); /*mean pavio meaningfulness*/
			aoa = itoaa(40,42,line); /*mean pavio meaningfulness*/
			tq2 = line[43];			/* type*/
			wtype = line[44]; 		/* part of speech*/
			pdwtype = line[45];		/* PD part of speech*/
			alphsyl = line[46];		/* Alphasyllable*/
			status = line[47];		/* Status*/
			varp = line[48];		/* Varient phoneme */
			cap = line[49];
			irreg = line[50];		/* irregular plural */
			w = 51;
			wt = 0;
			while (line[w] != '|'){
				word[wt] = line[w];		/* words itself*/
				w++;
				wt++;}
			word[wt] = '\0';
			++w;
			wt = 0;
			while (line[w] != '|'){
				phon[wt] = line[w];		/* phonetic transcription */
				w++;
				wt++;}
			phon[wt] = '\0';
			wt = 0;
			++w;
			while (line[w] != '|'){
				dphon[wt] = line[w];	/* edited phon transcription */
				w++;
				wt++;}
			dphon[wt] = '\0';
			wt = 0;
			++w;
			while (line[w] != '|' && line[w] != '\0'){
				stress[wt] = line[w];	/* stress pattern */
				w++;
				wt++;}
			stress[wt] = '\0';

/* test this line to see if it fits within the required limits */

#ifdef DEBUG
  		      printf(" before test nlet: %d\n ",  nlet);
  		      printf("nphon %d\n ",  nphon);
  		      printf("nsyl %d\n ",  nsyl);
  		      printf("kffreq %d\n ",  kffreq );
  		      printf("kfncats %d\n ",  kfncats);
  		      printf("t_l %d\n ",  t_l);
  		      printf("brownf %d\n ",  brownf);
  		      printf("kfnsamp %d\n ",  kfnsamp);
  		      printf("fam %d\n ",  fam);
  		      printf("conc %d\n ",  conc);
  		      printf("imag %d\n ",  imag);
  		      printf("meanc %d\n ",  meanc);
  		      printf("meanp %d\n ",  meanp);
  		      printf("aoa %d\n ",  aoa);
  		      printf("cap %c\n ",  cap);
#endif 

if ( inrange(nlet_max, nlet, nlet_min)  &&
 inrange( nphon_max, nphon, nphon_min) &&
 inrange(nsyl_max, nsyl, nsyl_min) &&
 inrange(kffreq_max,  kffreq, min_kffreq) &&
 inrange(max_kfncats, kfncats, min_kfncats) && 
 inrange(max_kfnsamp, kfnsamp, min_kfnsamp) &&
 inrange(t_l_max, t_l, t_l_min) &&
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
	 index(ALPHin, alphsyl)  &&
 	outstring(ALPHout, alphsyl) ) {

/* items that pass here fit the criteria and should be output in some form*/

		if (outa == 1 )
			printf(" %d ",nlet);
		if (outb == 1 )
			printf(" %d ",nphon);
		if (outc == 1 )
			printf(" %d ",nsyl);
		if (outd == 1 )
			printf(" %d ",kffreq);
		if (oute == 1 )
			printf(" %d ",kfncats);
		if (outf == 1 )
			printf(" %d ",kfnsamp);
		if (outg == 1 )
			printf(" %d ",t_l);
		if (outh == 1)
		      printf(" %d ",brownf);
		if (outi == 1 )
			printf(" %d ",fam);
		if (outj == 1 )
			printf(" %d ",conc);
		if (outk == 1 )
			printf(" %d ",imag);
		if (outl == 1 )
			printf(" %d ",meanc);
		if (outm == 1 )
			printf(" %d ",meanp);
		if (outn == 1 )
			printf(" %d ",aoa);
		if (outo == 1 )
			printf(" %c ",tq2);
		if (outp == 1 )
			printf(" %c ",wtype);
		if (outq == 1 )
			printf(" %c ",pdwtype);
		if (outr == 1 )
			printf(" %c ",alphsyl);
		if (outs == 1 )
			printf(" %c ",status);
		if (outt == 1 )
			printf(" %c ",varp);
		if (outu == 1)
		      printf(" %c ",cap);
		if (outv == 1 )
			printf(" %c ",irreg);
		if (outw == 1 )
		      printf(" %s ", word);
		if (outx == 1 )	
  		      printf(" %s ", phon);
  		if (outy == 1 )
  		      printf(" %s ", dphon);
  		if (outz == 1 )
  		      printf(" %s ", stress);
		if (whole_line != 1)
  			printf("\n");
		else printf("%s", line);
		 }
	    }
	}
}

itoaa(s, e, line)   /* returns value for variable */
int s, e;
char *line;
{
	int i, n;
			n = 0;               
			for(i = s; i <= e; i++)
			  n = 10 * n + line[i] - '0'; 
			return(n);
}

bad(v,s)
char *v;
char *s;
{	printf("\"%s\": bad %s specification\n",v,s);	
	exit();}


inrange(max, val, min)
int max, val, min;
{
if (max >= val && min <= val)
		return(1);
else return(0);
}

outstring(string, c)
char c, string[];
{

	if (index(string, c) != 0)
		return(0);
	else return(1);
}

usage()
{
			fprintf(stderr, "Usage: dict -a N [ ... -z N] -A [... V][X]\n");
			fprintf(stderr,"\n");
			fprintf(stderr,"MRC Psycholinguistic Database - mrc2\n");
			fprintf(stderr,"see QJEP 1981, 33A, 497-505 \n");
			fprintf(stderr,"\n");
			fprintf(stderr,"-File filename - non-default input file\n\n");
			fprintf(stderr,"Output selection:\n");
			fprintf(stderr,"-A NLET number of letters\n");
			fprintf(stderr,"-B NPHON number of phonemes\n");
			fprintf(stderr,"-C NSYL number of syllables\n");
			fprintf(stderr,"-D K-F-FREQ Kucera-Francis written frequency\n");
			fprintf(stderr,"-E K-F-NCATS Kucera-Francis number of categories\n");
			fprintf(stderr,"-F K-F-NSAMP Kucera-Francis number of samples\n");
			fprintf(stderr,"-G T-L-FREQ Thorndyke-Lorge written Frequency\n");
			fprintf(stderr,"-H BROWNF Brown verbal frequency\n");
			fprintf(stderr,"-I FAM Familiarity\n");
			fprintf(stderr,"-J CONC Concreteness\n");
			fprintf(stderr,"-K IMAG Imagery\n");
			fprintf(stderr,"-L MEANC Mean Colerado (Meaningfulness)\n");
			fprintf(stderr,"-M MEANP Mean Pavio (Meaningfulness)\n");
			fprintf(stderr,"-N AOA Age of Aquisition\n");
			fprintf(stderr,"-O TQ2 Type\n");
			fprintf(stderr,"-P WTYPE   Part of Speech\n");
			fprintf(stderr,"-Q PDWTYPE Common part of speech\n");
			fprintf(stderr,"-R ALPHSYL Alphasyllable\n");
			fprintf(stderr,"-S STATUS\n");
			fprintf(stderr,"-T VAR Varient phoneme\n");
			fprintf(stderr,"-U CAP Capitalisation of words\n");
			fprintf(stderr,"-V IRREG Irregular plural\n");
			fprintf(stderr,"-W WORD\n");
			fprintf(stderr,"-X PHON Phonetic transcription\n");
			fprintf(stderr,"-Y DPHON Edited phonetic transcription\n");
			fprintf(stderr,"-Z STRESS\n");
			fprintf(stderr,"-entry The whole entry\n");
			fprintf(stderr,"\n");

			fprintf(stderr,"Restriction by category\n\n");
			fprintf(stderr,"You specify the max and min on each scale\n \n");

			fprintf(stderr,"-nletmax N  max length \n");
			fprintf(stderr,"-nletmin N  min length\n");
			fprintf(stderr,"-nphonmax N  max num phonemes\n");
			fprintf(stderr,"-nphonmin N  min num phonemes\n");
			fprintf(stderr,"-nsylmax N  max num syllables\n");
			fprintf(stderr,"-nsylmin N  min num syllables\n");
			fprintf(stderr,"-kffreqmax N  max kf frequency\n");
			fprintf(stderr,"-kffreqmin N  min kf frequency  > 0\n");
			fprintf(stderr,"-kfncatsmax N  max kf number categories\n");
			fprintf(stderr,"-kfncatsmin N  min kf number categories\n");
			fprintf(stderr,"-kfnsampmax N  max kf number samples\n");
			fprintf(stderr,"-kfnsampmin N  min kf number samples\n");
			fprintf(stderr,"-t-lmax N  max thorndyke-lorge\n");
			fprintf(stderr,"-t-lmin N  min thorndyke-lorge\n");
			fprintf(stderr,"-brownfmax N  max Brown verbal frequency\n");
			fprintf(stderr,"-brownfmin N  min Brown verbal frequency\n");
			fprintf(stderr,"-fammax N  max familiarity  < 700\n");
			fprintf(stderr,"-fammin N  min familiarity  > 100\n");
			fprintf(stderr,"-concmax N  max concreteness < 700\n");
			fprintf(stderr,"-concmin N  min concreteness > 100\n");
			fprintf(stderr,"-imagmax N  max imagability  < 700\n");
			fprintf(stderr,"-imagmin N  min imagability  > 100\n");
			fprintf(stderr,"-meancmax N  max mean Colorado meaningfulness < 700\n");
			fprintf(stderr,"-meancmin N  min mean Colorado meaningfulness > 700\n");
			fprintf(stderr,"-meanpmax N  max mean pavio meaningfulness < 700\n");
			fprintf(stderr,"-meanpmin N  min pavio meaningfulness > 100\n");
		fprintf(stderr,"-aoamax N  max age of aquisition  < 700\n");

		fprintf(stderr,"-aoamin N  min age of aquisition  > 100\n");
		fprintf(stderr,"\n Select entries which include (+) or exclude (-) values (arguments) for linguistic properties (flags)\n \n");
		fprintf(stderr,"-+TYPE Q2\n");
		fprintf(stderr,"-+IRREG ZBYPN\n");	
		fprintf(stderr,"-+VAR BO\n");	
		fprintf(stderr,"-+ALPH SPHAT\n");	
		fprintf(stderr,"-+STATUS $ACDEFHOPQRSW\n");	
		fprintf(stderr,"-+PS NJVARCUIPO\n");
		fprintf(stderr,"-+PD NJVO\n");
		fprintf(stderr,"-+CAP C\n");
	exit();
}
