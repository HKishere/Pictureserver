#include <signal.h>
#include <cstdio>
#include <jsoncpp/json/json.h>
#include "mysql.hpp"
#include "httplib.h"
#include <openssl/md5.h>
#include <fstream>

class FileUtil{
public:
  static bool Write(const std::string& file_name, const std::string& content){
    std::ofstream file(file_name.c_str());
    if(!file.is_open()){
      return false;
    }
    file.write(content.c_str(), content.length());
    file.close();
    return true;
  }
  static bool Read(const std::string& file_name,std::string* content){
    std::ifstream file(file_name.c_str());
    if(!file.is_open()){
      return false;
    }
    struct stat st;
    stat(file_name.c_str(), &st);
    content->resize(st.st_size);

    file.read((char*)content->c_str(), content->size());
    file.close();
    return true;
  }
};

MYSQL* mysql;

int main(){
  using namespace httplib;
  using namespace image_system;

  mysql = image_system::MysqlInit();
  image_system::ImageTable image_table(mysql);
  signal(SIGINT, [](int){
        image_system::MysqlRelease(mysql);
      });
  Server svr;

  //lamda表达式不能直接访问image_table
  //需要使用[&image_table]来捕获变量
  svr.Post("/image",[&image_table](const Request& req, Response& resp){
      Json::FastWriter writer;
      Json::Value resp_json;
      printf("upload images\n");
      //auto size = req.files.size();
      auto ret = req.has_file("upload");
      if(!ret){
        printf("upload image failed!");
        resp.status = 404;
        //使用json格式返回出错结果
        resp_json["OK"] = false;
        resp_json["reason"] = "upload failed!,No data need to upload!";
        resp.set_content(writer.write(resp_json),"application/json");
        return;
      }
      //根据文件名获取文件数据
      const auto& file = req.get_file_value("upload");
      // file.filename;
      // file.content_type;
      //吧图片的属性信息插入到数据库
      Json::Value image;
      image["image_name"] = file.filename;
      image["size"] = (int)file.length;
      image["upload_time"] = "2018/08/31";
      image["md5"] = "ksihere";
      image["type"] = file.content_type;
      image["path"] = "./pic/" + file.filename;
      ret = image_table.Insert(image);
      if(!ret){
        printf("image_talbe Inset failed!\n");
        resp_json["OK"] = false;
        resp_json["reason"] = "database inset failed!";
        resp.status = 500;
        resp.set_content(writer.write(resp_json),"application/json");
        return ;
      }
      //把图片保存到指定的磁盘目录
      auto body = req.body.substr(file.offset, file.length);
      FileUtil::Write(image["path"].asString(),body);
      //构造一个响应数据告诉客户端上传成功
      resp_json["OK"] = true;
      resp.status = 200;

      //test
      //FileUtil::Write("./index/2.gif",body);
      //printf("size = %lu ret = %d filename = %s\n",size,ret,file.filename.c_str());
      resp.set_content(writer.write(resp_json),"application/json");
  });

  svr.Get("/image",[&image_table](const Request& req, Response& resp){
      (void)req;
        printf("get all pic imfo");
        Json::Value resp_json;
        Json::FastWriter writer;

        //调用数据库接口
        bool ret = image_table.SelectAll(&resp_json);
        if(!ret){
          printf("select failed!\n");
          resp_json["OK"] = false;
          resp_json["reason"] = "select DB failed!";
          resp.status = 500;
          resp.set_content(writer.write(resp_json),"application/json");
          return;
        }
        resp.status = 200;
        resp.set_content(writer.write(resp_json),"application/json");
      });
  //获取指定的图片信息
  svr.Get(R"(/image/?num=(\d+))",[&image_table](const Request& req, Response& resp){
          Json::FastWriter writer;
          Json::Value resp_json;
          int image_id = std::stoi(req.matches[1]);
          printf("get image info the id = %d\n",image_id);
          bool ret = image_table.SelectOne(image_id, &resp_json);
          if(!ret){
            printf("databases select failed!\n");
            resp_json["OK"] = false;
            resp_json["reason"];
            resp.status = 404;
            resp.set_content(writer.write(resp_json),"application/json");
            return;
          }
          resp_json["OK"] = true;
          resp.set_content(writer.write(resp_json),"application/json");
          return;
        });
  //获取图片内容
  svr.Get(R"(/show/(\d+))",[&image_table](const Request& req, Response& resp){
        Json::FastWriter writer;
        Json::Value resp_json;
        int image_id = std::stoi(req.matches[1]);
        printf("gei image where id = %d",image_id);
        Json::Value image;
        bool ret = image_table.SelectOne(image_id,&image);
        if(!ret){
          printf("databases select failed!\n");
          resp_json["OK"] = false;
          resp_json["reason"] = "databases select failed";
          resp.status = 404;
          resp.set_content(writer.write(resp_json),"application/json");
          return;
        }
        std::string image_body;
        printf("%s\n",image["path"].asCString());
        ret = FileUtil::Read(image["path"].asCString(), &image_body);
        if(!ret){
          printf("read image file failed!\n");
          resp_json["OK"] = false;
          resp_json["reason"] = "read image file failed!";
          resp.status = 500;
          resp.set_content(writer.write(resp_json),"application/json");
          return;
        }
        resp.status = 200;
        resp.set_content(image_body, image["type"].asCString());
      });

  svr.Delete(R"(/image/(\d+))",[&image_table](const Request&req, Response& resp){
        Json::FastWriter writer;
        Json::Value resp_json;

        int image_id = std::stoi(req.matches[1]);
        printf("delete image which id = %d\n",image_id);

        Json::Value image;
        bool ret = image_table.SelectOne(image_id, &image);
        if(!ret){
          printf("read database failed!\n");
          resp_json["OK"] = false;
          resp_json["reason"] = "read database failed!";
          resp.status = 404;
          resp.set_content(writer.write(resp_json),"application/json");
          return;
        }
        ret = image_table.Delete(image_id);
        if(!ret){
          printf("delete image failed!\n");
          resp_json["OK"] = false;
          resp_json["reason"] = "delete image failed!";
          resp.status = 404;
          resp.set_content(writer.write(resp_json),"application/json");
          return;
        }
        resp_json["OK"] = true;
        resp.status = 200;
        resp.set_content(writer.write(resp_json),"application/json"); 
      });

  svr.set_base_dir("./index");//设置资源路径
  svr.listen("0.0.0.0",3389);

  return 0;
}
