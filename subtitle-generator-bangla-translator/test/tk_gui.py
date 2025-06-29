# This script creates a non-functional GUI to show the layout and design of the Subtitle Generator & Translator.
# It has no functionality (buttons and checkboxes do nothing) but lets you tweak the visual design.

import tkinter as tk

# Create the main window
root = tk.Tk()
root.title("Subtitle Generator & Translator")  # Set the window title
root.configure(bg='#111111')  # Set a dark background color for the window

# Add a button to represent selecting an audio file
# This button does nothing but shows the style (colors, padding)
select_btn = tk.Button(
    root,
    text="Select Audio File",  # Button text
    bg='#333333',  # Dark gray background
    fg='white',  # White text
    activebackground='#555555'  # Slightly lighter gray when clicked
)
select_btn.pack(pady=10)  # Add button to window with 10 pixels of vertical padding

# Add a label to show the selected file name
# This is just a placeholder to mimic the file name display
file_label = tk.Label(
    root,
    text="No file selected",  # Default text
    bg='#111111',  # Match window background
    fg='white'  # White text for contrast
)
file_label.pack(pady=5)  # Add label with 5 pixels of vertical padding

# Add a checkbox to represent enabling Bangla translation
# This checkbox does nothing but shows the design
translate_var = tk.BooleanVar(value=False)  # Variable to store checkbox state (not used)
toggle = tk.Checkbutton(
    root,
    text="Enable Bangla Translation",  # Checkbox text
    variable=translate_var,  # Link to variable (not used here)
    bg='#111111',  # Match window background
    fg='white',  # White text
    selectcolor='#111111',  # Background when checked
    activebackground='#111111'  # Background when clicked
)
toggle.pack(pady=10)  # Add checkbox with 10 pixels of padding

# Add a button to represent starting the process
# This button does nothing but shows the style
run_btn = tk.Button(
    root,
    text="Run",  # Button text
    bg='#444444',  # Medium gray background
    fg='white',  # White text
    activebackground='#666666'  # Lighter gray when clicked
)
run_btn.pack(pady=20)  # Add button with 20 pixels of padding

# Add a status label to show processing messages
# This is just a placeholder to mimic status updates
status_label = tk.Label(
    root,
    text="",  # Empty by default (would show "Transcribing...", etc.)
    bg='#111111',  # Match window background
    fg='white'  # White text
)
status_label.pack(pady=5)  # Add label with 5 pixels of padding

# Start the GUI and keep the window open
root.mainloop()