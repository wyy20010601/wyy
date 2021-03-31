#include "user.h"
#include "book_management.h"
#define __SUCCESS 0
#define __DEFEAT  1


//-------------------------------------------aux code------------------
//将用户添加到UserArray中
void make_user(UserArray *ua, char *ch_name, char *ch_username, char *ch_password, int is_manager) {
    ua->array[ua->length].name = ch_name;
    ua->array[ua->length].username = ch_username;
    ua->array[ua->length].password = ch_password;
    ua->array[ua->length++].is_manager = is_manager;
}

////将用户添加到UserArray中
int get_userArray(UserArray *ua, User u) {
    if (ua->length == 0) {
        if ((ua->array = (User*)malloc(sizeof(User))) == NULL) {
            printf("malloc error\n");
            exit(0);
        }
    } else if (((*ua).array = realloc((*ua).array, ((*ua).length + 1) * sizeof(User))) == NULL) {
        printf("malloc error\n");
        exit(0);
    }

    char *ch_name = (char *)malloc(size
	of(u.name));
    char *ch_username = (char *)malloc(sizeof(u.username));
    char *ch_password = (char *)malloc(sizeof(u.password));
    strcpy(ch_name, u.name);
    strcpy(ch_username, u.username);
    strcpy(ch_password, u.password);
    make_user(ua, ch_name, ch_username, ch_password, u.is_manager);
    return 0;
}

//free memory
int clean_user(User *user) {
    free(user->name);
    free(user->password);
    free(user->username);
    user->is_manager = 0;
    return 0;
}

//free memory
int clean_user_array(UserArray ua) {
    int len = ua.length;
    int i = 0;

    for (i = 0; i < len; ++i) {
        if (!clean_user(ua.array + i));
    }
    free(ua.array);
    ua.length   = 0;
    return 0;
}

int init_user_array() {
    clean_user_array(user_array);
    user_array_size     = 0;
    return 0;
}

//malloc a user
User *get_new_user() {
    User* user = NULL;
    if (!(user = (User*)malloc(sizeof(User)))) {
        printf("malloc error\n");
        exit(0);
    }

    return user;
}

void print_user_array(UserArray user_array) {
    int len = user_array.length;
    int i;
    for (i = 0; i < len; ++i) {
        printf("%s %s %s %d\n", user_array.array[i].name, user_array.array[i].username, user_array.array[i].password, user_array.array[i].is_manager);
    }
}


//--------------------------------------------kerkel code-----------------------
//添加一个user
//成功返回0,失败打印失败原因并退出
int add_user(User user) {
	const int user_array_step = 10;

	if (user_array.length + 1 > user_array_size) {
        if ((user_array.array = realloc(user_array.array, user_array_size + user_array_step * sizeof(User))) == NULL) {
            printf("malloc error!\n");
            exit(0);
        }
        user_array_size += user_array_step;
	}

	user_array.array[user_array.length++] = user;

	return 0;
}


//向database中写入用户
//成功返回0,失败打印失败原因并退出
int store_users(FILE *file) {
    int len = user_array.length;
    int i;
    User user;

    fprintf(file, "%d\n", len);
    for (i = 0; i < len; ++i) {
        fprintf(file, "%d %s %s %s\n", user_array.array[i].is_manager, user_array.array[i].name, user_array.array[i].username, user_array.array[i].password);
    }

    return 0;
}

//从database中读取用户
//成功返回0,失败打印失败原因并退出
int load_users(FILE *file) {
    User user;
    int i;
    int cnt = 0;
    fscanf(file, "%d", &cnt);
    user_array_size = (cnt + 9) / 10 * 10;
    if ((user_array.array = (User*)malloc(sizeof(User) * user_array_size)) == NULL) {
        printf("malloc error\n");
        exit(0);
    }

    //load users
    user_array.length = cnt;
    for (i = 0; i < cnt; ++i) {
        fscanf(file, "%d", &user_array.array[i].is_manager);
        user_array.array[i].name = input_string(file);
        user_array.array[i].username = input_string(file);
        user_array.array[i].password = input_string(file);
    }

    return 0;
}

//判断database中是否存在该user，根据username查询
//存在返回1，不存在返回0
int have_user(const char *username) {
    int i;
    UserArray ua = {NULL, 0};

    for (i = 0; i < user_array.length; ++i) {
        if (!strcmp(user_array.array[i].username, username)) {
            return 1;
        }
    }
    return 0;
}

//根据username找到user
//返回UserArray，其中的元素都是新malloc的地址
//如果不存在user，UserArray的length为0
UserArray find_user_by_username(const char *username) {
    UserArray ua = {NULL, 0};
    int i;
    for (i = 0; i < user_array.length; ++i) {
        if (!strcmp(user_array.array[i].username, username)) {
            int tmp = i;
            get_userArray(&ua, user_array.array[tmp]);
        }
    }

    return ua;
}


