from deep_translator import GoogleTranslator 

def translate_srt(input_path, output_path):
    """
    This function reads an SRT file, translates the text to Bangla, and saves the result.

    Args:
        input_path (str): Path to the input SRT file (e.g., 'sample.srt')
        output_path (str): Path where the translated SRT file will be saved (e.g., 'sample_bangla.srt')
    """
    # Step 1: Read the SRT file
    with open(input_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()  # Read all lines into a list

    translated_lines = []  # List to store the translated SRT content
    block = []  # Temporary list to hold lines of one subtitle block (number, timestamp, text)

    # Step 2: Process each line of the SRT file
    for line in lines:
        strip = line.strip()  # Remove extra spaces or newlines from the line

        # Check if the line is a number (like "1" or "2") indicating a new subtitle block
        if strip.isdigit() and block:

            # If we already have a block, translate its text
            text = ' '.join(block[2:])  # Combine all text lines (after timestamp) into one string

            # Translate from English ('en') to Bangla ('bn') using GoogleTranslator
            translated = GoogleTranslator(source='en', target='bn').translate(text)

            # Add the block’s number, timestamp, translated text, and an empty line to the output
            translated_lines.extend(block[:2] + [translated, ''])
            block = [strip]  # Start a new block with the current number

        elif '-->' in strip or strip:  # Check for timestamp (e.g., "00:00:01,000 --> 00:00:02,000") or text

            block.append(strip)  # Add the line to the current block

        elif not strip and block:  # Check for an empty line (end of a block)

            text = ' '.join(block[2:])  # Combine text lines
            translated = GoogleTranslator(source='en', target='bn').translate(text)
            translated_lines.extend(block[:2] + [translated, ''])
            block = []  # Reset the block for the next subtitle

    # Step 3: Handle the last block (if the file doesn’t end with an empty line)
    if block:
        text = ' '.join(block[2:])  # Combine text lines
        translated = GoogleTranslator(source='en', target='bn').translate(text)
        translated_lines.extend(block[:2] + [translated, ''])

    # Step 4: Save the translated SRT to a new file
    with open(output_path, 'w', encoding='utf-8') as f:
        f.write('\n'.join(translated_lines))

# Define the input and output file paths
input_srt = 'sample.srt'
output_srt = 'sample_bangla.srt'  # The translated Bangla SRT file

# Run the translation function
translate_srt(input_srt, output_srt)