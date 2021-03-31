#include "book_management.h"
#include "loan.h"
#include <stdio.h>
#include <string.h>
static BookArray book_array;
static int book_array_size = 0; //代表现在已经为book申请的内存数量

//------------------------------------------------------aux code------------------------------------
//input string from file
char *input_string(FILE* file){
    char *str;
    int ch;
    int len = 0;
    int size = 16;
    if ((str = realloc(NULL, sizeof(char) * size)) == NULL) {
        printf("malloc error!\n");
        exit(0);
    }

    ch = fgetc(file);
    if (ch != ' ') {
        str[len++] = ch;
    }
    while(EOF != (ch = fgetc(file)) && ch != ' ' && ch != '\n') {
        str[len++] = ch;
        if(len == size) {
            if ((str = realloc(str, sizeof(char) * (size += 16))) == NULL) {
                printf("malloc error!\n");
                exit(0);
            }
        }
    }

    str[len++] = '\0';

    return realloc(str, sizeof(char) * len);
}

//copy a string with different address
char *get_string_copy(char *ch) {
    char *res = (char *)malloc(sizeof(ch));
    strcpy(res, ch);
    return res;
}

//copy a book with different address
Book* copy_book(Book book) {
    Book *res = (Book*)malloc(sizeof(Book));
    res->authors = get_string_copy(book.authors);
    res->title = get_string_copy(book.title);
    res->borrowed = book.borrowed;
    res->copies = book.copies;
    res->borrowed = book.borrowed;
    res->id = book.id;
    return res;
}

//free memory
void clean_book(Book *p) {
    free(p->authors);
    free(p->title);
    p->borrowed = 0;
    p->copies = 0;
    p->id = 0;
    p->year = 0;
}

void print_book_array(BookArray ba) {
    int len = ba.length;
    int i;
    for (i = 0; i < len; ++i) {
        printf("%d: id:%d\ttitle:%s\tauthors:%s\tborrowed:%d\tcopies:%d\tyear:%d\n", i + 1, ba.array[i].id, ba.array[i].title, ba.array[i].authors, ba.array[i].borrowed, ba.array[i].copies, ba.array[i].year);
    }
}


void print() {
    int i;
    printf("bool_array.length:%d\n", book_array.length);
    for (i = 0; i < book_array.length; ++i) {
        printf("%s %s %d %d %d %d\n", book_array.array[i].title, book_array.array[i].authors, book_array.array[i].borrowed, book_array.array[i].copies, book_array.array[i].id, book_array.array[i].year);
    }
}

//malloc for book
Book get_new_book() {
    Book *res;
    if (!(res = (Book*)malloc(sizeof(Book)))) {
        printf("malloc error\n");
        exit(0);
    }
    return *res;
}

//把一个book添加到某个BookArray中去
int get_bookArray(BookArray *ba, Book b) {
    if (ba->length == 0) {
        if ((ba->array = (Book*)malloc(sizeof(Book))) == NULL) {
            printf("malloc error\n");
            exit(0);
        }
    }
    else if (((*ba).array = realloc((*ba).array, ((*ba).length + 1) * sizeof(Book))) == NULL) {
        printf("malloc error!\n");
        exit(0);
    }

    char *ch_author = (char *)malloc(sizeof(b.authors));
    char *ch_title  = (char *)malloc(sizeof(b.title));
    strcpy(ch_author, b.authors);
    strcpy(ch_title, b.title);
    ba->array[ba->length].authors   = ch_author;
    ba->array[ba->length].borrowed  = b.borrowed;
    ba->array[ba->length].copies    = b.copies;
    ba->array[ba->length].id        = b.id;
    ba->array[ba->length].title     = ch_title;
    ba->array[ba->length].year      = b.year;
    ba->length += 1;

    return 0;
}

//free memory
void clean_book_array(BookArray ba) {
    int i;
    int len = ba.length;
    for (i = 0; i < len; ++i) {
        clean_book(ba.array + i);
    }
    ba.array = NULL;
    ba.length = 0;
}

void init_book_array() {
    clean_book_array(book_array);
    book_array_size = 0;
}

//judge if two books are the same book
int book_equal(Book book1, Book book2) {
    return !strcmp(book1.authors, book2.authors) && !strcmp(book1.title, book2.title) && \
            book1.id == book2.id && book1.year == book2.year;
}

//get the information of loan, and return a book with the same information
Book loan_to_book(Loan loan) {
    Book book = get_new_book();
    book.authors = malloc(sizeof(loan.authors));
    book.title = malloc(sizeof(loan.title));
    strcpy(book.authors, loan.authors);
    strcpy(book.title, loan.title);
    book.id = loan.id;
    book.year = loan.year;
    book.copies = 1;
    book.borrowed = 0;
    return book;
}

