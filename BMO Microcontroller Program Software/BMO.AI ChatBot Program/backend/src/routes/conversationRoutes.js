const express = require('express');
const { PrismaClient } = require('@prisma/client');

const router = express.Router();
const prisma = new PrismaClient();

// Get all conversations
router.get('/', async (req, res) => {
  try {
    const conversations = await prisma.conversation.findMany({
      include: {
        messages: {
          orderBy: { timestamp: 'desc' },
          take: 1
        },
        _count: {
          select: { messages: true }
        }
      },
      orderBy: { updatedAt: 'desc' }
    });

    res.json(conversations);
  } catch (error) {
    console.error('Get conversations error:', error);
    res.status(500).json({ error: 'Failed to get conversations' });
  }
});

// Create new conversation
router.post('/', async (req, res) => {
  try {
    const { title } = req.body;

    const conversation = await prisma.conversation.create({
      data: {
        title: title || 'New Chat with BMO'
      }
    });

    res.status(201).json(conversation);
  } catch (error) {
    console.error('Create conversation error:', error);
    res.status(500).json({ error: 'Failed to create conversation' });
  }
});

// Delete conversation
router.delete('/:id', async (req, res) => {
  try {
    const { id } = req.params;

    await prisma.conversation.delete({
      where: { id }
    });

    res.json({ message: 'Conversation deleted successfully' });
  } catch (error) {
    console.error('Delete conversation error:', error);
    res.status(500).json({ error: 'Failed to delete conversation' });
  }
});

// Update conversation title
router.patch('/:id', async (req, res) => {
  try {
    const { id } = req.params;
    const { title } = req.body;

    const conversation = await prisma.conversation.update({
      where: { id },
      data: { title }
    });

    res.json(conversation);
  } catch (error) {
    console.error('Update conversation error:', error);
    res.status(500).json({ error: 'Failed to update conversation' });
  }
});

module.exports = router;
