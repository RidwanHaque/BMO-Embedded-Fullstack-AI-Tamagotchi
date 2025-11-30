const OpenAI = require('openai');
const fs = require('fs');
const path = require('path');
const os = require('os');

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

/**
 * Convert PCM audio buffer to text using OpenAI Whisper API
 * @param {Buffer} audioBuffer - Raw PCM audio data (16-bit, mono, 16kHz expected)
 * @param {string} format - Audio format ('pcm' or 'wav')
 * @returns {Promise<string>} - Transcribed text
 */
async function transcribeAudio(audioBuffer, format = 'pcm') {
  try {
    // OpenAI Whisper expects audio files, so we need to create a temporary file
    // For PCM, we'll create a simple WAV header and prepend it
    const tempDir = os.tmpdir();
    const tempFilePath = path.join(tempDir, `bmo-voice-${Date.now()}.${format === 'pcm' ? 'wav' : format}`);
    
    let audioFile;
    if (format === 'pcm') {
      // Create a simple WAV file from PCM data
      // Assumes: 16-bit, mono, 16000 Hz
      const sampleRate = 16000;
      const numChannels = 1;
      const bitsPerSample = 16;
      const dataSize = audioBuffer.length;
      const fileSize = 36 + dataSize;
      
      const wavHeader = Buffer.alloc(44);
      // RIFF header
      wavHeader.write('RIFF', 0);
      wavHeader.writeUInt32LE(fileSize - 8, 4);
      wavHeader.write('WAVE', 8);
      // fmt chunk
      wavHeader.write('fmt ', 12);
      wavHeader.writeUInt32LE(16, 16); // fmt chunk size
      wavHeader.writeUInt16LE(1, 20); // audio format (PCM)
      wavHeader.writeUInt16LE(numChannels, 22);
      wavHeader.writeUInt32LE(sampleRate, 24);
      wavHeader.writeUInt32LE(sampleRate * numChannels * (bitsPerSample / 8), 28); // byte rate
      wavHeader.writeUInt16LE(numChannels * (bitsPerSample / 8), 32); // block align
      wavHeader.writeUInt16LE(bitsPerSample, 34);
      // data chunk
      wavHeader.write('data', 36);
      wavHeader.writeUInt32LE(dataSize, 40);
      
      audioFile = Buffer.concat([wavHeader, audioBuffer]);
    } else {
      audioFile = audioBuffer;
    }
    
    // Write to temp file
    fs.writeFileSync(tempFilePath, audioFile);
    
    // Transcribe using OpenAI Whisper
    const transcription = await openai.audio.transcriptions.create({
      file: fs.createReadStream(tempFilePath),
      model: 'whisper-1',
      language: 'en', // Optional: specify language for better accuracy
      response_format: 'text'
    });
    
    // Clean up temp file
    fs.unlinkSync(tempFilePath);
    
    return typeof transcription === 'string' ? transcription.trim() : transcription.text?.trim() || '';
    
  } catch (error) {
    console.error('Speech-to-text error:', error);
    
    // Clean up temp file if it exists
    const tempDir = os.tmpdir();
    const tempFiles = fs.readdirSync(tempDir).filter(f => f.startsWith('bmo-voice-'));
    tempFiles.forEach(f => {
      try {
        fs.unlinkSync(path.join(tempDir, f));
      } catch (e) {
        // Ignore cleanup errors
      }
    });
    
    throw new Error(`Failed to transcribe audio: ${error.message}`);
  }
}

module.exports = {
  transcribeAudio
};

