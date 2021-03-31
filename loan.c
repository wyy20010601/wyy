#include "loan.h"
#include "book_management.h"
#include <stdio.h>
static LoanArray loan_array; //存放所有的loan
static int loan_array_size = 0; //代表现在已经为book申请的内存数量

//----------------------------------------------aux code------------------------------


//把一个loan添加到LoanArray中去
//如果成功就return 0
int get_loanArray(LoanArray *la, Loan b) {
    if (la->length == 0) {
        if ((la->array = (Loan*)malloc(sizeof(Loan))) == NULL) {
            printf("malloc error\n");
            exit(0);
        }
    }
    else if (((*la).array = realloc((*la).array, ((*la).length + 1) * sizeof(Loan))) == NULL) {
        printf("malloc error!\n");
        exit(0);
    }

    char *ch_username = (char *)malloc(sizeof(b.username));
    char *ch_author = (char *)malloc(sizeof(b.authors));
    char *ch_title  = (char *)malloc(sizeof(b.title));
    strcpy(ch_username, b.username);
    strcpy(ch_author, b.authors);
    strcpy(ch_title, b.title);
    la->array[la->length].username  = ch_username;
    la->array[la->length].authors   = ch_author;
    la->array[la->length].id        = b.id;
    la->array[la->length].title     = ch_title;
    la->array[la->length].year      = b.year;
    la->length += 1;
    return 0;
}

//输出LoanArray中的全部元素
void print_loan_array(LoanArray la) {
    int len = la.length;
    int i;
    for (i = 0; i < len; ++i) {
        printf("number:%d\tusername:%s\tbook id:%d\tbook title:%s\tbook authors:%s\tbook year:%d\n", i + 1, la.array[i].username, la.array[i].id, la.array[i].title, la.array[i].authors, la.array[i].year);
    }
}

//new a loan by username and book
Loan* new_loan(const char *username, Book book) {
    char *ch_username = get_string_copy(username);
    char *ch_title = get_string_copy(book.title);
    char *ch_author = get_string_copy(book.authors);
    Loan *res = (Loan*)malloc(sizeof(Loan));
    res->username = ch_username;
    res->title = ch_title;
    res->authors = ch_author;
    res->id = book.id;
    res->year = book.year;
    return res;
}

//free memory
void clean_loan(Loan *loan) {
    free(loan->authors);
    free(loan->title);
    free(loan->username);
    loan->id = 0;
    loan->year = 0;
}

//----------------------------------------------kernel code--------------------------
//将loan存放到database
//成功返回0
int store_loans(FILE *file) {
    int len = loan_array.length;
    int i;
    Loan loan;
    fprintf(file, "%d\n", len);

    for (i = 0; i < len; ++i) {
        fprintf(file, "%s %d %s %s %d\n", loan_array.array[i].username, loan_array.array[i].id, loan_array.array[i].title, loan_array.array[i].authors, loan_array.array[i].year);
    }
    return 0;
}

//从database获取loan
//成功返回0，存在故障就打印故障并退出
int load_loans(FILE *file) {
    Loan loan;
    int i;

    //获取有多少loan（借阅的书）
    int cnt = 0;
    fscanf(file, "%d", &cnt);

    loan_array_size = (cnt + 9) / 10 * 10;  //申请的数组长度，会多申请一部分
    if ((loan_array.array = (Loan*)malloc(sizeof(Loan) * loan_array_size)) == NULL) {
        printf("malloc error!\n");
        exit(0);
    }
    loan_array.length = cnt;
    //加载...load from file
    for (i = 0; i < cnt; ++i) {
        fgetc(file);
        loan_array.array[i].username = input_string(file);
        fscanf(file, "%d", &loan_array.array[i].id);
        loan_array.array[i].title = input_string(file);
        loan_array.array[i].authors = input_string(file);
        fscanf(file, "%d", &loan_array.array[i].year);
    }
    return 0;
}

//读者借书，需要add a loan
//将一个loan添加到loan_array中
int add_loan(Loan loan) {
    const int loan_array_step = 10;

    if (loan_array.length + 1 > loan_array_size) {
        if ((loan_array.array = realloc(loan_array.array, (loan_array_size + loan_array_step) * sizeof(Loan))) == NULL) {
            printf("malloc error!\n");
            exit(0);
        }
        loan_array_size += loan_array_step;
    }

    printf("\nfunction add_loan. print loan\n");
    printf("%s %d %s %s %d\n\n", loan.username, loan.id, loan.title, loan.authors, loan.year);
    loan_array.array[loan_array.length++] = loan;

    return 0;
}

//读者还书，需要将loan移除
//return 0代表成功删除了，1代表未找到书
int remove_Loan(Loan loan) {
    int i, j;
    for (i = 0; i < loan_array.length; ++i) {
        if (!strcmp(loan_array.array[i].username, loan.username) && !strcmp(loan_array.array[i].authors, loan.authors) && loan_array.array[i].year== loan.year&& \
            !strcmp(loan_array.array[i].title, loan.title)) {
            clean_loan(loan_array.array + i);
            for (j = i; j < loan_array.length - 1; ++j) {
                loan_array.array[j] = loan_array.array[j + 1];
            }
            loan_array.length--;
            return 0;
        }
    }
    return 1;
}

//通过username寻找读者借过的书
//LoanArray中的loan地址是新malloc的
//如果读者未借过书,LoanArray中的length为0
LoanArray find_loan_by_username(const char *username) {
    LoanArray *la;
    if ((la = (LoanArray*)malloc(sizeof(LoanArray))) == NULL) {
        printf("malloc error.\n");
        exit(0);
    }
    //初始化
    la->length = 0;
    la->array = NULL;

    int i;
    for (i = 0; i < loan_array.length; ++i) {

        if (!strcmp(loan_array.array[i].username, username)) {    //比较title，如果有相同的就放加入ba
            get_loanArray(la, loan_array.array[i]);
        }
    }

    return *la;
}

//判断读者是否在借某本书
//在借返回1,未借返回0
int have_loan(const char* username, Book book) {
    int len = loan_array.length;
    int i;
    for (i = 0; i < len; ++i) {
        if (!strcmp(loan_array.array[i].username, username) && book.id == loan_array.array[i].id) {
            return 1;
        }
    }
    return 0;
}

