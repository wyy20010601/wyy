#ifndef __LOAN__H__
#define __LOAN__H__

#include "book_management.h"

typedef struct _Loan
{
    char *username;
    char *title;
    char *authors;
    int id;
    int year;
} Loan;

typedef struct _LoanArray
{
    Loan *array;
    unsigned int length;
} LoanArray;

LoanArray find_loan_by_username(const char *);


#endif // __LOAN__H__

