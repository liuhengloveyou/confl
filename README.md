confl
=====

C语言配置文件读写

/*
  author: liuhengloveyou@gmail.com
  date  : 2013-10-09

  C语言读取配置文件实现

  每行为一条记录, 格式如下：
  key = value # 注释

  '#'字符后的所有内容都作为注释忽略
  key和value的头尾空白会被砍掉, 中间可以有空格
*/
