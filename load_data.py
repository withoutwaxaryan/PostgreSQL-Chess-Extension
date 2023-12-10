import csv
import psycopg2
from psycopg2 import sql

# Replace these values with your PostgreSQL connection details
db_params = {
    'host': 'localhost',
    'port': '5432',
    'database': 'chess',
    'user': 'postgres',
    'password': '12345'
}

# CSV file path
csv_file_path = '/mnt/e/outputfull.csv'

# Table and column names
table_name = 'new'
column_name = 'moves'

# Connect to the PostgreSQL database
try:
    connection = psycopg2.connect(**db_params)
    cursor = connection.cursor()

    # Create the table if it doesn't exist
    create_table_query = sql.SQL("""
        CREATE TABLE IF NOT EXISTS {table} (
            {column} chessgame  -- Adjust the data type based on your CSV structure
        )
    """).format(table=sql.Identifier(table_name), column=sql.Identifier(column_name))
    cursor.execute(create_table_query)
    connection.commit()

    # Read and insert rows from the CSV file
    with open(csv_file_path, 'r', encoding='utf-8', errors='ignore') as csv_file:
        csv_reader = csv.reader(csv_file)
        next(csv_reader)  # Skip header if present

        for row in csv_reader:
            column_value = row[0]  # Adjust the index based on your CSV structure
            insert_query = sql.SQL("""
                INSERT INTO {table} ({column})
                VALUES (%s)
            """).format(table=sql.Identifier(table_name), column=sql.Identifier(column_name))
            cursor.execute(insert_query, (column_value,))

    connection.commit()

except psycopg2.Error as e:
    print("Error: Unable to connect to the database")
    print(e)

finally:
    if connection:
        cursor.close()
        connection.close()
        print("Connection closed")


# insert into new select '1. e4 e5 2. b5 b7' from generate_series(1, 400000);