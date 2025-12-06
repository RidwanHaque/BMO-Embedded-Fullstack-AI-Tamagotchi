const { PrismaClient } = require('@prisma/client');

const prisma = new PrismaClient();

function initializeSocket(io) {
  io.on('connection', (socket) => {
    console.log(`🔌 User connected: ${socket.id}`);

    // Join conversation room
    socket.on('join-conversation', (conversationId) => {
      socket.join(conversationId);
      console.log(`👥 User ${socket.id} joined conversation ${conversationId}`);
    });

    // Leave conversation room
    socket.on('leave-conversation', (conversationId) => {
      socket.leave(conversationId);
      console.log(`👋 User ${socket.id} left conversation ${conversationId}`);
    });

    // Handle typing indicators
    socket.on('typing-start', (data) => {
      socket.to(data.conversationId).emit('user-typing', {
        userId: socket.id,
        isTyping: true
      });
    });

    socket.on('typing-stop', (data) => {
      socket.to(data.conversationId).emit('user-typing', {
        userId: socket.id,
        isTyping: false
      });
    });

    // Handle disconnection
    socket.on('disconnect', () => {
      console.log(`🔌 User disconnected: ${socket.id}`);
    });
  });
}

// Helper function to broadcast message to conversation
function broadcastToConversation(io, conversationId, event, data) {
  io.to(conversationId).emit(event, data);
}

module.exports = {
  initializeSocket,
  broadcastToConversation
};
