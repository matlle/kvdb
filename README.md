**Kvdb is a key-value data storage engine. Cross-Platform.**


* keys and values are char arrays.
* BTREE data structure is used to store and retrive data.
* data are organized by databases and tables.
* a database is a directory on the filesystem which can contains tables. Tables are represented by folders inside the
  database directory.
* data in tables are stored in binary file stream.

# Examples
* to select/create a new database testdb (if the database doesn't exist, it will be created)
   * > selectdb /path/testdb
* to save data in a table students (if the table doesn't exist, it will be created)
   * >testdb> students.put(id=1,firstname=Alice,lastname=Smith)
* to search rows from the table students  
   * >testdb> students.get(id=1) // will return all rows which contain this key and value from the table  
     testdb> students.get() // will return all rows from the table  
     testdb> students.get(key=value).order_by(key) // will return and sort rows by this key  
     testdb> students.get(key=value).order_by(key=desc) // will return and sort rows in reverse order by this key  
* to delete rows from a table  
   * >testdb> students.delete(key=value) // will delete all rows which contain this key and value  
     testdb> students.delete() // will delete all rows from the table  
     
