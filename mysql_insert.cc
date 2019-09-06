#include <cstdio>
#include <cstdlib>
#include <memory>
#include <mysql/mysql.h>

int main(){
  //链接数据库
  MYSQL* connect_fd = mysql_init(NULL);
  if(mysql_real_connect(connect_fd,"127.0.0.1","root","","image_system",3306,NULL,0)==NULL){
    printf("connect failed! %s\n",mysql_error(connect_fd));
    return 1;
  }
  mysql_set_character_set(connect_fd,"utf8");
  char sql[4096] = {0};
  char image_name[]="kishere's_pic.png";
  int size = 16*1024;
  char upload_time[] = "2019/8/26";
  char md5[] = "12345";
  char content_type[] = "png";
  char path[] = "./kishere's_pic.png";
  sprintf(sql,"inset into image_table values(null,'%s',%d,'%s','%s','%s','%s')",image_name, size, upload_time, md5, content_type, path);
  int ret = mysql_query(connect_fd,sql);
  if(ret != 0){
    printf("do SQL failed! %s\n",mysql_error(connect_fd));
    return 1;
  }
  mysql_close(connect_fd);
  printf("do SQL successful!");
  return 0;
}
