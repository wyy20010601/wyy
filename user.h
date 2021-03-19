#ifndef __USER__H
#define __USER__H


typedef struct _User {
    char *name;
    char *username;
    char *password;
    _User *nxt;
}User, *User_p;

typedef struct _UserArray {
	 User* array;
	 unsigned int length;
}UserArray;

//user register
const char *user_file = "user_message.dat";
struct User *user_register(struct User u);
struct User *login(char *username, *password);

int store_users(File *file);

int load_users(File *file);

int user_register(User user);

UserArray find_user_by_username(const char *username);

#endif
