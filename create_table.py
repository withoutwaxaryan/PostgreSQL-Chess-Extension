import csv
import psycopg2

def create_table(cursor):
    # Create a table if it doesn't exist
    create_table_query = """
        CREATE TABLE IF NOT EXISTS chess_games100 (
            id SERIAL PRIMARY KEY,
            moves chessgame
        )
    """
    cursor.execute(create_table_query)

def insert_values(cursor, csv_file_path):
    with open(csv_file_path, 'r', encoding='utf-8', errors='ignore') as csv_file:
        csv_reader = csv.reader(csv_file)
        next(csv_reader)  # Skip header if present

        for row in csv_reader:
            column_value = row[0]  # Adjust the index based on your CSV structure
            insert_query = """
                INSERT INTO chess_games100 (moves)
                VALUES (%s)
            """
            cursor.execute(insert_query, (column_value,))

def main():
    try:
        # Replace these with your actual database connection details
        db_params = {
            'dbname': 'chess',
            'user': 'postgres',
            'password': '12345',
            'host': 'localhost',
            'port': '5432',
        }

        conn = psycopg2.connect(**db_params)
        cursor = conn.cursor()

        # Replace 'your_table_name' with the actual table name
        create_table(cursor)

        # Replace 'your_csv_file.csv' with the actual CSV file path
        csv_file_path = '/mnt/e/output100.csv'
        insert_values(cursor, csv_file_path)

        conn.commit()
        print("Values inserted successfully!")

    except psycopg2.Error as e:
        print(f"Error: {e}")
    finally:
        if conn:
            conn.close()

if __name__ == "__main__":
    main()
