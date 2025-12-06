const express = require('express');
const { body, validationResult } = require('express-validator');
const { PrismaClient } = require('@prisma/client');
const { generateBMOResponse } = require('../services/aiService');

const router = express.Router();
const prisma = new PrismaClient();

// Send message to BMO
router.post('/message', [
  body('message').notEmpty().withMessage('Message is required'),
  body('conversationId').optional().isUUID().withMessage('Invalid conversation ID')
], async (req, res) => {
  try {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
      return res.status(400).json({ errors: errors.array() });
    }

    const { message, conversationId } = req.body;

    // Create or get conversation
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
      // Create new conversation
      conversation = await prisma.conversation.create({
        data: {
          title: message.substring(0, 50) + (message.length > 50 ? '...' : '')
        },
        include: { messages: true }
      });
    }

    // Save user message
    const userMessage = await prisma.message.create({
      data: {
        conversationId: conversation.id,
        role: 'USER',
        content: message
      }
    });

    // Generate BMO's response
    const bmoResponse = await generateBMOResponse(message, conversation.messages);

    // Save BMO's response
    const assistantMessage = await prisma.message.create({
      data: {
        conversationId: conversation.id,
        role: 'ASSISTANT',
        content: bmoResponse
      }
    });

    res.json({
      conversationId: conversation.id,
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
    console.error('Chat error:', error);
    res.status(500).json({ 
      error: 'Failed to process message',
      message: process.env.NODE_ENV === 'development' ? error.message : 'Something went wrong'
    });
  }
});

// Get conversation history
router.get('/conversation/:id', async (req, res) => {
  try {
    const { id } = req.params;

    const conversation = await prisma.conversation.findUnique({
      where: { id },
      include: {
        messages: {
          orderBy: { timestamp: 'asc' }
        }
      }
    });

    if (!conversation) {
      return res.status(404).json({ error: 'Conversation not found' });
    }

    res.json(conversation);
  } catch (error) {
    console.error('Get conversation error:', error);
    res.status(500).json({ error: 'Failed to get conversation' });
  }
});

module.exports = router;
