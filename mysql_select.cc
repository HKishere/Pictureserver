#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>

int main(){
  MYSQL* connect_fd = mysql_init(NULL);
  //建立连接
  if(mysql_real_connect(connect_fd,"127.0.0.1","root","","image_system",3306,NULL,0) == NULL){
    printf("connect failed! %s\n",mysql_error(connect_fd));
    return 1;
  }
  //设置字符编码
  mysql_set_character_set(connect_fd,"utf8");
  char sql[1024*4] = {0};
  sprintf(sql,"select*from image_table");
  //执行查询语句
  int ret = mysql_query(connect_fd,sql);
  if(ret < 0){
    printf("Do SQL failed! %s\n",mysql_error(connect_fd));
    return 1;
  }
  //获取结果集
  MYSQL_RES* result = mysql_store_result(connect_fd);
  if(result == NULL){
    printf("Get result failed! %s\n",mysql_error(connect_fd));
    return 1;
  }
  //获取结果集中的行数和列数
  int rows = mysql_num_rows(result);
  int fields = mysql_num_fields(result);
  printf("rows:%d, fields:%d\n", rows, fields);
  
  for(int i = 0;i <rows; ++i){
    MYSQL_ROW row = mysql_fetch_row(result);
    for(int j = 0;j < fields; ++j){
      printf("%s\t",row[j]);
    }
    printf("\n");
  }
  mysql_free_result(result);
  mysql_close(connect_fd);
  return 0;
}
