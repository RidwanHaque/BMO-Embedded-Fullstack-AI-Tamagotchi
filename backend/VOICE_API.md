# BMO Voice API Integration Guide

## Overview
The `/api/voice/input` endpoint accepts raw PCM audio from the ESP32, transcribes it using OpenAI Whisper, and processes it through the existing BMO chat pipeline.

## Endpoint
```
POST /api/voice/input
```

## Request Format

### Headers
```
Content-Type: application/octet-stream
X-Conversation-ID: <optional-uuid>  // If continuing an existing conversation
```

### Query Parameters (alternative to header)
```
?conversationId=<uuid>  // Optional: continue existing conversation
```

### Body
- **Format**: Raw binary PCM audio data
- **Specs**: 
  - 16-bit signed integer
  - Mono channel
  - 16 kHz sample rate
  - Little-endian byte order
- **Max Size**: ~500KB (~15 seconds of audio)

## Response Format

### Success (200 OK)
```json
{
  "success": true,
  "conversationId": "uuid-string",
  "transcribedText": "Hello BMO, how are you?",
  "userMessage": {
    "id": "message-uuid",
    "content": "Hello BMO, how are you?",
    "timestamp": "2025-11-24T15:30:00.000Z"
  },
  "bmoResponse": {
    "id": "message-uuid",
    "content": "Hei! I'm doing great! How about you?",
    "timestamp": "2025-11-24T15:30:01.500Z"
  }
}
```

### Error (400/404/500)
```json
{
  "error": "Error message",
  "message": "Detailed error (dev mode only)"
}
```

## ESP32 Integration Example

### Arduino/ESP-IDF HTTP Client
```cpp
// After recording PCM audio into a buffer (e.g., uint8_t audioBuffer[])
// POST to: http://<YOUR_PC_IP>:3001/api/voice/input

HTTPClient http;
http.begin("http://192.168.1.100:3001/api/voice/input");
http.addHeader("Content-Type", "application/octet-stream");
// Optional: http.addHeader("X-Conversation-ID", conversationIdString);

int httpResponseCode = http.POST((uint8_t*)audioBuffer, audioBufferSize);

if (httpResponseCode == 200) {
  String response = http.getString();
  // Parse JSON to get bmoResponse.content
  // Display on uLCD or speak via TTS
} else {
  Serial.printf("Voice API error: %d\n", httpResponseCode);
}

http.end();
```

## Notes

1. **Network Access**: The backend listens on `0.0.0.0:3001` to accept connections from your LAN (not just localhost).

2. **CORS**: CORS is configured to allow requests from your ESP32's IP range (192.168.x.x, 10.x.x.x).

3. **Conversation Continuity**: 
   - If you send `conversationId`, the voice message is added to that conversation.
   - If omitted, a new conversation is created.

4. **Audio Quality**: 
   - Better audio quality = better transcription accuracy
   - Ensure your SPW2430 mic is properly calibrated
   - Consider noise reduction/AGC if possible

5. **Latency**: 
   - STT processing: ~1-3 seconds
   - BMO response generation: ~1-2 seconds
   - Total: ~2-5 seconds typical

## Testing with cURL

```bash
# Record audio to PCM file first, then:
curl -X POST http://localhost:3001/api/voice/input \
  -H "Content-Type: application/octet-stream" \
  --data-binary @audio.pcm

# With conversation ID:
curl -X POST "http://localhost:3001/api/voice/input?conversationId=<uuid>" \
  -H "Content-Type: application/octet-stream" \
  --data-binary @audio.pcm
```

