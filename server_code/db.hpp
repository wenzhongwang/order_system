/////////////////////////
//model层
////////////////////////
#pragma once

#include <cstdio>
#include <cstdlib>
#include <mysql/mysql.h>
#include <jsoncpp/json/json.h>

namespace order_system{
  
  //创建MySQL句柄并建立连接
static MYSQL* MySQLInit(){
    //1.创建句柄
    MYSQL* mysql = mysql_init(NULL);
    //2.建立连接
   if(mysql_real_connect(mysql,"127.0.0.1","root","","order_system",3306,NULL,0)==NULL){
    printf("mysql 连接失败 %s\n",mysql_error(mysql));
     return NULL; 
  }
  //3.设置编码方式
  mysql_set_character_set(mysql,"utf8");
  return mysql;
}

static void MySQLRelease(MYSQL* mysql){
  mysql_close(mysql);
}

class DishTable{
  public:
    DishTable(MYSQL* mysql) : mysql_(mysql){
      
    }
    // 使用JSON作为函数参数
    // jsoncpp
    bool Insert(const Json::Value& dish){
      char sql[1024 * 4] = {0};
      sprintf(sql,"insert into dish_table values(null,'%s','%d')",dish["name"].asCString(),dish["price"].asInt());
      //执行sql
      int ret = mysql_query(mysql_, sql);
      if(ret != 0){
        printf("DishTable Insert failed! %s\n",mysql_error(mysql_));
        return false;
      }
      printf("DishTable Insert OK!\n");
      return true;
    }
    
    //使用json表示输出型参数。Json::Valuue*
    //const & 表示输出型参数
    //* 表示输出型参数
    //& 表示输入输出型参数
    bool SelectAll(Json::Value* dishes){
      char sql[1024 * 4];
      sprintf(sql,"select dish_id, name, price from dish_table");
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("DishTable SelectAll failed! %s\n",mysql_error(mysql_));
        return false;
      }
      printf("DishTable SelectAll OK!\n");
      //构造结果
      MYSQL_RES* result = mysql_store_result(mysql_);
      //获取到行数
      int rows = mysql_num_rows(result);
      for(int i=0;i<rows;++i){
        MYSQL_ROW row = mysql_fetch_row(result);
        Json::Value dish;
        dish["dish_id"] = atoi(row[0]);
        dish["name"] = row[1];
        dish["price"] = atoi(row[2]);
        dishes->append(dish);
      }
      mysql_free_result(result);
     return true; 
    }

    bool SelectOne(int32_t dish_id, Json::Value* dish){
      //1.拼sql语句
      char sql[1024*4]={0};
      sprintf(sql,"select dish_id,name,price from dish_table where dish_id = %d",dish_id);
      //2.执行SQL
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("DishTable SelectOne failed! %s",mysql_error(mysql_));
        return false;
      }
      //3.遍历结果集合
      MYSQL_RES* result = mysql_store_result(mysql_);
      int rows = mysql_num_rows(result);
      if(rows != 1){
        printf("DishTable SelectOne result rows not 1!");
        mysql_free_result(result); 
        return false;
      }
      //取出这一行数据
      MYSQL_ROW row = mysql_fetch_row(result);
      (*dish)["dish_id"] = dish_id;
      (*dish)["name"] = row[1];
      (*dish)["price"] = atoi(row[2]);
      mysql_free_result(result);
      return true;
    }

    bool Update(const Json::Value& dish){
      //1.拼装SQL
      char sql[104*4]={0};
      sprintf(sql,"update dish_table set name = '%s',price=%d where dish_id = %d",dish["name"].asCString(),dish["price"].asInt(),dish["dish_id"].asInt());
      //printf("sql: %s\n",sql);
      //2.执行SQL
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("DishTable Update failed! %s\n",mysql_error(mysql_));
        return false;
      }
      printf("DishTable Update OK!\n");
      return true;
    }

    bool Delete(int32_t dish_id){
      //1.拼装SQL
      char sql[104 * 4] = {0};
      sprintf(sql,"delete from dish_table where dish_id = %d",dish_id);
      //printf("sql: %s\n",sql);
      //2.执行SQL
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("DishTable Delete failed! %s\n",mysql_error(mysql_));
        return false;
      }
      printf("DishTable Delete OK!\n");
      return true;
    }

private:
    MYSQL* mysql_;
};

class OrderTable{
  public:
    OrderTable(MYSQL* mysql) : mysql_(mysql){

    }

    bool Insert(const Json::Value& orders){
      //1.拼装SQL
      char sql[1024 * 4]={0};
      sprintf(sql,"insert into order_table values(null,'%s','%s','%s','%d')",orders["table_id"].asCString(),orders["time"].asCString(),orders["dishes"].asCString(),orders["status"].asInt());
      //2.执行SQL
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("OrdrTable Insert failed! %s \n",mysql_error(mysql_));
        return false;
      }
      printf("OrderTable Insert ok! \n");
      return true;
    }

    bool SelectAll(Json::Value* orders){
      //1.拼装SQL
      char sql[1024 * 4]={0};
      sprintf(sql,"select order_id, table_id, time, dishes, status from order_table");
      //2.执行SQL
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("OrdrTable Select failed! %s \n",mysql_error(mysql_));
        return false;
      }
      printf("OrderTable Select ok! \n");
      //3.拼装到输出型参数中
      MYSQL_RES* result = mysql_store_result(mysql_);
      int rows = mysql_num_rows(result);
      for(int i=0;i<rows;++i){
        MYSQL_ROW row = mysql_fetch_row(result);
        Json::Value order;
        order["order_id"]=atoi(row[0]);
        order["table_id"]=row[1];
        order["time"]=row[2];
        order["dishes"]=row[3];
        order["ststus"]=atoi(row[4]);
        orders->append(order);
      }
      mysql_free_result(result);
      return true;
    }

    bool ChangeState(const Json::Value& order){
      //1.拼装SQL
      char sql[1024 * 4]={0};
      sprintf(sql,"update order_table set status = %d where order_id = %d",order["status"].asInt(),order["order_id"].asInt());
      //2.执行SQL
      int ret = mysql_query(mysql_,sql);
      if(ret != 0){
        printf("OrdrTable ChangeState failed! %s \n",mysql_error(mysql_));
        return false;
      }
      printf("OrderTable ChangeState ok! \n");
      return true;
    }

  private:
    MYSQL* mysql_;
};

} // end order_system
