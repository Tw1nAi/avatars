import argparse
import librosa
import soundfile as sf
import os

def resample_audio(input_file, output_file, in_rate, out_rate):
    # Load audio file with the specified original sampling rate
    audio, sr = librosa.load(input_file, sr=in_rate)

    # Resample the audio to the target sampling rate
    audio_resampled = librosa.resample(audio, orig_sr=sr, target_sr=out_rate)

    # Save the resampled audio file
    sf.write(output_file, audio_resampled, out_rate)

def process_directory(input_dir, output_dir, in_rate, out_rate):
    # Ensure output directory exists
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    # Process each .wav file in the input directory
    for filename in os.listdir(input_dir):
        if filename.endswith(".wav"):
            input_file = os.path.join(input_dir, filename)
            output_file = os.path.join(output_dir, filename)
            resample_audio(input_file, output_file, in_rate, out_rate)
            print(f"Processed {filename}")

def main():
    # Set up the argument parser
    parser = argparse.ArgumentParser(description="Batch resample WAV audio files from a specified original to a target sampling rate.")
    parser.add_argument('-i', '--input_dir', required=True, help='Path to the directory containing input WAV files.')
    parser.add_argument('-o', '--output_dir', required=True, help='Path to the directory where output WAV files will be saved.')
    parser.add_argument('--in_rate', type=int, default=44100, help='Original sampling rate of the input files (default: 44100 Hz).')
    parser.add_argument('--out_rate', type=int, default=16000, help='Target sampling rate for the output files (default: 16000 Hz).')

    # Parse arguments
    args = parser.parse_args()

    # Process all .wav files in the directory
    process_directory(args.input_dir, args.output_dir, args.in_rate, args.out_rate)

if __name__ == "__main__":
    main()
