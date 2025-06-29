import assemblyai as aai
import os

# Set up your API key
API_KEY = 'YOUR_API_KEY_HERE'  # Replace this with your actual key
aai.settings.api_key = API_KEY  # Tell the AssemblyAI library to use this key

def transcribe_to_srt(audio_path, srt_path):
    """
    This function takes an audio file and converts it into an SRT subtitle file.

    Args:
        audio_path (str): The path to your audio file (e.g., 'sample.mp3')
        srt_path (str): The path where the SRT file will be saved (e.g., 'sample.srt')
    """
    # Step 1: Set up transcription settings
    # We’re using the default settings (e.g., auto-detect language).
    config = aai.TranscriptionConfig()

    # Step 2: Create a transcriber object
    # This object will handle the actual transcription process using our settings.
    transcriber = aai.Transcriber(config=config)

    # Step 3: Send the audio file to AssemblyAI and get the transcription (text with timestamps)
    transcript = transcriber.transcribe(audio_path)

    # Step 4: Convert the transcription to SRT format
    srt = transcript.export_subtitles_srt()

    # Step 5: Save the SRT text to a file
    with open(srt_path, 'w', encoding='utf-8') as f:
        f.write(srt)  # Write the SRT text to the file

    # Print confirmation message with the file path
    print(f"SRT saved to {srt_path}")  

# Define the input and output file paths
audio_file = os.path.join('..', 'sample.mp3')  # The audio file to transcribe
output_srt = 'sample.srt'  # The name of the SRT file

# Run the transcription function with the file paths
transcribe_to_srt(audio_file, output_srt)