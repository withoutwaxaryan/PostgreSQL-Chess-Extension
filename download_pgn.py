import os
import requests
from bs4 import BeautifulSoup
from urllib.parse import urljoin

def download_file(url, dest_folder):
    response = requests.get(url)
    if response.status_code == 200:
        filename = os.path.join(dest_folder, url.split("/")[-1])
        with open(filename, 'wb') as file:
            file.write(response.content)
        print(f"Downloaded: {filename}")
    else:
        print(f"Failed to download: {url}")

def download_pgn_files(base_url, dest_folder):
    # Fetch HTML content from the website
    response = requests.get(base_url)
    if response.status_code == 200:
        soup = BeautifulSoup(response.content, 'html.parser')
        # Find all links to PGN files
        pgn_links = [urljoin(base_url, a['href']) for a in soup.find_all('a', href=True) if a['href'].endswith('.pgn')]
        # Create the destination folder if it doesn't exist
        os.makedirs(dest_folder, exist_ok=True)
        # Download each PGN file
        for link in pgn_links:
            download_file(link, dest_folder)
    else:
        print(f"Failed to fetch HTML from {base_url}")

def main():
    # Set the base URL and destination folder
    base_url = "https://www.pgnmentor.com/files.html"
    dest_folder = "/mnt/e/downloaded_pgns_full"

    # Download PGN files
    download_pgn_files(base_url, dest_folder)

if __name__ == "__main__":
    main()
