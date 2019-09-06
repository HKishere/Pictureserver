#include "mysql.hpp"


void TestImageTable(){
  MYSQL*mysql = image_system::MysqlInit();
  image_system::ImageTable image_table(mysql);
  bool ret = false;
  
  //插入数据
  //Json::Value image;
  //image["image_name"] = "test.png";
  //image["size"] = 1024; 
  //image["upload_time"] = "2019/8/31";
  //image["md5"] = "abbbb";
  //image["content_type"] = "png";
  //image["path"] = "data/test.png";
  //ret = image_table.Insert(image);
  //printf("ret = %d\n",ret);
  

  //查询图片信息
  //Json::StyledWriter writer;
  //Json::Value image;
  //ret = image_table.SelectAll(&image);
  //printf("ret = %d\n",ret);
  //printf("%s\n",writer.write(image).c_str());
  
  //查询指定图片信息
  //Json::StyledWriter writer;
  //Json::Value image;
  //ret = image_table.SelectOne(2,&image);
  //printf("ret = %d\n",ret);
  //printf("%s\n",writer.write(image).c_str());
  
  //删除指定图片信息
  ret = image_table.Delete(1);
  printf("ret = %d\n",ret);
  
  
  image_system::MysqlRelease(mysql);
}


int main(){
  TestImageTable();
  return 0;
}
