#include <string.h>
#include <stdio.h>
#include "book_management.h"
#include "user.h"
#include "loan.h"

const int INIT = 0X3F3F3F;
User login_user = {"", "", "", -1}; //代表当前登录的用户,初始化-1代表尚未登录
char *userfile = "users.txt";
char *bookfile = "books.txt";
char *loanfile = "loans.txt";


//---------------------------------------------------------aux code-------------------------

//检查是否有用户登录该系统，如果有的话就返回用户的权限(user, librarian)，没有的话返回-1
int check_login() {
    if (login_user.is_manager == -1) {  //尚未登陆
        printf("please login into the system.\n");
        return -1;
    }
    return login_user.is_manager;
}


static char *ask_question(const char *question) {

	printf("%s",question);

	const int size_step = 2;
	char *answer = malloc(size_step);
	answer[0] = 0; //now it's a string of length 0.

	//read until a new line in blocks of size_step  bytes at at time
	char *next_chunk = answer;
	int iteration = 0;
	do {
		answer = realloc(answer, size_step + iteration*size_step);
		next_chunk = answer + strlen(answer); //answer may have moved.
		fgets(next_chunk, size_step, stdin);

		next_chunk = answer + strlen(answer); //take the new read into account
		++iteration;
	} while (* (next_chunk-1) != '\n');

	*(next_chunk-1) = 0; //truncate the string eliminating the new line.

	return answer;
}


//-----------------------------------------------------kernel code-----------------------------------------
//用户注册
void case_register() {
    FILE *file = fopen(userfile, "r");
    load_users(file);   //从文件中加载用户数据
    fclose(file);
    char *name, *password;
    char *username = ask_question("please input username:");

    //检查该username是否被注册过
    if (have_user(username)) {
        printf("this username has been registered, please change one.\n");
        free(username);
        clean_user_array(user_array);
        return;
    }

    //尚未注册过，go on
    name        = ask_question("please input your name:");
    password    = ask_question("please input your password:");
    User *user  = get_new_user();

    user->is_manager    = 0;
    user->name          = name;
    user->password      = password;
    user->username      = username;
    add_user(*user);

    //store this user
    file = fopen(userfile, "w");
    store_users(file);
    fclose(file);

    printf("%s, welcome to our library!\n", name);

    //free the memory
    clean_user_array(user_array);
    free(name);
    free(username);
    free(password);
}

//用户登录
void case_login() {
    FILE *file = fopen(userfile, "r");
    load_users(file);
    print_user_array(user_array);
    fclose(file);

    int cnt = 3;

    char *username = ask_question("please input the username:");
    char *password = NULL;

    //Didn't find this user
    if (!have_user(username)) {
        printf("username %s not exits. Please check the username.\n", username);
        clean_user_array(user_array);
        free(username);
        return;
    }

    UserArray log_user = find_user_by_username(username);

    //输错三次密码就退出
    do {
        printf("please input the password, you have %d times.", cnt);
        free(password);
        password = ask_question("");
        if (!strcmp(password, log_user.array[0].password)) break;
        else printf("wrong password.\n");
    } while(--cnt);

    if (!cnt) {
        printf("you have no more chance.\n");
        clean_user_array(user_array);
        free(username);
        free(password);
        return;
    }

    //登录
    login_user = log_user.array[0];
    printf("hello, %s.\n", log_user.array[0].username);
}

//查询书
BookArray case_find_book() {
    FILE *file = fopen(bookfile, "r");
    load_books(file);
    fclose(file);

    int i;
    char *ch = ask_question("please input the way you want to search, 1)title 2)author 3)year:");
    int number = atoi(ch);
    free(ch);
    BookArray ba = {NULL, 0};
    //让用户选择查询的方式(title, author, year)
    if (number == 1) {
        ch = ask_question("please input the title:");
        ba = find_book_by_title(ch);
    } else if (number == 2) {
        ch = ask_question("please input the author:");
        ba = find_book_by_author(ch);
    } else if (number == 3) {
        ch = ask_question("please input the year:");
        ba = find_book_by_year(atoi(ch));
    }

    //清理空间，防止内存泄露
    free(ch);

    return ba;
}

