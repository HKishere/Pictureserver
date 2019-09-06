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
    if(mysql_real_connect(connect_fd,"127.0.0.1","root","kishere","image_system",3306,NULL,0) == NULL){
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
    public:
      ImageTable(MYSQL* mysql):mysql_(mysql){}
      //函数参数较多,所以使用json
      bool Insert(const Json::Value& image){
        char sql[4096] = {0};
        //使用键值对作为函数参数
        //各个键值对的名称一定要严格相同,类型也是
        sprintf(sql,"insert into image_table values(NULL,'%s','%d','%s','%s','%s','%s')",
            image["image_name"].asCString(),image["size"].asInt(),image["upload_time"].asCString(),
            image["md5"].asCString(),image["content_type"].asCString(),image["path"].asCString());
        printf("[Insert sql] %s\n",sql);
        int ret = mysql_query(mysql_,sql);
          if(ret != 0){
            printf("Do SQL failed! sql = %s, %s\n",sql,mysql_error(mysql_));
            return false;
          }
          return true;
        }//拼装SQL语句可能会导致SQL注入攻击
      bool SelectAll(Json::Value* images){
        char sql[1024*4] = {0};
        sprintf(sql,"select* from image_table");
        int ret = mysql_query(mysql_,sql);
        if(ret != 0){
          printf("Do SQL failed! %s\n",mysql_error(mysql_));
          return false;
        }
        MYSQL_RES* result = mysql_store_result(mysql_);

        int rows = mysql_num_rows(result);
        for(int i = 0;i < rows;++i){
          MYSQL_ROW row = mysql_fetch_row(result);
          Json::Value image;
          image["image_id"] = atoi(row[0]);
          image["image_name"] = (row[1]);
          image["size"] = atoi(row[2]);
          image["upload_time"] = (row[3]);
          image["md5"] = (row[4]);
          image["type"] = (row[5]);
          image["path"] = (row[6]);
          images->append(image);
          mysql_free_result(result);
        }
        return true;

      }
      bool SelectOne(int32_t image_id, Json::Value* ptr_image){
        char sql[1024*4] = {0};
        sprintf(sql,"select* from image_table where image_id = %d",image_id);
        int ret = mysql_query(mysql_,sql);
        if(ret != 0){
          printf("Do SQL failed! %s\n",mysql_error(mysql_));
          return false;
        }
        MYSQL_RES* result = mysql_store_result(mysql_);
        int rows = mysql_num_rows(result);
        if(rows != 1){
          printf("Found more than 1 result! %d result has been Found\n",rows);
        }
          MYSQL_ROW row = mysql_fetch_row(result);
          Json::Value image;
          image["image_id"] = atoi(row[0]);
          image["image_name"] = (row[1]);
          image["size"] = atoi(row[2]);
          image["upload_time"] = (row[3]);
          image["md5"] = (row[4]);
          image["type"] = (row[5]);
          image["path"] = (row[6]);
          *ptr_image =image;
          mysql_free_result(result);
        return true;
      }
      bool Delete(int image_id){
        char sql[1024*4] ={0};
        sprintf(sql,"delete from image_table where image_id = %d",image_id);
        int ret = mysql_query(mysql_,sql);
        if(ret != 0){
          printf("Do SQL failed! %s\n",mysql_error(mysql_));
          return false;
        }
        return true;
      }
    private:
      MYSQL* mysql_;
  }; 

}//end image_system
