import os
import assemblyai as aai
from deep_translator import GoogleTranslator
import re
import tkinter as tk
from tkinter import filedialog, messagebox

# Configuration
API_KEY = os.getenv('ASSEMBLYAI_API_KEY', 'YOUR_API_KEY_HERE')
aai.settings.api_key = API_KEY

# Translation Utilities

def ends_with_punctuation(text):
    """
    Check if the text ends with a punctuation mark (., !, ?, or …).

    Args:
        text (str): The input text to check.

    Returns:
        bool: True if the text ends with punctuation, False otherwise.
    """
    return bool(re.search(r'[.!?…]$', text.strip()))

def split_text(translated_text, proportions):
    """
    Split translated text into parts based on given proportions to maintain subtitle timing.

    Args:
        translated_text (str): The text to split.
        proportions (list): A list of proportions to divide the text.

    Returns:
        list: A list of text segments based on the proportions.
    """
    words = translated_text.split()
    total = len(words)
    if total == 0:
        return [''] * len(proportions)
    cum = 0
    splits = []
    indices = []
    for p in proportions[:-1]:
        cum += p
        indices.append(round(cum * total))
    start = 0
    for idx in indices:
        splits.append(' '.join(words[start:idx]))
        start = idx
    splits.append(' '.join(words[start:]))
    return splits

