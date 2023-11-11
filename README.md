Run the following commands to make and install chess extension:
```
>> PATH=/usr/local/pgsql/bin:$PATH
>> 
>> cd “path_to_chess_directory”/chess/
>>
>> make
>>
>> sudo make install
```

After, in order to create and use the chess extension:

```
>> createdb chess (or any other name you want for the database)
>>
>> psql chess
>>
>> chess=# CREATE EXTENSION chess;
```
