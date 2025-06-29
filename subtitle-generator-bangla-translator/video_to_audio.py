# This script creates a GUI to convert video files to audio files.
# Users can select a video, choose an audio format, and convert it.

import tkinter as tk  # For creating the GUI
from tkinter import filedialog, messagebox, ttk  # For file dialog, messages, and dropdown
import moviepy as mp  # For video-to-audio conversion
import os  # For file path operations

# Create the main GUI window
root = tk.Tk()
root.title("Video to Audio Converter")  # Set window title
root.configure(bg='#f0f0f0')  # Light gray background for the window

# Variable to store the selected video file path
video_path = ''

# Supported audio formats for the dropdown
audio_formats = ['mp3', 'wav', 'm4a']

def select_video():
    """
    Open a file dialog to select a video file and update the label.
    Restricts selection to common video formats.
    """
    global video_path
    # Open file dialog with video file filters
    path = filedialog.askopenfilename(
        filetypes=[("Video files", "*.mp4 *.avi *.mkv *.mov *.wmv"), ("All files", "*")]
    )
    if path:
        # Check if the file has a valid video extension
        valid_extensions = ['.mp4', '.avi', '.mkv', '.mov', '.wmv']
        if not any(path.lower().endswith(ext) for ext in valid_extensions):
            messagebox.showwarning("Invalid File", "Please select a valid video file (.mp4, .avi, .mkv, .mov, .wmv).")
            return
        video_path = path
        # Update label with the file name (last part of the path)
        file_label.config(text=os.path.basename(path))

def convert_to_audio():
    """
    Convert the selected video to an audio file in the chosen format.
    Saves the audio file in the same directory as the video.
    """
    if not video_path:
        messagebox.showwarning("No File", "Please select a video file first.")
        return

    # Get the selected audio format from the dropdown
    audio_format = format_var.get()
    if not audio_format:
        messagebox.showwarning("No Format", "Please select an audio format.")
        return

    # Create output audio file path by replacing video extension with audio extension
    base, _ = os.path.splitext(video_path)
    output_audio = f"{base}.{audio_format}"

    # Show processing message and disable the convert button
    convert_btn.config(state='disabled', text="Converting...")
    status_label.config(text="Converting to audio...")
    root.update()

    try:
        # Load the video file
        video = mp.VideoFileClip(video_path)
        # Extract audio and save it in the chosen format
        video.audio.write_audiofile(output_audio)
        # Close the video file to free resources
        video.close()
        # Show success message
        messagebox.showinfo("Success", f"Audio saved as: {output_audio}")
        status_label.config(text="Done!")
    except Exception as e:
        # Show error message if conversion fails
        messagebox.showerror("Conversion Error", f"Failed to convert: {str(e)}")
        status_label.config(text="Error occurred")
    finally:
        # Re-enable the convert button
        convert_btn.config(state='normal', text="Convert")

# GUI Elements
# Label to display the selected file name
file_label = tk.Label(
    root,
    text="No video selected",
    bg='#f0f0f0',
    fg='#333333'
)
file_label.pack(pady=10)

# Button to select a video file
select_btn = tk.Button(
    root,
    text="Select Video File",
    command=select_video,
    bg='#4CAF50',  # Green background
    fg='white',  # White text
    activebackground='#45a049'  # Slightly darker green when clicked
)
select_btn.pack(pady=5)

# Dropdown menu for selecting audio format
format_var = tk.StringVar(value=audio_formats[0])  # Default to first format (mp3)
format_label = tk.Label(
    root,
    text="Select Audio Format:",
    bg='#f0f0f0',
    fg='#333333'
)
format_label.pack(pady=5)
format_dropdown = ttk.Combobox(
    root,
    textvariable=format_var,
    values=audio_formats,
    state='readonly',  # Prevent typing in dropdown
    width=10
)
format_dropdown.pack(pady=5)

# Button to start conversion
convert_btn = tk.Button(
    root,
    text="Convert",
    command=convert_to_audio,
    bg='#2196F3',  # Blue background
    fg='white',  # White text
    activebackground='#1e88e5'  # Slightly darker blue when clicked
)
convert_btn.pack(pady=10)

# Label to show conversion status
status_label = tk.Label(
    root,
    text="",
    bg='#f0f0f0',
    fg='#333333'
)
status_label.pack(pady=5)

# Start the GUI
root.mainloop()