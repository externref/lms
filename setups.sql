CREATE TABLE IF NOT EXISTS books
  (
     id       INTEGER PRIMARY KEY,
     name     VARCHAR,
     category VARCHAR,
     author   VARCHAR,
     year     INTEGER
  );

CREATE TABLE IF NOT EXISTS user
  (
     id         INTEGER PRIMARY KEY,
     name       VARCHAR,
     address    VARCHAR,
     phone      INTEGER,
     dob        INTEGER,
     registered TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  );

CREATE TABLE IF NOT EXISTS borrowed
  (
     book_id        INTEGER,
     user_id        INTEGER,
     borrowed_on TIMESTAMP DEFAULT CURRENT_TIMESTAMP
  ); 