def translate_srt(input_path, output_path):
    """
    Translate an SRT subtitle file from English to Bangla.

    Args:
        input_path (str): Path to the input SRT file.
        output_path (str): Path to save the translated SRT file.
    """
    with open(input_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()
    translated_lines = []
    group = []
    buffer = ""
    for line in lines:
        strip = line.strip()
        if not strip:
            continue
        if strip.isdigit():
            if group and ends_with_punctuation(buffer):
                process_group(group, buffer, translated_lines)
                group, buffer = [], ""
            group.append([strip])
        elif '-->' in strip:
            group[-1].append(strip)
        else:
            buffer += strip + ' '
            group[-1].append(strip)
            if ends_with_punctuation(strip):
                process_group(group, buffer, translated_lines)
                group, buffer = [], ""
    if group:
        process_group(group, buffer, translated_lines)
    with open(output_path, 'w', encoding='utf-8') as f:
        for l in translated_lines:
            f.write(l + '\n')

def process_group(group, buffer, out_lines):
    """
    Process a group of subtitle lines and translate the text.

    Args:
        group (list): List of subtitle lines (number, timestamp, text).
        buffer (str): Accumulated text to translate.
        out_lines (list): List to store the translated lines.
    """
    if not group:
        return
    text = buffer.strip()
    trans = GoogleTranslator(source='auto', target='bn').translate(text)
    if len(group) == 1:
        block = group[0]
        out_lines.extend(block[:2] + [trans, ''])
        return
    lengths = [len(' '.join(b[2:])) for b in group]
    total_len = sum(lengths)
    proportions = [l/total_len for l in lengths]
    parts = split_text(trans, proportions)
    for i, b in enumerate(group):
        out_lines.extend(b[:2] + [parts[i], ''])

# Subtitle Generation
def transcribe_to_srt(audio_path, srt_path):
    """
    Convert an audio file into an SRT subtitle file using AssemblyAI.

    Args:
        audio_path (str): Path to the input audio file.
        srt_path (str): Path to save the generated SRT file.

    Raises:
        RuntimeError: If transcription fails due to an error from AssemblyAI.
    """
    config = aai.TranscriptionConfig(speech_model=aai.SpeechModel.best)
    transcriber = aai.Transcriber(config=config)
    transcript = transcriber.transcribe(audio_path)
    if transcript.status == 'error':
        raise RuntimeError(f"Transcription failed: {transcript.error}")
    srt = transcript.export_subtitles_srt()
    with open(srt_path, 'w', encoding='utf-8') as f:
        f.write(srt)
    return srt_path

# GUI Implementation
class SubtitleApp:
    def __init__(self, root):
        """
        Initialize the Subtitle Generator & Translator GUI.

        Args:
            root (tk.Tk): The root window for the Tkinter application.
        """
        self.root = root
        self.root.title("Subtitle Generator & Translator")
        self.root.configure(bg='#111111')  # Dark background for better visibility
        self.file_path = None

        # Button to select audio file
        self.select_btn = tk.Button(root, text="Select Audio File", command=self.select_file,
                                    bg='#333333', fg='white', activebackground='#555555')
        self.select_btn.pack(pady=10)

        # Label to display selected file name
        self.file_label = tk.Label(root, text="No file selected", bg='#111111', fg='white')
        self.file_label.pack(pady=5)

        # Checkbox to enable Bangla translation
        self.translate_var = tk.BooleanVar(value=False)
        self.toggle = tk.Checkbutton(root, text="Enable Bangla Translation", var=self.translate_var,
                                     bg='#111111', fg='white', selectcolor='#111111', activebackground='#111111')
        self.toggle.pack(pady=10)

        # Button to start the process
        self.run_btn = tk.Button(root, text="Run", command=self.run_process,
                                 bg='#444444', fg='white', activebackground='#666666')
        self.run_btn.pack(pady=20)

        # Status label to show processing steps
        self.status_label = tk.Label(root, text="", bg='#111111', fg='white')
        self.status_label.pack(pady=5)

    def select_file(self):
        """
        Open a file dialog to select an audio file and update the label with validation.
        """
        path = filedialog.askopenfilename(
            filetypes=[("Audio files", "*.mp3 *.wav *.m4a *.flac *.aac"), ("All files", "*")]
        )
        if path:
            # Check if the selected file is a supported audio format
            valid_extensions = ['.mp3', '.wav', '.m4a', '.flac', '.aac']
            if not any(path.lower().endswith(ext) for ext in valid_extensions):
                messagebox.showwarning("Invalid File", "Please select a supported audio file (.mp3, .wav, .m4a, .flac, .aac).")
                return
            self.file_path = path
            fname = os.path.basename(path)
            self.file_label.config(text=fname)

    def run_process(self):
        """
        Run the transcription and optional translation process with status updates.

        Checks for file selection, handles file overwrite prompts, and displays success/error messages.
        """
        if not self.file_path:
            messagebox.showwarning("No File", "Please select an audio file first.")
            return
        base, _ = os.path.splitext(self.file_path)
        eng_srt = f"{base}.srt"
        bn_srt = f"{base}_bangla.srt" if self.translate_var.get() else None

        if os.path.exists(eng_srt):
            if not messagebox.askyesno("File Exists", f"{eng_srt} already exists. Overwrite?"):
                return
        if bn_srt and os.path.exists(bn_srt):
            if not messagebox.askyesno("File Exists", f"{bn_srt} already exists. Overwrite?"):
                return

        self.run_btn.config(state='disabled', text="Processing...")
        self.status_label.config(text="Transcribing...")
        self.root.update()

        try:
            transcribe_to_srt(self.file_path, eng_srt)
            if self.translate_var.get():
                self.status_label.config(text="Translating...")
                self.root.update()
                translate_srt(eng_srt, bn_srt)
                messagebox.showinfo("Success", f"Generated:\n{eng_srt}\n{bn_srt}")
            else:
                messagebox.showinfo("Success", f"Generated: {eng_srt}")
        except aai.AssemblyAIError as e:
            messagebox.showerror("Transcription Error", f"AssemblyAI error: {str(e)}")
        except IOError as e:
            messagebox.showerror("File Error", f"File operation failed: {str(e)}")
        except Exception as e:
            import traceback
            traceback.print_exc()  # Print full traceback to console for debugging
            messagebox.showerror("Unexpected Error", f"An unexpected error occurred: {str(e)}")
        finally:
            self.run_btn.config(state='normal', text="Run")
            self.status_label.config(text="Done!")

if __name__ == '__main__':
    root = tk.Tk()
    app = SubtitleApp(root)
    root.mainloop()