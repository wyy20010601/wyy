#include "user.h"
#define __SUCCESS 0
#define __DEFEAT  1
int user_register(User user) {
	FILE *fp;

	struct User new_user;
	struct User temp;

	if ((fp = fopen(user_file, "ab+")) == NULL) {
		printf("file open error!\n");
		exit(0);
	}


	//find whether there is another user uses this username
	while (fread(&temp, sizeof(struct account), 1, fp) != NULL)
		if (!strcmp(new_user.username, temp.username))
			break;

    //����Ѵ��ڸ�username���������ɹ�
    UserArray ua = find_user_by_username(user.username);
	if (!strcmp(new_user.username, temp.username)) {
		printf("this username has exists, please change one.\n");
		//������ھ�������
		return __DEFEAT;
		//user_register();
	}
	else {

		printf("please input the password:");
		//��������
		get_password(new_user.password);


		//������˺�����
		fwrite(&new_user, sizeof(struct account), 1, fp);
		fclose(fp);
		store(&new_user);
		//�����û�����
		user_interface(new_user.ID);
	}

	return __SUCCESS;
}

void get_password(char* password) {
	char temp;
	//get character number
	int count = 0;
	while (temp = getch()) {
		//user press the 'enter'
		if (temp == '\r') {
			password[count] = '\0';
			break;
		}

		//backspace
		if (temp == 8)
			if (count == 0)	continue;
			else {
				//ģ���˸�
				--count;
				putchar('\b');
				putchar(' ');
				putchar('\b');
			}
		else {
			//save the char
			password[count++] = temp;
			putchar('*');
		}
	}
}


int store_users(File *file) {

}

UserArray find_user_by_username(const char *username);
