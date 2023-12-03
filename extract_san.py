import os
import csv

def extract_san_moves_from_pgn(pgn_file_path):
    san_moves = []
    current_game = []

    with open(pgn_file_path) as pgn_file:
        in_game_moves = False

        for line in pgn_file:
            line = line.strip()

            if line.startswith('1.'):
                if current_game:
                    san_moves.append(' '.join(current_game))
                    current_game = []
                in_game_moves = True
                current_game.append(line)
                continue

            if in_game_moves and line and line[0].isdigit():
                current_game.append(line)

    if current_game:
        san_moves.append(' '.join(current_game))

    return san_moves

def save_san_moves_to_csv(folder_path, csv_file_path):
    with open(csv_file_path, 'w', newline='') as csv_file:
        writer = csv.writer(csv_file)

        for filename in os.listdir(folder_path):
            if filename.endswith(".pgn"):
                file_path = os.path.join(folder_path, filename)
                san_moves = extract_san_moves_from_pgn(file_path)
                writer.writerows([[game] for game in san_moves])

if __name__ == "__main__":
    # Replace 'your_folder_path' and 'output.csv' with the actual paths
    folder_path = '/mnt/e/downloaded_pgns_full/'
    csv_file_path = 'outputfull.csv'

    save_san_moves_to_csv(folder_path, csv_file_path)

    print(f'SAN moves from PGN files in {folder_path} saved to {csv_file_path}')
