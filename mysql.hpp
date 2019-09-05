#pragma once 
#include <cstdlib>
#include <cstring>
#include <stdint.h>
#include <string>
#include <memory>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>
namespace image_system{
  static MYSQL* MysqlInit(){
    MYSQL* connect_fd = mysql_init(NULL);
    if(mysql_real_connect(connect_fd,"127.0.0.1","root","","image_system",3306,NULL,0) == NULL){
      printf("connect failed! %s\n",mysql_error(connect_fd));
      return NULL;
    }
    mysql_set_character_set(connect_fd,"utf8");
    return connect_fd;
}
static void MysqlRelease(MYSQL* mysql){
  mysql_close(mysql);
}
  
class ImageTable{
pubilc:
    ImageTable(MYSQL* mysql):mysql_(mysql){}
    bool Insert(const Json::value& image){
      char sql[4096] = {0};
     sprintf(sql,"insert into image_table values(NULL,'%s','%d','%s','%s','%s','%s')",
         image["name"].asCString(),image["size"].asInt(),image["upload_time"].asCString(),
         image["md5"],image["content_type"],asCString(),image["path"].asCString());
    }
    int ret = mysql_query(mysql_,sql){
      if(ret != 0){
        printf("Do SQL failed! sql = %s, %s\n",sql,mysql_error(mysql_));
        return false;
      }
      return true;
    }
    bool SelectAll(Json::value* image){
      char sql[1024*4] = {0};
      sprintf(sql,"select* from image_table");
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("Do SQL failed! %s\n",mysql_error(mysql_));
        return false;
      }
      MYSQL_RES* result = mysql_store_result(mysqql_);
      
    }
    bool SelectOne(int32_t image_id, Json::Value* image){
    }
    bool Delete(int image_id);
  };

