/**
 * @file
 *
 * @date 28.10.13
 * @author Anton Bondarev
 */

#include <limits.h>
#include <string.h>
#include <libgen.h>

char * basename(char *path) {
	static char bname_buff[NAME_MAX + 1];
	int len;
	int start;
	int clen;

	if (path == NULL) {
		return strcpy(&bname_buff[0], ".");
	}
	if (0 == (len = strlen(path))) {
		return strcpy(&bname_buff[0], ".");
	}
	while (path[len - 1] == '/' && len > 1) {
		len--;
	}

	start = len - 1;
	while (start > 0 && path[start] != '/') {
		start--;
	}

	if (start == 0) {
		if (path[start] != '/' || len == 1) {
			clen = len - start;
		} else {
			clen = len - start - 1;
			start++;
		}
	} else {
		clen = len - start - 1;
		start++;
	}

	if ((clen - 1) > sizeof (bname_buff) ) {
		clen = sizeof (bname_buff) - 1;
	}

	memcpy(bname_buff, &path[start], clen);
	bname_buff[clen] = '\0';

	return bname_buff;
}

char  *dirname(char *path) {
	static char dname_buff[PATH_MAX + 1];
	int len;
	int start;
	//int slash;
	//int clen;

	if (path == NULL) {
		return strcpy(&dname_buff[0], ".");
	}
	if (0 == (len = strlen(path))) {
		return strcpy(&dname_buff[0], ".");
	}
	while (path[len - 1] == '/' && len > 1) {
		len--;
	}

	start = len - 1;
	while (start > 0 && path[start] != '/') {
		start--;
	}

	if (start) {
		memcpy(dname_buff, path, start);
		dname_buff[start] = '\0';
	} else if (path[0] != '/'){
		return strcpy(&dname_buff[0], ".");
	} else {
		memcpy(dname_buff, path, 1);
		dname_buff[1] = '\0';
	}

	return dname_buff;
}