//借书
void case_borrow_book() {
    FILE *file = fopen(loanfile, "r");
    load_loans(file);
    fclose(file);

    BookArray ba = case_find_book();
    if (ba.length == 0) {   //There is no this book in the library
        printf("Didn't find this book.\n");
        return;
    }
    int number = INIT;
    int cnt = 3;
    do {
        if (number != INIT) {
            printf("Number is wrong.\n");
        }
        printf("Please input the number of book that you want to borrow:\n");
        print_book_array(ba);
        char *ch_num = ask_question("");
        number = atoi(ch_num);
    } while( (number > ba.length || number < 0) && cnt--);
    if (cnt == -1) {
        printf("You must be joking...");   //这是输入借阅的书号三次错误了，不知道该说什么，自己编一个吧。可以把printf里面的内容改一下
        return;
    }
    number -=1;

    //The user has borrowed this book.
    if (have_loan(login_user.username, ba.array[number])) {
        printf("You have borrowed this book.\n");
        return ;
    }

    //No copy
    int ret = borrow_book(ba.array[number]);
    if (ret == 1) {
        printf("The copy of this book is zero.\n");
        return;
    }

    //borrow
    Loan *loan = new_loan(login_user.username, ba.array[number]);
    add_loan(*loan);
    printf("\nBook:\nid:%d\ttitle:%s\tauthor:%s\tyear:%d\nBorrow successfully!\n\n", ba.array[number].id, ba.array[number].title, ba.array[number].authors, ba.array[number].year);

    //store the loans and books to files.
    file = fopen(loanfile, "w");
    store_loans(file);
    fclose(file);
    file = fopen(bookfile, "w");
    store_books(file);
    fclose(file);
}

//return book
void case_return_book() {
    FILE *file = fopen(bookfile, "r");
    load_books(file);
    fclose(file);
    file = fopen(loanfile, "r");
    load_loans(file);
    fclose(file);

    LoanArray la = find_loan_by_username(login_user.username);
    // The user hasn't borrowed any book.
    if (la.length == 0) {
        printf("There is no book need return.\n");
        return;
    }

    print_loan_array(la);
    char *num =  ask_question("Please input the number you want to return. input -1 to exit:");
    int number = atoi(num);
    if (number == -1) return;  //exit
    if (number <= 0 || number > la.length) {
        printf("invalid number.\n");
        return;
    }
    number -= 1;

    // return book
    Book tmp_book = loan_to_book(la.array[number]);
    return_book(tmp_book);
    remove_Loan(la.array[number]);

    // store to files
    file = fopen(bookfile, "w");
    store_books(file);
    fclose(file);
    file = fopen(loanfile, "w");
    store_loans(file);
    fclose(file);
}

//Add book to library
//Only librarian can do this
void case_add_book() {
    FILE *file = fopen(bookfile, "r");
    load_books(file);
    fclose(file);
    Book book;
    book.id = atoi(ask_question("Please input the id:"));
    book.title = ask_question("Please input the title:");
    book.authors = ask_question("Please input the authors:");
    book.year = atoi(ask_question("Please input the year:"));
    book.copies = atoi(ask_question("Please input the number of books:"));
    book.borrowed = 0;
    if (!add_book(book)) printf("Add book successfully.\n");
    file = fopen(bookfile, "w");
    store_books(file);
    fclose(file);
}

//Remove book from librarian
//Only librarian can do this
void case_remove_book() {
    FILE *file = fopen(bookfile, "r");
    load_books(file);
    fclose(file);
    Book book;
    book.id = atoi(ask_question("Please input the id:"));
    book.title = ask_question("Please input the title:");
    book.authors = ask_question("Please input the authors:");
    book.year = atoi(ask_question("Please input the year:"));
    int number = remove_book(book);
    if (number == 0) printf("Remove book successfully.\n");
    else if (number ==  1) printf("The book has been borrowed.\n");
    else if (number == 2) printf("Didn't find this book.\n");
    file = fopen(bookfile, "w");
    store_books(file);
    fclose(file);
}

static void main_menu() {
    int choice = 5;
    char *answer;
    do {
        answer = ask_question("\nPlease enter next action:\n1) register\n2) login\n3) search for books\n4) borrow a book\n5) return a book\n6) add a book\n7)remove a book\n8) exit\nchoice: ");
        choice = atoi(answer);
        switch(choice) {
        case 1:
            case_register();
            break;
        case 2:
            case_login();
            break;
        case 3:
            if (login_user.is_manager == -1) printf("Please login.\n"); //先检查一下是否login
            else {
                BookArray ba = case_find_book();
                if (ba.length == 0) printf("Did't find this book!\n");
                else print_book_array(ba);
                clean_book_array(ba);
            }
            break;
        case 4:
            if (login_user.is_manager == -1) printf("Please login.\n");
            else case_borrow_book();
            break;
        case 5:
            if (login_user.is_manager == -1) printf("Please login.\n");
            else case_return_book();
            break;
        case 6:
            if (login_user.is_manager != 1) printf("Please login as librarian.\n");
            else case_add_book();
            break;
        case 7:
            if (login_user.is_manager != 1) printf("Please login as librarian.\n");
            else case_remove_book();
            break;
        }
    } while(choice != 8);
}

void run_interface() {
    main_menu();
}
