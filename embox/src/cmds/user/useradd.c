/**
 * @file
 * @brief
 *
 * @author  Vita Loginova
 * @date    13.02.2014
 */

#include <embox/cmd.h>

#include <unistd.h>
#include <ctype.h>

#include "user.h"

EMBOX_CMD(useradd);

static int get_default_pwd(struct passwd *result, char *name, char *buf,
		size_t buf_len) {

	/* set GROUP, HOME and SHELL */
	if (0 != get_defpswd(result, buf, buf_len)) {
		return -1;
	}

	result->pw_name = name;
	result->pw_passwd = "x";
	result->pw_uid = getmaxuid() + 1;
	result->pw_gecos = name;

	return 0;
}

static int create_user(char *name, char *home, char *shell, char *pswd,
		char *gecos, int group) {
	struct passwd pwd;
	FILE *pswdf, *sdwf;
	char buf_pswd[80];
	int res = 0;

	if (0 == (pswdf = fopen(PASSWD_FILE, "a"))) {
		return -1;
	}

	if (0 == (sdwf = fopen(SHADOW_FILE, "a"))) {
		fclose(sdwf);
		return -1;
	}

	if (0 != get_default_pwd(&pwd, name, buf_pswd, 80)) {
		res = -1;
		goto out;
	}

	set_options(&pwd, home, shell, gecos, group);

	write_user_passwd(&pwd, pswdf);

	/* shadow */
	{
		put_string(pwd.pw_name, sdwf, ':');
		put_string(pswd, sdwf, ':');
		put_string("::::::", sdwf, '\n');
	}

out:
	fclose(pswdf);
	fclose(sdwf);
	return res;
}

static int change_default_options(char *home, char *shell, int group){
	FILE *fd;
	char buff[80];
	struct passwd pwd;

	if (0 != get_defpswd(&pwd, buff, 80)) {
		return -1;
	}

	if (NULL == (fd = fopen(ADDUSER_FILE, "w"))) {
		return -1;
	}

	set_options(&pwd, home, shell, "", group);

	put_string("GROUP", fd, '=');
	put_int(pwd.pw_gid, fd, '\n');
	put_string("HOME", fd, '=');
	put_string(pwd.pw_dir, fd, '\n');
	put_string("SHELL", fd, '=');
	put_string(pwd.pw_shell, fd, '\n');

	fclose(fd);
	return 0;
}

static void print_help(void) {
	printf("Usage:\tuseradd [option] LOGIN\n"
			"\tuseradd -D\n"
			"\tuseradd -D [options]\n"
			"Options: see 'man usage'\n");
}

static int print_default_options(void) {
	FILE *fd;
	char buff;
	int number = 0, line = 0, new_line = 1;

	if (NULL == (fd = fopen(ADDUSER_FILE, "r"))) {
		return -1;
	}

	while (fread(&buff, 1, 1, fd) == 1) {
		if (new_line && number) {
			printf("\t%d %c", line++, buff);
		} else {
			printf("%c", buff);
		}
		new_line = (buff == '\n') ? 1 : 0;
	}
	fclose(fd);

	return 0;
}

static int useradd(int argc, char **argv) {
	char name[15], home[20] = "", shell[20] = "", pswd[15] = "", gecos[15] = "";
	int group = -1;
	int opt, count = 0, user_create = 1;

	if (argc >= 1) {
		getopt_init();
		while (-1 != (opt = getopt(argc, argv, "d:s:p:c:g:Dh"))) {
			count++;

			switch (opt) {
			case 'd':
				//todo: isdir
				strcpy(home, optarg);
				break;
			case 's':
				//todo: isshell
				strcpy(shell, optarg);
				break;
			case 'p':
				strcpy(pswd, optarg);
				break;
			case 'c':
				strcpy(gecos, optarg);
				break;
			case 'g':
				if ((group = get_group(optarg)) < 0) {
					printf("useradd: group '%i' doesn't exist\n", group);
					return 0;
				}
				break;
			case 'h':
				goto out;
			case 'D':
				if (count != 1) {
					goto out;
				}
				if (optind >= argc) {
					return print_default_options();
				}
				user_create = 0;
				break;
			default:
				printf("useradd: invalid option -%c\n", optopt);
				goto out;
			}
		}

		if (optind >= argc && user_create) {
			goto out;
		}

		if (user_create) {
			strcpy(name, argv[optind]);

			if (is_user_exists(name)) {
				printf("useradd: user '%s' already exists\n", name);
				return 0;
			}

			return create_user(name, home, shell, pswd, gecos, group);
		} else {
			return change_default_options(home, shell, group);
		}
	}

out:
	print_help();
	return 0;
}
