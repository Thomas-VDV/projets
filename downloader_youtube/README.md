# YouTube Music Playlist Downloader

This script downloads videos or entire playlists from YouTube and, if specified, converts them to MP3 format while extracting metadata such as album art, artist, and album information.

## Features

- Download individual videos or entire playlists in MP4 format.
- Convert downloaded videos to MP3 format.
- Automatically extract metadata (title, artist, album, thumbnail) for MP3 files.
- Support for downloading a list of videos from a file.
  
## Requirements

Make sure you have the following installed:

- Python 3.x
- `pytube`
- `moviepy`
- `yt-dlp`
- `eyed3`
- `requests`
- `argparse`
- `ffmpeg`
  
To install the required Python libraries, run:

```bash
pip install pytube moviepy yt-dlp eyed3 requests argparse ffmpeg
```

## Usage

1. **Download a single video:**

    ```bash
    python downloader_youtube.py <YouTube_URL>
    ```

2. **Download audio only (MP3 format):**

    ```bash
    python downloader_youtube.py <YouTube_URL> -mp3
    ```

3. **Download a playlist:**

    ```bash
    python downloader_youtube.py <Playlist_URL> -p
    ```

4. **Download a list of videos (URLs in a file, one per line):**

    - For a list of individual videos:

        ```bash
        python downloader_youtube.py <file_with_urls.txt> -l
        ```

    - For a list of playlists:

        ```bash
        python downloader_youtube.py <file_with_urls.txt> -l -p
        ```

## How It Works

- The script uses `yt-dlp` for downloading videos.
- Converts video to MP3 if the `-mp3` option is provided.
- Fetches metadata like title, artist, and album using the YouTube video information.
- Saves the album artwork from the video as the cover image in the MP3 file using `eyed3`.

## Notes

- The script automatically skips already downloaded files.
