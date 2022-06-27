# ifndef __GETOPT_H_
# define __GETOPT_H_

# include "auto_tchar.h"
// Standard GNU options
# define null_argument           0 /*Argument Null*/
# define no_argument             0 /*Argument Switch Only*/
# define required_argument       1 /*Argument Required*/
# define optional_argument       2 /*Argument Optional*/
// Shorter Versions of options
# define ARG_NULL 0 /*Argument Null*/
# define ARG_NONE 0 /*Argument Switch Only*/
# define ARG_REQ  1 /*Argument Required*/
# define ARG_OPT  2 /*Argument Optional*/

extern TCHAR* optarg;
extern int    optind;
extern int    opterr;
extern int    optopt;
struct option
{
    
    TCHAR* name;
    int has_arg;
    int* flag;
    TCHAR val;
};

extern int getopt(int argc, TCHAR* const* argv, const TCHAR* optstring);

extern int getopt_long
(int ___argc, TCHAR* const* ___argv
    , const TCHAR* __shortopts
    , const struct option* __longopts
    , int* __longind);

extern int getopt_long_only
(int ___argc, TCHAR* const* ___argv
    , const TCHAR* __shortopts
    , const struct option* __longopts
    , int* __longind);

extern void getopt_reset();

# endif  // __GETOPT_H_