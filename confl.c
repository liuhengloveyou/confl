#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "confl.h"

static void freeConfigOne(struct config **one)
{
	struct config *tmp = *one;

	if (!tmp) {
		return;
	}
	if (tmp->name) {
		free(tmp->name);
		tmp->name = NULL;
	}
	if (tmp->value) {
		free(tmp->value);
		tmp->value = NULL;
	}

	*one = tmp->next;
	free(tmp);

	return;
}

static struct config **findNode(config_t *one, const char *key)
{
	if (!one) {
		return NULL;
	}

	if (!strcmp(one->confs->name, key)) {
		return &one->confs;
	}

	for (struct config *tmp = one->confs; tmp; tmp = tmp->next) {
		if (tmp->next && !strcmp(tmp->next->name, key)) {
			return &tmp->next;
		}
	}

	return NULL;
}

struct config *newConfig(const char *key, const char *val)
{
	size_t keyLen = strlen(key) + 1;
	size_t valLen = strlen(val) + 1;
	if (keyLen <= 1 || valLen <= 1) {
		return NULL;
	}

	struct config *one = malloc(sizeof(struct config));
	if (one) {
		one->next = NULL;

		one->name = malloc(keyLen);
		if (!one->name) {
			free(one);
			return NULL;
		}
		strcpy(one->name, key);

		one->value = malloc(valLen);
		if (!one->value) {
			free(one->name);
			free(one);
			return NULL;
		}
		strcpy(one->value, val);
	}

	return one;
}

config_t *newConfigl(const char *file_name, size_t len)
{
	config_t *one = malloc(sizeof(config_t));
	if (one) {
		one->fn = malloc(len+1);
		if (!one->fn) {
			return NULL;
		}
		strcpy(one->fn, file_name);

		one->confs = NULL;
	}

	return one;
}

void confDestory(config_t *one)
{
	if (!one) {
		return;
	}
	if (one->fn) {
		free(one->fn);
		one->fn = NULL;
	}
	while (one->confs) {
		freeConfigOne(&one->confs);
	}
}

int confSet(config_t *one, const char *key, const char *val)
{
	if (!one) {
		return 0;
	}
	
	struct config **tmp = findNode(one, key);
	if (tmp) {
		return -2;
	}

	struct config *node = newConfig(key, val);
	if (!node) {
		return -1;
	}

	if (one->confs) {
		node->next =  one->confs;
	}

	one->confs = node;

	return 0;
}


const char *confGet(config_t *one, const char *key)
{
	struct config **tmp = findNode(one, key);
	if (!tmp) {
		return NULL;
	}

	return (const char *)(*tmp)->value;
}

void confDel(config_t *one, const char *key)
{
	struct config **tmp = findNode(one, key);
	if (tmp) {
		freeConfigOne(tmp);
	}

	return;
}

int confWrite(config_t *one)
{
	if (!one) {
		return -1;
	}
	if (!one->fn) {
		return -1;
	}

	FILE *fp = fopen(one->fn, "w");
	if (NULL == fp) {
		return -1;
	}

	for (struct config *tmp = one->confs; tmp; tmp = tmp->next) {
		if (tmp->name && tmp->value) {
			fprintf(fp, "%s = %s\n", tmp->name, tmp->value);	
		}
	}

	fclose(fp);
	
	return 0;
}

config_t *confRead(const char *file_name)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	size_t palen = strlen(file_name);
	if (palen <= 0) {
		return NULL;
	}

	config_t *one = newConfigl(file_name, palen);
	if (!one) {
		return NULL;
	}

	FILE *fp = fopen(file_name, "r");
	if (NULL == fp) {
		return NULL;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		char *tmp = NULL, *space = NULL;
		char *key = NULL, *val = NULL;
		unsigned char stat = 0; //状态机

		for (tmp = line; *tmp; ++tmp) {
			if ('#' == *tmp || 0x0a == *tmp || 0x0d == *tmp) {
				if (space) {
					*space = 0;
				} else {
					*tmp = 0;
				}
				break;
			} else if (isspace(*tmp)) {
				if (1 == stat) {
					stat = 2;
					space = tmp;
				} else if (4 == stat) {
					stat = 5;
					space = tmp;
				}
				continue;
			} else if ('=' == *tmp) {
				stat = 3;
				if (space) {
					*space = 0;
					space = NULL;
				}
				continue;
			}

			if (0 == stat) {
				stat = 1;
				key = tmp;
				continue;
			}
			if (3 == stat) {
				stat = 4;
				val = tmp;
				continue;
			}
			if (3 > stat) {
				stat = 1;
				space = NULL;
			} else if (5 == stat) {
				stat = 4;
				space = NULL;
			}
		}

		// 新的配置项
		if (key && val) {
			confSet(one, key, val);
		}
	}

	free(line);
	fclose(fp);
	
	return one;
}


int main(int argc, char *argv[])
{
	config_t *myconf = confRead("./configfile.prop");

	printf("val:'%s'\n", confGet(myconf, "bbb"));

	confDel(myconf, "c");

	confWrite(myconf);

	confDestory(myconf);

	return 0;
}
