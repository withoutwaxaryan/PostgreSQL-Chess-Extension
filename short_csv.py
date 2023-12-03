import csv

def copy_first_100_rows(input_file, output_file):
    with open(input_file, 'r', newline='') as infile, open(output_file, 'w', newline='') as outfile:
        reader = csv.reader(infile)
        writer = csv.writer(outfile)

        # Write the header if present
        header = next(reader, None)
        if header:
            writer.writerow(header)

        # Write the first 100 rows
        for _ in range(30000):
            row = next(reader, None)
            if row:
                writer.writerow(row)
            else:
                break

# Example usage:
input_csv_file = '/mnt/e/output.csv'
output_csv_file = '/mnt/e/output30000.csv'
copy_first_100_rows(input_csv_file, output_csv_file)
