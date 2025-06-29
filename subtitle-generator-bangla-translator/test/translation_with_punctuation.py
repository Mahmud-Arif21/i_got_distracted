# You may have noticed that the previous translation was full of wierd sentances.
# This script translates an English SRT subtitle file into Bangla by accumulating text until a full sentence
# is detected (ending with punctuation like '.', '!', '?', or '…'), then translating it as a whole.
# The translated text is split proportionally across the original subtitle blocks to maintain timing accuracy.

from deep_translator import GoogleTranslator
import re

def ends_with_punctuation(text):
    """
    Check if the text ends with a punctuation mark indicating a full sentence (., !, ?, or …).

    Args:
        text (str): The text to check.

    Returns:
        bool: True if text ends with punctuation, False otherwise.
    """
    return bool(re.search(r'[.!?…]$', text.strip()))

def split_text(translated_text, proportions):
    """
    Split the translated text into parts based on proportions of original text lengths.

    Args:
        translated_text (str): The translated sentence to split.
        proportions (list): Proportions based on original text lengths.

    Returns:
        list: Parts of the translated text for each subtitle block.
    """
    words = translated_text.split()  # Split into individual words
    total_words = len(words)
    if total_words == 0:
        return [''] * len(proportions)  # Handle empty text case
    cumulative = 0
    splits = []
    indices = []
    for p in proportions[:-1]:  # Exclude last proportion to handle remainder
        cumulative += p
        indices.append(round(cumulative * total_words))
    start = 0
    for idx in indices:
        splits.append(' '.join(words[start:idx]))
        start = idx
    splits.append(' '.join(words[start:]))  # Add remaining words
    return splits

def translate_srt_with_punctuation(input_path, output_path):
    """
    Translate an SRT file from English to Bangla, processing full sentences for better accuracy.

    Args:
        input_path (str): Path to the input English SRT file.
        output_path (str): Path to save the translated Bangla SRT file.
    """
    with open(input_path, 'r', encoding='utf-8') as f:
        lines = f.readlines()  # Read the entire SRT file

    translated_lines = []  # Store the final translated SRT content
    group = []  # List of subtitle blocks forming a sentence
    buffer = ""  # Accumulated text of the sentence

    for line in lines:
        stripped_line = line.strip()
        if not stripped_line:
            continue  # Skip empty lines between blocks

        if stripped_line.isdigit():
            # New subtitle block starts with a number
            if group and ends_with_punctuation(buffer):
                # Process accumulated blocks when a sentence ends
                process_group(group, buffer, translated_lines)
                group = []
                buffer = ""
            group.append([stripped_line])  # Start new block with block number
        elif '-->' in stripped_line:
            group[-1].append(stripped_line)  # Add timestamp to current block
        else:
            buffer += stripped_line + ' '  # Accumulate text for the sentence
            group[-1].append(stripped_line)  # Add text to current block
            if ends_with_punctuation(stripped_line):
                # Sentence complete, process the group
                process_group(group, buffer, translated_lines)
                group = []
                buffer = ""

    # Process any remaining group at the end of the file
    if group:
        process_group(group, buffer, translated_lines)

    # Write translated content to output file
    with open(output_path, 'w', encoding='utf-8') as f:
        for line in translated_lines:
            f.write(line + '\n')

def process_group(group, buffer, out_lines):
    """
    Translate the accumulated sentence and distribute it across subtitle blocks.

    Args:
        group (list): List of subtitle blocks forming a sentence.
        buffer (str): Full accumulated text of the sentence.
        out_lines (list): List to append translated SRT lines.
    """
    if not group:
        return

    full_text = buffer.strip()  # The complete sentence
    translated = GoogleTranslator(source='auto', target='bn').translate(full_text)

    if len(group) == 1:
        # Single block: assign translation directly
        block = group[0]
        out_lines.extend(block[:2] + [translated, ''])  # Number, timestamp, text, empty line
    else:
        # Multiple blocks: split translation proportionally
        lengths = [len(' '.join(block[2:])) for block in group]  # Length of each block's text
        total_length = sum(lengths)
        proportions = [length / total_length for length in lengths]
        parts = split_text(translated, proportions)
        for i, block in enumerate(group):
            out_lines.extend(block[:2] + [parts[i], ''])  # Assign split parts to blocks

input_srt = 'sample.srt'
output_srt = 'sample_bangla_punct.srt'
translate_srt_with_punctuation(input_srt, output_srt)