const express = require('express');
const { PrismaClient } = require('@prisma/client');
const { transcribeAudio } = require('../services/speechService');
const { generateBMOResponse } = require('../services/aiService');

const router = express.Router();
const prisma = new PrismaClient();

// Voice input endpoint - accepts PCM audio from ESP32
router.post('/input', async (req, res) => {
  try {
    // Get conversationId from query params or headers (ESP32 can send it)
    const conversationId = req.query.conversationId || req.headers['x-conversation-id'] || null;
    
    // Get audio data from request body (raw binary PCM)
    const audioBuffer = req.body;
    
    if (!audioBuffer || audioBuffer.length === 0) {
      return res.status(400).json({ error: 'No audio data received' });
    }
    
    // Validate audio size (max 10 seconds at 16kHz, 16-bit mono = ~320KB)
    const maxSize = 320000; // ~10 seconds
    if (audioBuffer.length > maxSize) {
      return res.status(400).json({ error: 'Audio data too large (max 10 seconds)' });
    }
    
    console.log(`🎤 Received voice input: ${audioBuffer.length} bytes from ESP32`);
    
    // Step 1: Convert audio to text using Whisper
    const transcribedText = await transcribeAudio(audioBuffer, 'pcm');
    
    if (!transcribedText || transcribedText.trim().length === 0) {
      return res.status(400).json({ 
        error: 'Could not transcribe audio. Please try again.',
        transcribedText: null
      });
    }
    
    console.log(`📝 Transcribed: "${transcribedText}"`);
    
    // Step 2: Create or get conversation (same logic as chatRoutes)
    let conversation;
    if (conversationId) {
      conversation = await prisma.conversation.findUnique({
        where: { id: conversationId },
        include: { messages: true }
      });
      
      if (!conversation) {
        return res.status(404).json({ error: 'Conversation not found' });
      }
    } else {
      // Create new conversation with transcribed text as title
      conversation = await prisma.conversation.create({
        data: {
          title: transcribedText.substring(0, 50) + (transcribedText.length > 50 ? '...' : '')
        },
        include: { messages: true }
      });
    }
    
    // Step 3: Save transcribed message as USER message
    const userMessage = await prisma.message.create({
      data: {
        conversationId: conversation.id,
        role: 'USER',
        content: transcribedText
      }
    });
    
    // Step 4: Generate BMO's response using existing chat logic
    const bmoResponse = await generateBMOResponse(transcribedText, conversation.messages);
    
    // Step 5: Save BMO's response
    const assistantMessage = await prisma.message.create({
      data: {
        conversationId: conversation.id,
        role: 'ASSISTANT',
        content: bmoResponse
      }
    });
    
    // Step 6: Return response (ESP32 can display this on uLCD)
    res.json({
      success: true,
      conversationId: conversation.id,
      transcribedText: transcribedText,
      userMessage: {
        id: userMessage.id,
        content: userMessage.content,
        timestamp: userMessage.timestamp
      },
      bmoResponse: {
        id: assistantMessage.id,
        content: assistantMessage.content,
        timestamp: assistantMessage.timestamp
      }
    });
    
  } catch (error) {
    console.error('Voice input error:', error);
    res.status(500).json({ 
      error: 'Failed to process voice input',
      message: process.env.NODE_ENV === 'development' ? error.message : 'Something went wrong'
    });
  }
});

module.exports = router;

