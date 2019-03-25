#include <pqxx/pqxx>
#include <iostream>
#include <string>
using namespace std;
using namespace pqxx;
std::vector<string> id_set;
std::vector<string> symbol_set;



class DBOperator{

 public:
  void init_db();
  void insert_attribute(string name);
  void insert_account(string id, string balance);
  void insert_sym(string id, string sym, string value);
  bool checkId(string id);
  bool checkSymbol(string name);
};


void DBOperator::init_db(){

  std::string sql;

  try{
    connection C("dbname=hw4database user=userhw4 password=000000");
    if (C.is_open()) {
      cout << "Opened database successfully: " << C.dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return;
    }
    /* Create SQL statement */
    sql = "CREATE TABLE account(account_id BIGINT   PRIMARY KEY, balance INT DEFAULT 0);";

    std::string drop_table = "DROP TABLE IF EXISTS account;";

    work W(C);


    //W.exec(drop_table);
    W.exec( drop_table );
    W.exec( sql );
    W.commit();
    cout << "Table created successfully" << endl;
    C.disconnect ();
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return;
  }

  return;
  

}



void DBOperator::insert_attribute(string name){

  try{
    symbol_set.push_back(name);
    connection C("dbname=hw4database user=userhw4 password=000000");
    if (C.is_open()) {
      //      cout << "Opened database successfully: " << C.dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return;
    }
    /* Create SQL statement */
    std::string sql = "ALTER TABLE account ADD COLUMN " +name+" VARCHAR DEFALUT NULL;";

    work W(C);
    
    W.exec( sql );
    W.commit();
    // cout << "Table created successfully" << endl;
    C.disconnect ();
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return;
  }

  return;
  
  
}
bool DBOperator::checkSymbol(string name){
  bool exist = false;
  vector<string>::iterator it = symbol_set.begin();
  while(it != symbol_set.end()){
    if((*it).compare(name)==0){
      exist = true;
    }
    ++it;
  }

  return exist;

}
bool DBOperator::checkId(string id){


  bool exist = false;
  vector<string>::iterator it = id_set.begin();
  while(it != id_set.end()){


    if((*it).compare(id)==0){
      exist = true;
    }
    ++it;

  }
  return exist;

}


void DBOperator::insert_account(string id, string balance){
  try{

   

      id_set.push_back(id);

   

    
    connection C("dbname=hw4database user=userhw4 password=000000");
    if (C.is_open()) {
      //      cout << "Opened database successfully: " << C.dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return;
    }
    /* Create SQL statement */
    std::string sql = "INSERT INTO account(account_id, balance) VALUES ( '"+id+"', '"+balance+"' );";
    work W(C);

    W.exec( sql );
    W.commit();
    // cout << "Table created successfully" << endl;
    C.disconnect ();
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return;
  }

  return;
  

}




void DBOperator::insert_sym(string id, string sym, string value){
  try{

   
    connection C("dbname=hw4database user=userhw4 password=000000");
    if (C.is_open()) {
      //      cout << "Opened database successfully: " << C.dbname() << endl;
    } else {
      cout << "Can't open database" << endl;
      return;
    }
    /* Create SQL statement */
    std::string sql = "UPDATE account SET "+sym+" = '"+value+"' WHERE account_id = '"+id+"' ;";
    work W(C);

    W.exec( sql );
    W.commit();
    // cout << "Table created successfully" << endl;
    C.disconnect ();
  } catch (const std::exception &e){
    cerr << e.what() << std::endl;
    return;
  }

  return;


}
