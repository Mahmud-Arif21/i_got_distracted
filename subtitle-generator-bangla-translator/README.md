# Subtitle Generator & Bangla Translator

A simple GUI application to generate English subtitles from audio files (converted from the original video file) using AssemblyAI, optionally translate them into Bangla using the `deep_translator` library with Google Translate, and convert video files to audio. You can play the original video, drag and drop the srt file on the media player and get the subtitle.

## Overview

This tool allows beginners to:
- Select an audio file (e.g., `.mp3`, `.wav`, `.m4a`, `.flac`, `.aac`) or convert a video file (e.g., `.mp4`, `.avi`, `.mkv`) to audio.
- Generate English subtitles (SRT format) via the AssemblyAI API.
- Optionally translate the subtitles into Bangla with improved sentence boundary detection.

The Bangla translation has been enhanced by:
- Using `ends_with_punctuation` to detect sentence endings (e.g., `.`, `!`, `?`, `…`).
- Splitting text proportionally to maintain subtitle timing accuracy during translation.

## Prerequisites

- Python 3.7 or higher.
- An AssemblyAI API key (sign up at [AssemblyAI](https://www.assemblyai.com/) and get your API key by:
  1. Visiting the [AssemblyAI dashboard](https://www.assemblyai.com/app).
  2. Signing in or creating an account.
  3. Navigating to the "API Keys" section.
  4. Copying the provided API key or generating a new one if needed).
- FFmpeg installed for video-to-audio conversion (install via `brew install ffmpeg` on macOS, `sudo apt-get install ffmpeg` on Linux, or download from [FFmpeg](https://ffmpeg.org/download.html) for Windows).

## Installation

### Using Conda Environment

1. Install [Miniconda](https://docs.conda.io/en/latest/miniconda.html) or [Anaconda](https://www.anaconda.com/products/distribution).
2. Create a new Conda environment and install dependencies:
   ```bash
   conda create -n subtitle_env python=3.9
   conda activate subtitle_env
   pip install -r requirements.txt
   ```
3. Set your AssemblyAI API key in the `subtitle.py` script:
   - Open `subtitle.py` in a text editor.
   - Find the line `API_KEY = 'YOUR_API_KEY_HERE'`.
   - Replace `'YOUR_API_KEY_HERE'` with your actual API key from the AssemblyAI dashboard.

### Manual Installation

1. Download the project folder:
   - Visit [this link](https://minhaskamal.github.io/DownGit/#/home?url=https://github.com/Mahmud-Arif21/i_got_distracted/tree/main/subtitle-generator-bangla-translator) to download the `subtitle-generator-bangla-translator` folder as a ZIP file.
   - Extract the ZIP file to a folder on your computer.
   - Navigate to the extracted folder:
     ```bash
     cd path/to/extracted/subtitle-generator-bangla-translator
     ```
2. Install dependencies using pip:
   ```bash
   pip install -r requirements.txt
   ```
3. Set your AssemblyAI API key in the `subtitle.py` script as described above.

## Usage

### Subtitle Generation and Translation
1. Ensure your API key is set in `subtitle.py`.
2. Run the application:
   ```bash
   python subtitle.py
   ```
3. Click "Select Audio File" to choose your audio file (e.g., `sample.mp3`).
4. Check "Enable Bangla Translation" if you want Bangla subtitles (optional).
5. Click "Run" to process the file.
   - The app will generate `filename.srt` (English) and, if translation is enabled, `filename_bangla.srt`.
   - Overwrite prompts appear if files exist.
6. Check the console for debug output if errors occur.

### Video to Audio Conversion
1. Run the video-to-audio converter:
   ```bash
   python video_to_audio.py
   ```
2. Click "Select Video File" to choose a video (e.g., `.mp4`, `.avi`, `.mkv`).
3. Select an audio format (e.g., `mp3`, `wav`, `m4a`) from the dropdown.
4. Click "Convert" to create an audio file in the same directory (e.g., `video.mp4` becomes `video.mp3`).

## Testing the App

Try it with the included `sample.mp3` file or download free audio or video from [Freesound.org](https://freesound.org/) (e.g., search for "speech" or "video" under a Creative Commons license).

## Learning with Test Scripts

The `test` folder contains simple scripts to help beginners understand the code:
- `simple_transcription.py`: Transcribes an audio file to an English SRT file.
- `simple_translation.py`: Translates an English SRT file to Bangla.
- `translation_with_punctuation.py`: Translates an SRT file to Bangla with full-sentence detection.
- `tk_gui.py`: Shows the GUI design without functionality for learning Tkinter.

## How It Works

- **Transcription**: The app uses the AssemblyAI API to convert audio to English subtitles.
- **Translation**: If enabled, it uses the `deep_translator` library with Google Translate to convert English subtitles to Bangla. The code improves translation by:
  - Detecting sentence boundaries with punctuation.
  - Splitting text to align with original subtitle timings.
- **Video to Audio**: The `video_to_audio.py` script uses `moviepy` to extract audio from video files.
- **GUI**: Built with Tkinter, providing a user-friendly interface with status updates.

## Troubleshooting

- **Errors**: If you see errors like "Unexpected Error," check the console for a traceback. Update libraries with `pip install --upgrade assemblyai httpx deep_translator moviepy`.
- **API Key**: Ensure your API key is correctly set in `subtitle.py`.
- **File Issues**: Use supported audio/video formats and ensure write permissions.
- **FFmpeg**: Ensure FFmpeg is installed for video-to-audio conversion (see Prerequisites).
