#include <stdio.h>
#include "dlink.h"
#include <stdlib.h>
#include <string.h>
  static int Shiftfac = 4;
  static int OffSet = 4;
  static int StartOffset = 2;
  static char *OutFile = NULL;
  static char *PgrName = NULL;
#define SHIFT { \
  dpt [ 0 ] = '\0';\
      for ( int n = 0;n < ( Shift-1 ) ;n++ ) \
      { strcat ( dpt , SBUFF ) ; }\
      for ( int n = 0;n < StartOffset;n++ ) strcat ( dpt , " " ) ;\
  }
  static int GetBracketField ( char *tmp , char *Tag , int *ln ) {
      int i , j = 0;
      char *pt = NULL;
      int ret;
      char buff [ 500 ] ;
      int count = 0;
      Tag [ 0 ] = '\0';
      pt = tmp;
      i = 0;
      while ( ( *pt == ' ' ) || ( *pt == '\t' ) ) {
          if ( *pt == '\t' ) *pt = ' ';
          i++;
          pt++;
      }
      if ( ( *pt < ' ' ) ) {*ln = i;return i;}
      if ( ( *pt == ';' ) ) {*ln = i;return i;}
      if ( ( *pt != '(' ) ) {*ln = i;return i;}
      count = 1;
      pt++;i++;
      j = 0;
      while ( *pt == ' ' ) {pt++;i++;}
//	while(*pt != ';') {
      while ( 1 ) {
          if ( ( *pt < ' ' ) && ( *pt != '\t' ) ) {
              fprintf ( stderr , "In getting Bracket field:%s\n" , tmp ) ;
              buff [ j ] = '\0';
              strcpy ( Tag , buff ) ;
              return 0;
          }
          if ( *pt == '(' ) count++;
          if ( *pt == ')' ) count--;
          if ( count == 0 ) {pt++;i++;break;}
          if ( *pt == '\'' ) {
              buff [ j++ ] = *pt++;i++;
              while ( *pt != '\'' ) {
                  if ( *pt == '\\' ) {buff [ j++ ] = *pt++;i++;}
                  buff [ j++ ] = *pt++;i++;
              }
          }
          if ( *pt == '"' ) {
              buff [ j++ ] = *pt++;i++;
              while ( *pt != '"' ) {
                  if ( *pt == '\\' ) {buff [ j++ ] = *pt++;i++;}
                  buff [ j++ ] = *pt++;i++;
              }
          }
          buff [ j++ ] = *pt++;i++;
      }
      while ( buff [ j-1 ] == ' ' ) {
          j--;
          if ( j <= 0 ) break;
      }
      buff [ j ] = '\0';
      strcpy ( Tag , buff ) ;
      *ln = i;
      return *ln;
  }
  static int GetNextTag ( char *tmp , char *Tag , int *ln ) {
      int i , j = 0;
      char *pt = NULL;
      int ret;
      char buff [ 500 ] ;
      int ok = 1;
      Tag [ 0 ] = '\0';
      pt = tmp;
      i = 0;
      while ( ( *pt == ' ' ) || ( *pt == '\t' ) ) {
          if ( ( *pt != '\t' ) ) *pt = ' ';
          i++;
          pt++;
      }
      if ( ( *pt < ' ' ) ) {*ln = i;return i;}
      j = 0;
      while ( ( *pt > ' ' ) ) {
          ok = 1;
          if ( *pt == '\'' ) {
              buff [ j++ ] = *pt++;i++;
              while ( *pt != '\'' ) {
                  if ( *pt == '\\' ) {buff [ j++ ] = *pt++;i++;}
                  if ( ( *pt == '\n' ) || ( *pt == '\0' ) ) {ok = 0;break;}
                  buff [ j++ ] = *pt++;
                  i++;
              }
          }
          if ( *pt == '"' ) {
              buff [ j++ ] = *pt++;i++;
              while ( *pt != '"' ) {
                  if ( *pt == '\\' ) {buff [ j++ ] = *pt++;i++;}
                  if ( ( *pt == '\n' ) || ( *pt == '\0' ) ) {ok = 0;break;}
                  buff [ j++ ] = *pt++;
                  i++;
              }
          }
          if ( ok ) {
              Tag [ j++ ] = *pt++;
              i++;
          }
      }
      Tag [ j ] = '\0';
      *ln = i;
      return *ln;
  }
  static int OldGetNextTag ( char *tmp , char *Tag , int *ln ) {
      int i , j = 0;
      char *pt = NULL;
      int ret;
      char buff [ 500 ] ;
      Tag [ 0 ] = '\0';
      pt = tmp;
      i = 0;
      while ( ( *pt == ' ' ) || ( *pt == '\t' ) ) {
          if ( ( *pt != '\t' ) ) *pt = ' ';
          i++;
          pt++;
      }
      if ( ( *pt < ' ' ) ) {*ln = i;return i;}
      j = 0;
      while ( ( *pt > ' ' ) ) {
//            if ( *pt == '\n' ) break;
          Tag [ j++ ] = *pt++;i++;
      }
      Tag [ j ] = '\0';
      *ln = i;
      return *ln;
  }
  static int CheckTag ( char *bf , char *Token ) {
      char Tag [ 500 ] ;
      char *pt;
      int ln;
      pt = bf;
      while ( ( *pt == ' ' ) || ( *pt == '\t' ) ) pt++;
      if ( *pt < ' ' ) return 0;
      if ( *pt != '#' ) return 0;
      pt++;
      GetNextTag ( pt , Tag , & ln ) ;
      if ( Tag [ 0 ] == '\0' ) return 0;
      pt += ln;
      if ( strcmp ( Tag , "define" ) == 0 ) {
          GetNextTag ( pt , Tag , & ln ) ;
          if ( Tag [ 0 ] == '\0' ) return 0;
          pt += ln;
      }
      else return 0;
      if ( strcmp ( Tag , Token ) == 0 ) {
          return 1;
      }
      else return 0;
  }
  static int ProcessNoClean ( Dlink *S , Dlink *P ) {
      char *bf;
      char *pt , *fpt;
      int ln;
      char Tag [ 500 ] ;
      Dmove_back ( S , 1 ) ;
      bf = ( char * ) Getrecord ( S ) ;
      pt = bf;
      if ( CheckTag ( bf , "D_NOCLEANCC" ) ) {
          bf = ( char * ) Getrecord ( S ) ;
          fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
          strcpy ( fpt , bf ) ;
          Dadd ( P , fpt ) ;
          while ( CheckTag ( bf , "D_CLEANCC" ) == 0 ) {
              bf = ( char * ) Getrecord ( S ) ;
              fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( fpt , bf ) ;
              Dadd ( P , fpt ) ;
          }
      }
      return 1;
  }
  static int CheckSyntax ( char *bf ) {
      char Tag [ 500 ] ;
      int ln;
      char *pt;
      int i = 0;
      char ch;
      pt = bf;
      GetNextTag ( pt , Tag , & ln ) ;
      if ( ( strcmp ( Tag , "if" ) == 0 ) || ( strcmp ( Tag , "while" ) == 0 ) 
          || ( strcmp ( Tag , "for" ) == 0 ) ) {
          pt += ln;
          while ( *pt != '(' ) {
              if ( *pt == '\0' ) return 1;
              pt++;
          }
          GetBracketField ( pt , Tag , & ln ) ;
          i = 0;
          while ( Tag [ i ] != '\0' ) {
              switch ( Tag [ i ] ) {
                  case '=':
                  if ( Tag [ i+1 ] == '=' ) {
                      i++;break;
                  }
                  ch = Tag [ i-1 ] ;
                  if ( ( ch != '!' ) && ( ch != '<' ) && ( ch != '>' ) && ( ch != '\'' ) ) {
                      fprintf ( stderr , "Doubtful '=': %s\n" , Tag ) ;
                  }
                  break;
                  default:
                  break;
              }
              i++;
          }
      }
      else return 1;
  }
  static int Strcat ( char *dest , char*sou ) {
      int i = 0 , j = 0;
      char *dpt , *spt;
      char ch;
      dpt = dest;
      spt = sou;
//	printf("Dest: %s %s",dest,sou);
      while ( *dpt != '\0' ) dpt++;
      while ( *spt != '\0' ) {
          ch = *spt;
          *dpt = *spt;
          switch ( ch ) {
              case '"':
              case '\'':
              spt++;dpt++;
              while ( *spt != ch ) {
                  *dpt = *spt;
                  if ( *spt == '\\' ) {dpt++;spt++;*dpt = *spt;}
                  dpt++;spt++;
              }
              *dpt = *spt;
              dpt++;spt++;
              break;
              case ' ':
              spt++;
              while ( * ( spt ) == ' ' ) spt++;
              dpt++;
              break;
              default:
              dpt++; spt++;
              break;
          }
      }
      *dpt = '\0';
      return 1;
  }
  static int DADD ( Dlink *S , char *bf ) {
      char buff [ 1000 ] ;
      char buff1 [ 1000 ] ;
      char *fpt , *pt , *spt;
      int i;
      int len , maxlen;
      int blanks , blanksplus;
      int linelow = 70 , lineup = 80 , size;
      i = 0;
      fpt = bf;
      while ( *fpt != '\n' ) {
          switch ( *fpt ) {
              case '-':
              if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '>' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '=' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else {
                  buff [ i ] = *fpt;
              }
              break;
              case '+':
              if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '+' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '=' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else {
                  buff [ i ] = *fpt;
              }
              break;
              case '%':
              if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '=' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else {
                  buff [ i ] = *fpt;
              }
              break;
              case '*':
              if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '=' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else {
                  buff [ i ] = *fpt;
              }
              break;
              case '/':
              if ( ( fpt [ 1 ] == ' ' ) && ( fpt [ 2 ] == '=' ) ) {
                  buff [ i ] = *fpt;
                  fpt++;
              }
              else {
                  buff [ i ] = *fpt;
              }
              break;
              default:
              buff [ i ] = *fpt;
              break;
          }
          i++;
          fpt++;
      }
      while ( buff [ i-1 ] == ' ' ) i--;
      buff [ i++ ] = '\n'; buff [ i ] = '\0';
      if ( buff [ i-2 ] == '\\' ) {
          fpt = ( char * ) malloc ( strlen ( buff ) +1 ) ;
          strcpy ( fpt , buff ) ;
          Dadd ( S , fpt ) ;
          return 1;
      }
      if ( ( buff [ i-2 ] == ':' ) && ( buff [ i-3 ] == ':' ) && 
          ( buff [ i-4 ] == ':' ) ) {
          buff [ i-4 ] = '\n';
          buff [ i-3 ] = '\0';
      }
      blanks = 0;
      while ( buff [ blanks ] == ' ' ) blanks++;
      blanksplus = blanks+OffSet;
      spt = buff+blanks;
      len = strlen ( spt ) ;
      while ( len > linelow ) {
          char ch;
          pt = spt;
          size = linelow;
          maxlen = strlen ( spt ) ;
          if ( maxlen < lineup ) {
              len = maxlen;
              break;
          }
          if ( maxlen < 2*linelow ) size = maxlen/2;
          i = 0;
          while ( i < size ) {
              ch = *pt;
              if ( ch == '\n' ) break;
              if ( ( ch == '"' ) || ( ch == '\'' ) ) {
                  i++;pt++;
                  while ( *pt != ch ) {
                      if ( *pt == '\\' ) {i++;pt++;};
                      i++;pt++;
                  }
              }
              i++;pt++;
          }
          while ( ( *pt != '(' ) && ( *pt != '{' ) && 
              ( *pt != '|' ) && ( *pt != ';' ) ) {
              ch = *pt;
              if ( ( ch == '"' ) || ( ch == '\'' ) ) {
                  pt++;
                  while ( *pt != ch ) {
                      if ( *pt == '\\' ) {pt++;};
                      pt++;
                  }
                  pt++;
              }
              if ( *pt == '\n' ) {
                  len = strlen ( spt ) ;
                  fpt = ( char * ) malloc ( len+1+blanks ) ;
                  for ( i = 0;i < blanks;i++ ) fpt [ i ] = ' ';
                  fpt [ blanks ] = '\0';
                  Strcat ( fpt , spt ) ;
                  Dadd ( S , fpt ) ;
                  return 1;
              }
              pt++;
          }
          if ( *pt == ';' ) pt++;
          if ( ( *pt == '|' ) && ( * ( pt-1 ) == '|' ) ) pt--;
          ch = * ( pt ) ;
          *pt = '\0';
          len = strlen ( spt ) ;
          fpt = ( char * ) malloc ( len+2+blanks ) ;
          for ( i = 0;i < blanks;i++ ) fpt [ i ] = ' ';
          fpt [ blanks ] = '\0';
          Strcat ( fpt , spt ) ;
          strcat ( fpt , "\n" ) ;
          Dadd ( S , fpt ) ;
          *pt = ch;
          spt = pt;
          len = strlen ( spt ) ;
          blanks = blanksplus;
      }
      fpt = ( char * ) malloc ( len+1+blanks ) ;
      for ( i = 0;i < blanks;i++ ) fpt [ i ] = ' ';
      fpt [ blanks ] = '\0';
      Strcat ( fpt , spt ) ;
      Dadd ( S , fpt ) ;
      return 1;
  }
  static Dlink *PreproPass2 ( void *tmp ) {
      char buff [ 5000 ] ;
      Dlink *S = NULL;
      Dlink *Stmp = ( Dlink * ) tmp;
      int i = 0;
      char *bf;
      char *pt , *dpt , *fpt;
      char ch;
      int nbrk = 0;
      int Shift = 1;
      char SBUFF [ 20 ] ;
      int Scond = 0;
      for ( i = 0;i < Shiftfac;i++ ) SBUFF [ i ] = ' ';
      SBUFF [ i ] = '\0';
      if ( Stmp == NULL ) return NULL;
      S = Dopen ( ) ;
      Resetlink ( Stmp ) ;
      i = 0;
      buff [ 0 ] = '\0';
      while ( ( bf = ( char * ) Getrecord ( Stmp ) ) != NULL ) {
          pt = bf;
          fpt = bf;
          while ( ( *fpt == ' ' ) || ( *fpt == '\t' ) ) fpt++;
          if ( *fpt == '#' ) {
//		  DADD(S,bf);
              fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( fpt , bf ) ;
              Dadd ( S , fpt ) ;
              while ( *pt != '\0' ) {
                  if ( *pt == '{' ) {nbrk++;Scond = 0;}
                  if ( *pt == '}' ) {nbrk--;Scond = 1;}
                  pt++;
              }
              ProcessNoClean ( Stmp , S ) ;
              continue;
          }
          if ( ( *fpt == '/' ) && ( * ( fpt+1 ) == '/' ) ) {
//		  DADD(S,bf);
              fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( fpt , bf ) ;
              Dadd ( S , fpt ) ;
              continue;
          }
          if ( ( *fpt == '/' ) && ( * ( fpt+1 ) == '*' ) ) {
              int end = 0;
//		  DADD(S,bf);
              fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( fpt , bf ) ;
              Dadd ( S , fpt ) ;
              while ( 1 ) {
                  while ( *fpt != '\n' ) {
                      if ( ( fpt [ 0 ] == '*' ) && ( fpt [ 1 ] == '/' ) ) {end = 1; break;}
                      fpt++;
                  }
                  if ( end ) break;
                  if ( ( bf = ( char * ) Getrecord ( Stmp ) ) != NULL ) {
                      fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
                      strcpy ( fpt , bf ) ;
                      Dadd ( S , fpt ) ;
                  }
                  else break;
                  fpt = bf;
              }
              continue;
          }
          while ( *pt != '\0' ) {
              if ( *pt == ' ' ) {pt++;buff [ i++ ] = ' ';}
              while ( *pt == ' ' ) {pt++;}
              ch = *pt;
              if ( *pt < ' ' ) {
                  if ( *pt == '\n' ) {
                      if ( i > 0 ) {
                          pt--;
                          while ( *pt == ' ' ) pt--;
                          if ( *pt != '\\' ) buff [ i++ ] = ' ';
                          buff [ i++ ] = '\n';
                          buff [ i++ ] = '\0';
                          if ( Scond ) Shift = nbrk+1;
                          dpt = ( char * ) malloc ( i+1+Shift*Shiftfac ) ;
                          SHIFT;
                          Strcat ( dpt , buff ) ;
                          DADD ( S , dpt ) ;
                          free ( dpt ) ;
                          Shift = nbrk+1;
                      }
                      pt++;
                      i = 0;
                      buff [ 0 ] = '\0';
                      continue;
                  }
                  buff [ i++ ] = ' ';
                  pt++;
                  continue;
              }
              if ( \
               ( *pt == '>' ) || ( *pt == '<' ) || \
               ( *pt == '=' ) || ( *pt == ',' ) || \
               ( *pt == '!' ) \
               ) {
                  char ch;
                  ch = buff [ i-1 ] ;
                  if ( ( ch != ' ' ) && ( ch != '*' ) && ( ch != '/' ) && 
                      ( ch != '-' ) && ( ch != '+' ) ) buff [ i++ ] = ' ';
                  if ( ( *pt == '=' ) || ( *pt == '<' ) || \
                   ( *pt == '>' ) || ( *pt == '!' ) ) {
                      ch = * ( pt+1 ) ;
                      buff [ i++ ] = *pt;
                      if ( ( ch == '=' ) ) {
                          buff [ i++ ] = ch;
                          pt++;
                      }
                  }
                  else buff [ i++ ] = *pt;
                  buff [ i++ ] = ' ';
                  pt++;
                  continue;
              }
              if ( ( *pt == '{' ) || ( *pt == '}' ) ) {
                  if ( *pt == '{' ) {nbrk++;Scond = 0;}
                  if ( *pt == '}' ) {nbrk--;Scond = 1;}
                  if ( nbrk < 0 ) {
                      fprintf ( stderr , "Brackets({,}) not matching; %s\n" , buff ) ;
                      exit ( -1 ) ;
                  }
                  buff [ i++ ] = *pt;
                  pt++;
                  continue;
              }
              if ( ( *pt == '|' ) || ( *pt == '&' ) ) {
                  if ( buff [ i-1 ] != ' ' ) buff [ i++ ] = ' ';
                  buff [ i++ ] = *pt;
                  char ch = * ( pt+1 ) ;
                  if ( ( ch == *pt ) ) {pt++;buff [ i++ ] = ch;}
                  if ( buff [ i-1 ] != ' ' ) buff [ i++ ] = ' ';
                  pt++;
                  continue;
              }
              if ( ( *pt == '[' ) || ( *pt == '(' ) ) {
                  if ( buff [ i-1 ] != ' ' ) buff [ i++ ] = ' ';
                  buff [ i++ ] = *pt;
                  buff [ i++ ] = ' ';
                  pt++;
                  continue;
              }
              if ( ( *pt == ']' ) || ( *pt == ')' ) ) {
                  if ( buff [ i-1 ] != ' ' ) buff [ i++ ] = ' ';
                  buff [ i++ ] = *pt;
                  buff [ i++ ] = ' ';
                  pt++;
                  continue;
              }
#if 0
              if ( ( *pt == '+' ) || ( *pt == '-' ) ) {
                  char ch , chp;
                  int j = i-1;
                  int ok = 0;
                  chp = *pt;
                  while ( buff [ j ] == ' ' ) j--;
                  ch = * ( pt+1 ) ;
                  if ( ( ch == chp ) || ( chp == '=' ) ) {
                      buff [ i++ ] = ch;
                      buff [ i++ ] = chp;
                      buff [ i++ ] = ' ';
                      pt++;
                  }
                  else {
                      ok = isalnum ( buff [ j ] ) || ( buff [ j ] == '.' ) ;
                      if ( isdigit ( * ( pt+1 ) ) && ( ! ok ) ) {buff [ i++ ] = *pt;}
                      else {
                          if ( buff [ i-1 ] != ' ' ) buff [ i++ ] = ' ';
                          buff [ i++ ] = *pt;
                          buff [ i++ ] = ' ';
                      }
                  }
                  pt++;
                  continue;
              }
#endif
              ch = *pt;
              if ( ( ch == '"' ) || ( ch == '\'' ) ) {
                  char next;
                  buff [ i++ ] = ch;
                  pt++;
                  while ( *pt != ch ) {
                      if ( *pt == '\\' ) {
                          buff [ i++ ] = *pt++;
                          buff [ i++ ] = *pt++;
                          continue;
                      }
                      buff [ i++ ] = *pt++;
                      if ( ( *pt != '\t' ) && ( *pt < ' ' ) ) {
                          fprintf ( stderr , "Unclosed String: %s\n" , bf ) ;
                          exit ( -1 ) ;
                      }
                  }
                  buff [ i++ ] = ch;
                  pt++;
                  continue;
              }
              if ( *pt == ' ' ) {pt++;continue;}
              buff [ i++ ] = *pt;
              pt++;
          }
      }
      Dempty ( Stmp ) ;
//        Dwritefile (  S ,  "JUNK1" )  ;
      if ( nbrk != 0 ) {
          fprintf ( stderr , "Brackets({,}) not matching;\n" ) ;
          exit ( -1 ) ;
      }
      return S;;
  }
  Dlink *prepro ( char *flname ) {
      char buff [ 50000 ] ;
      Dlink *S = Dreadfile ( flname ) ;
      char *bf , *pt , *fpt;
      char *sptr;
      int End = 0;
      Dlink *P = Dopen ( ) ;
      buff [ 0 ] = '\0';
      while ( ( bf = ( char * ) Getrecord ( S ) ) != NULL ) {
          pt = bf;
//	   printf("%s",bf);
          while ( ( *pt == ' ' ) || ( *pt == '\t' ) ) {
              pt++;
          }
          if ( *pt < ' ' ) continue;
          if ( *pt == '#' ) {
              fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( fpt , bf ) ;
              Dadd ( P , fpt ) ;
              ProcessNoClean ( S , P ) ;
              continue;
          }
          if ( ( *pt == '/' ) && ( * ( pt+1 ) == '/' ) ) {
              fpt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( fpt , bf ) ;
              Dadd ( P , fpt ) ;
              continue;
          }
          if ( ( *pt == '/' ) && ( * ( pt+1 ) == '*' ) ) {
              int end = 0;
              pt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
              strcpy ( pt , bf ) ;
              Dadd ( P , pt ) ;
              pt = bf;
              while ( 1 ) {
                  while ( *pt != '\n' ) {
                      if ( ( pt [ 0 ] == '*' ) && ( pt [ 1 ] == '/' ) ) {end = 1; break;}
                      pt++;
                  }
                  if ( end ) break;
                  if ( ( bf = ( char * ) Getrecord ( S ) ) != NULL ) {
                      pt = ( char * ) malloc ( strlen ( bf ) +1 ) ;
                      strcpy ( pt , bf ) ;
                      Dadd ( P , pt ) ;
                  }
                  else break;
                  pt = bf;
              }
              continue;
          }
          sptr = pt;
//         while ( ( *pt >= ' ' ) || ( *pt == '\t' ) ) {
          End = 0;
          while ( ( *pt != '\n' ) && ( *pt != '\0' ) ) {
//             if( (*pt< ' ') && (*pt != '\t')) *pt=' ';
              if ( ( *pt == '/' ) && ( * ( pt+1 ) == '/' ) ) {
#if 0
                  pt--;
                  while ( *pt == ' ' ) pt--;
                  pt++;
                  * ( pt ) = '\0';
#else
                  End = 1;
                  break;
#endif
              }
              pt++;
          }
          if ( End ) {
              while ( *pt != '\n' ) pt++;
              *pt = '\0';
              pt = ( char * ) malloc ( strlen ( bf ) +1+4 ) ;
              strcpy ( pt , bf ) ;
              strcat ( pt , ":::\n" ) ;
              Dadd ( P , pt ) ;
              buff [ 0 ] = '\0';
              continue;
          }
          pt--;
          while ( *pt == ' ' ) pt--;
          if ( ( *pt == ';' ) || ( *pt == ':' ) || ( *pt == '}' ) 
              || ( *pt == '{' ) || ( *pt == '\\' ) ) {
              pt++;
              *pt = '\n'; * ( pt+1 ) = '\0';
              Strcat ( buff , sptr ) ;
              fpt = ( char * ) malloc ( strlen ( buff ) +1 ) ;
              strcpy ( fpt , buff ) ;
              Dadd ( P , fpt ) ;
              buff [ 0 ] = '\0';
          }
          else {
		 //Checking Comment end
              if ( ( * ( pt-1 ) == '*' ) && ( *pt == '/' ) ) {
                  pt++;
                  *pt = '\n'; * ( pt+1 ) = '\0';
                  Strcat ( buff , sptr ) ;
                  fpt = ( char * ) malloc ( strlen ( buff ) +1 ) ;
                  strcpy ( fpt , buff ) ;
                  Dadd ( P , fpt ) ;
                  buff [ 0 ] = '\0';
              }
              else {
                  pt++;
                  *pt = ' ';
                  pt++;
                  *pt = '\0';
                  Strcat ( buff , sptr ) ;
              }
          }
      }
      Dempty ( S ) ;
//       Dwritefile (  P ,  "JUNK0" )  ;
      return PreproPass2 ( P ) ;
  }
#define D_NOCLEANCC
  void Usage ( void *pt ) {
      char **Name = (char **)(pt);
      fprintf ( stderr , "Usage: "
		        "%s <-i indent offset> "
		        " <-s start  offset> "
		        " <-o outfile> "
		        " [inputfile] \n" ,*Name) ;
       fprintf (stderr, "cleancc is a C code cleaner with proper indentation.\n"
                        "Try it with a backup of the code .\n"
                        " Options can be seen with '-h/--help'\n"
                        " Incode declarions are thre to skip cleaning portions by\n"
                        " #define D_NOCLEANCC and\n"
                        " #define D_CLEANCC\n"
                        " by default D_CLEANCC is defined\n");
          
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