//--------------------------------------------------------kernel code---------------------------------


int store_books(FILE *file) {
    int len = book_array.length;
    int i;
    Book book;
    fprintf(file, "%d\n", len);
    for (i = 0; i < len; ++i) {
        fprintf(file, "%d %s %s %d %d %d\n", book_array.array[i].id, book_array.array[i].title, book_array.\
                array[i].authors, book_array.array[i].year, book_array.array[i].copies, book_array.array[i].borrowed);
    }
    return 0;
}


int load_books(FILE *file) {
    Book book;
    int i;
    //获取有多少种书
    int cnt = 0;
    fscanf(file, "%d", &cnt);
    book_array_size = (cnt + 9) / 10 * 10;  //申请的数组长度，会多申请一部分，比如51本书会申请60本书的内存；为了方便后面加书
    if ((book_array.array = (Book*)malloc(sizeof(Book) * book_array_size)) == NULL) {
        printf("malloc error!\n");
        exit(0);
    }
    //将书从file中加载出来
    book_array.length = cnt;
    for (i = 0; i < cnt; ++i) {
        fscanf(file, "%d", &book_array.array[i].id);
        book_array.array[i].title = input_string(file);
        book_array.array[i].authors = input_string(file);
        fscanf(file, "%d", &book_array.array[i].year);
        fscanf(file, "%d", &book_array.array[i].copies);
        fscanf(file, "%d", &book_array.array[i].borrowed);
    }
    return 0;
}

int add_book(Book book) {
    const int book_array_step = 10;

    if (book_array.length + 1 > book_array_size) {
        if ((book_array.array = realloc(book_array.array, book_array_size + book_array_step * sizeof(Book))) == NULL) {
            printf("malloc error!\n");
            exit(0);
        }
        book_array_size += book_array_step;
    }
    int i;
    for (i = 0; i < book_array.length; ++i) {
        if (book_equal(book_array.array[i], book)) {    //如果已存在该书了，将copy数目添加到原book中
            book_array.array[i].copies += book.copies;
        }
    }
    if (i == book_array.length) //haven't this book, add it to book array.
        book_array.array[book_array.length++] = book;

    return 0;
}

int return_book(Book book) {
    int i;
    for (i = 0; i < book_array.length; ++i) {
        if (book_equal(book_array.array[i], book)) {
            book_array.array[i].borrowed -= 1;
            book_array.array[i].copies += 1;
        }
    }
    return 0;
}

//return 0代表成功删除了，1代表书被借走了，2代表未找到书
int remove_book(Book book) {
    int i, j;
    for (i = 0; i < book_array.length; ++i) {
        if (book_equal(book_array.array[i], book)) {
                if (book_array.array[i].borrowed) {
                    return 1;
                } else {
                    clean_book(book_array.array + i);
                    for (j = i; j < book_array.length - 1; ++j) {
                        book_array.array[j] = book_array.array[j + 1];
                    }
                    book_array.length--;
                    return 0;
                }
            }
    }
    return 2;
}

//借出去一本书
//0代表借阅成功，1代表书已经全部借出
int borrow_book(Book book) {
    int i, j;
    for (i = 0; i < book_array.length; ++i) {
        if (book_equal(book, book_array.array[i])) {
            if (book_array.array[i].copies == 0) return 1;
            book_array.array[i].copies -= 1;
            book_array.array[i].borrowed += 1;
            return 0;
        }
    }
}


BookArray find_book_by_title (const char *title) {
    int i;
//    BookArray ba = {NULL, 0};//初始化一个结构体，充当头指针
    BookArray *ba = (BookArray*)malloc(sizeof(BookArray));
    ba->array = NULL;
    ba->length = 0;
    for (i = 0; i < book_array.length; ++i) {
        if (!strcmp(book_array.array[i].title, title)) {    //比较title，如果有相同的就放加入ba
            Book tmp_book = book_array.array[i];
            get_bookArray(ba, tmp_book);
        }
    }
    return *ba;
}

BookArray find_book_by_author (const char *author) {
    int i;
    BookArray ba = {NULL, 0};//初始化一个结构体，充当头指针
    for (i = 0; i < book_array.length; ++i) {
        if (!strcmp(book_array.array[i].authors, author)) {    //比较title，如果有相同的就放加入ba
            get_bookArray(&ba, book_array.array[i]);
        }
    }
    return ba;
}

BookArray find_book_by_year (unsigned int year) {
    int i;
    BookArray ba = {NULL, 0};//初始化一个结构体，充当头指针
    for (i = 0; i < book_array.length; ++i) {
        if (book_array.array[i].year == year) {
            get_bookArray(&ba, book_array.array[i]);
        }
    }
    return ba;
}



