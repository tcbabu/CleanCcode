/*
   Version 3.0
   Dated 27/05/2024
*/

#ifndef D_LINK
#define D_LINK
#ifdef __cplusplus
extern "C"
{
#endif

#include <stdio.h>
#include <ctype.h>
#include <malloc.h>
#include <string.h>

typedef  struct d_l {
         void  *bf;
         struct d_l *nx;
         struct d_l *pv;
} D_l;
typedef  struct l_L {
         D_l *st; 
         D_l *cr;
         D_l *en;
} Dlink;

typedef struct _ArgStruct {
        char *flag; //option flag with -/-- 
        void *pt;   // pointer to set value
        int code;   // code for pointer; 'i' integer,'l' long, 'f' double, 'c' char, 's' string
        int nvals;  // number values to store in 'pt'
        void (*Callback)(void *); // Call back if any ,can be NULL
        void *arg;   // argument for callback ,can be NULL
} DARGS;



Dlink * Dopen(void);
int Ddup( Dlink *S,Dlink *D);
int  Dappend(Dlink *F,void  *buf);
void Dempty(Dlink *F);
void Dfree(Dlink *F);
void Drewind(Dlink *F) ;
void Dend (Dlink *F) ;
void Dmove_back(Dlink *LN,int n) ;
void Dmove_forward(Dlink *LN,int n) ;
void Ddelete_items(Dlink *LN,int n) ;
void Ddelete(Dlink *LN) ;
void Dinsert(Dlink *F,void  *buf) ;
void Dadd(Dlink *F,void  *buf) ;
Dlink *Dcopy(Dlink *LN) ;
Dlink *Dsublist(Dlink *LN,void *s,int Dcondition(void *,void *)) ;
Dlink *Dnewlist(Dlink *LN,void * Dnewrule(void *)) ;
/*
  The comparison return 1 if the criteria satisfied
  and repositioning is done
  the first arg(the current value,ie the one coming first in the
  current order) is compared with the
  second( ie the one coming later).
  Example:
    To compare an array of numbers is in increasing order
    int compno(void *first,void *second) {
      int ret = -1;
      if ( *((int *)first > *((int *)second) ) ret =1;
      return ret;
    }
    String based sort in ascenting order
int SortVars(void *p1,void *p2) {
	int val =0;
	val =  strcmp(((VARS *)p1)->name,((VARS*)p2)->name);
	if(val > 0) return 1;
	else return 0;
}

*/
void Dsort(Dlink *LN , int Dcmpitems(void *,void *)) ;
// order = 0 ascenting
// else decenting
// field is counted from 1
void Dstringsort(Dlink *LN , int field,int order);
void Dnumbersort(Dlink *LN , int field,int order);
void Dsort(Dlink *LN , int Dcmpitems(void *,void *)) ;
int Dcount(Dlink *F) ;
void Dposition(Dlink *F,int n) ;
int Dsetcurpos(Dlink *LN,void *s,int Dcondition(void *,void *)) ;
Dlink *Djoin(Dlink *d,Dlink *s);
int Disum(Dlink *LN , int Dsumitem(void *)) ;
float Dfsum(Dlink *LN , float Dsumitem(void *)) ;
double Ddsum(Dlink *LN , double Dsumitem(void *)) ;
int comp_list(Dlink *L1,Dlink *L2,int comprecord(void *tmp1,void *tmp2)) ;
void Dcondition(Dlink *F,void *arg, int condition(void *,void *));
int Dread(Dlink **L,FILE *f,void * read_link(FILE *fp));
int Dwrite(Dlink *L,FILE *f,int write_link(FILE *fp,void *buf));
int Dreadauto(Dlink **L,char *flname,int size_of_rec);
int Dwriteauto(Dlink *L,char *flname,int size_of_rec);
int Drmvdup(Dlink *L,int size_of_rec);
int Drmvdup_cond(Dlink *L,int rule(void *,void *));
    /* if rule returns 1 records are same */
void *Resetlink(Dlink *L);
void *Getrecord(Dlink *L);
void *Getrecordrev(Dlink *L);
void* Dpick(Dlink *LN) ; /* like Ddelete but does not free */
//
// New ones with position starting from 0
//
int Dlocation(Dlink *L,int pos) ; // pos starts from 0
int Dremove(Dlink *L,int pos); // pos counted from 0 ; deletes item and free
void * Dtake(Dlink *L,int pos); // pos counted from 0; delete item and retuns record
void *Drecord(Dlink *L,int position); 
void Dtravel(Dlink *L,void (*action) (void *));
Dlink * DgetFlags(char **argv);
Dlink * DgetArgs(char **argv);
char * DgetFlagArg(Dlink *Alist,char **argv,char *flag,int nv); //Alist from DgetArgs
#if 0
//  example code  'DprocessFlags(...)'
#define D_NOCLEANCC
  void Usage ( void *pt ) {
      char **Name = (char **)(pt);
      fprintf ( stderr , "Usage: "
		        "%s <-i indent offset> "
		        " <-s start  offset> "
		        " <-o outfile> "
		        " [inputfile] \n" ,*Name) ;
          
      exit ( 0 ) ;
  }
  DARGS Parg [ ] = {
      {"-i" , &Shiftfac , 'i' , 1 , NULL , NULL} ,
      {"-s" , &StartOffset , 'i' , 1 , NULL , NULL} , 
      {"-b" , &OffSet , 'i' , 1 , NULL , NULL} ,
      {"-o" , &OutFile , 's' , 1 , NULL , NULL} , 
      {"-h" , NULL , 'i' , 1 , *Usage , &PgrName } ,
      {"--help" , NULL , 'i' , 1 , *Usage , &PgrName } ,
      {NULL , NULL}
  };
#define D_CLEANCC
 int main ( int argc , char **argv ) {
     char **Rargs = NULL;
     Dlink *S = NULL;
     PgrName = argv [ 0 ] ;
     Rargs = DprocessFlags ( argv , Parg ) ;
     if ( ( Rargs == NULL ) || ( Rargs [ 0 ] == NULL ) ) {
         Usage ( & PgrName ) ;
     }
     S = prepro ( Rargs [ 0 ] ) ;
     if ( OutFile == NULL ) Dwritefile ( S , Rargs [ 0 ] ) ;
     else Dwritefile ( S , OutFile ) ;
     return 1;
 }
#endif
char **  DprocessFlags(char *argv[],DARGS fargpt[]);
Dlink *Dreadfile(char *flname);
int Dwritefile(Dlink *L,char *flname);
int Dpush(Dlink *L,void *bf);
void * Dpop(Dlink *L);
int Dfifoin(Dlink *L,void *bf);
void * Dfifoout(Dlink *L);
int  Dreplace(Dlink *L,void *bf,int pos);
#if 0
/ *
 * Sample code for compare function
 * LINK MUST BE IN ASCENTING ORDER
 */

int CompareAction(void *rec, char *name) {
	/* 
	 * return -1 if rec field < name
	 * return  1 if rec field > name
	 * return  0 if rec field = name
	 *
	 */
	PACTION  *recv = ((PACTION  *)rec);
//	int val = *((int *) name);
	char *field = recv->Flag;;
	// field = ...
	return strcmp(field,(char *)name);
//	return (recv - val);
}
 
#endif
void *Dsearch(Dlink *L, char *val  ,int (*Compare)(void *,char *));


#ifdef __cplusplus
}
#endif
#endif /* end of dlink.h */
