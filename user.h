#ifndef __USER__H
#define __USER__H
#include <stdio.h>

typedef struct _User {
    char *name;
    char *username;
    char *password;
    int is_manager;
    //_User *nxt;
}User, *User_p;

typedef struct _UserArray {
	 User* array;
	 unsigned int length;
}UserArray;

static UserArray user_array;
static int user_array_size = 0;


int add_user(User user);

int store_users(FILE *file);

int load_users(FILE *file);

int have_user(const char *username);

UserArray find_user_by_username(const char *username);

int clean_user_array(UserArray ua);
UserArray find_user_by_username(const char *username) ;
void print_user_array(UserArray user_array);

#endif
