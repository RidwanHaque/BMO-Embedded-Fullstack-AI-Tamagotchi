const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const { createServer } = require('http');
const { Server } = require('socket.io');
require('dotenv').config();

const chatRoutes = require('./routes/chatRoutes');
const conversationRoutes = require('./routes/conversationRoutes');
const voiceRoutes = require('./routes/voiceRoutes');
const { initializeSocket } = require('./services/socketService');

const app = express();
const server = createServer(app);
const io = new Server(server, {
  cors: {
    origin: process.env.NODE_ENV === 'production' ? false : ['http://localhost:3000'],
    methods: ['GET', 'POST']
  }
});

const PORT = process.env.PORT || 3001;

// Middleware
app.use(helmet());
app.use(cors({
  origin: process.env.NODE_ENV === 'production' 
    ? false 
    : ['http://localhost:3000', /^http:\/\/192\.168\.\d+\.\d+:3001$/, /^http:\/\/10\.\d+\.\d+\.\d+:3001$/],
  credentials: true
}));
app.use(express.json({ limit: '10mb' }));
app.use(express.urlencoded({ extended: true }));

// Raw body parser for voice audio (PCM binary data)
// Apply only to /api/voice routes
app.use('/api/voice', express.raw({ 
  type: 'application/octet-stream',
  limit: '500kb' // ~15 seconds of 16kHz 16-bit mono
}));

// Routes
app.use('/api/chat', chatRoutes);
app.use('/api/conversations', conversationRoutes);
app.use('/api/voice', voiceRoutes);

// Health check endpoint
app.get('/api/health', (req, res) => {
  res.json({ 
    status: 'healthy', 
    timestamp: new Date().toISOString(),
    service: 'BMO Chatbot API'
  });
});

// Initialize Socket.IO
initializeSocket(io);

// Error handling middleware
app.use((err, req, res, next) => {
  console.error('Error:', err);
  res.status(500).json({ 
    error: 'Internal server error',
    message: process.env.NODE_ENV === 'development' ? err.message : 'Something went wrong'
  });
});

// 404 handler
app.use('*', (req, res) => {
  res.status(404).json({ error: 'Route not found' });
});

server.listen(PORT, '0.0.0.0', () => {
  console.log(`🤖 BMO Backend running on port ${PORT}`);
  console.log(`📡 Socket.IO server initialized`);
  console.log(`🎤 Voice API endpoint: POST /api/voice/input`);
  console.log(`🌍 Environment: ${process.env.NODE_ENV || 'development'}`);
  console.log(`💡 Listening on all interfaces (0.0.0.0) for ESP32 access`);
});
