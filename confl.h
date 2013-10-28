/*
  author: liuhengloveyou@gmail.com
  date  : 2013-10-09

  C语言读写配置文件实现

  每行为一条记录, 格式如下：
  key = value # 注释

  '#'字符后的所有内容都作为注释忽略
  key和value的头尾空白会被砍掉, 中间可以有空格
*/

#ifndef C_CONFIG_L
#define C_CONFIG_L

#ifdef __cplusplus
extern "C" {
#endif


struct config
{
	char *name;
	char *value;
	struct config *next;
};
typedef struct configl
{
	char *fn;
	struct config *confs;
} config_t;

config_t *confRead(const char *file_name);
int confWrite(config_t *one);
void confDestory(config_t *one);
const char *confGet(const config_t *one, const char *key);
int confSet(config_t *one, const char *key, const char *val);
void confDel(config_t *one, const char *key);

#ifdef __cplusplus
}
#endif

#endif
