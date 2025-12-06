# BMO Chatbot Project

A full-stack chatbot and embedded companion system featuring BMO from Adventure Time, built with modern web technologies and ESP32-based hardware.

![BMO Tamagotchi setup](BMOImage.JPG)

## Embedded BMO Hardware Companion

In addition to the web chatbot, this project includes a physical BMO-inspired Tamagotchi device built around an ESP32-C6 microcontroller and a secondary ESP32 board with an IMU sensor. The device acts as a small digital companion that displays emotions, plays games, reacts to user input, and manages virtual stats such as happiness and energy using a combination of display graphics, audio feedback, and sensor events.

The main ESP32-C6 drives a 4D Systems uLCD 144 G2 screen that renders BMO’s face, menus, stat bars, and a Snake minigame. A 5-way navigation switch is used for menu navigation and gameplay; a common-anode RGB LED reflects BMO’s emotional state; and a piezo speaker generates sound effects and melodies via the ESP32 LEDC PWM hardware. A microphone module provides basic voice or noise detection that can trigger animations or chatbot interaction.

A secondary ESP32 board with an IMU acts as a wireless accessory over ESP-NOW: shaking the accessory sends a “happiness boost” event to the main device. The system also implements happiness and energy stat decay, a real-time clock synchronized via NTP, a low-power mode, and a complete Snake game loop with a color display, sound, and wireless events.

## ESP32–Web Chatbot Integration

The physical BMO device acts as a voice-enabled front-end to the full-stack web chatbot. Both the hardware and the browser UI talk to the same backend API and database, so all interactions share a single conversation history.

### System Architecture

ESP32 BMO Device → WiFi → Backend `/api/voice/input` → OpenAI Whisper + Chat Logic → PostgreSQL (Prisma) → Response → ESP32 Display

- The browser UI sends text messages to `/api/chat`.  
- The ESP32 sends audio to `/api/voice/input`.  
- Both flows run through the same chat pipeline and store messages in the same conversations table.

### Voice Workflow from ESP32

1. The user presses a button on the ESP32 BMO device; the microphone (SPW2430) records a short PCM audio buffer.
2. The ESP32 formats this audio and sends an HTTP `POST` request over WiFi to `http://<PC_IP>:3001/api/voice/input`.
3. The backend receives the raw audio, uses OpenAI Whisper to transcribe it to text, and creates a new “user message” in the Prisma/PostgreSQL database.
4. The existing chat service generates a reply using the BMO personality prompt (same logic used for `POST /api/chat` from the web UI).
5. The reply is stored in the conversation history and returned to the ESP32 as JSON.
6. The ESP32 parses the response and renders the text and related UI state on the uLCD screen, optionally updating LED color, sounds, or animations based on the reply.

### Shared State and Conversation Continuity

- Both web and hardware clients identify a conversation (e.g., by `conversationId` in the request), so messages from the ESP32 and the browser append to the same thread.
- The backend enforces all personality and behavior rules centrally; the microcontroller only handles transport, display, sensors, and local game/LED logic.
- This separation lets the ESP32 focus on hardware tasks (display, input, IMU events, low-power modes) while the Node.js backend handles AI, persistence, and multi-client coordination.


## 🎮 Features

- **BMO Personality**: AI-powered responses with BMO's quirky, helpful personality
- **Real-time Chat**: WebSocket-based instant messaging
- **Conversation History**: Persistent chat history in PostgreSQL
- **Responsive UI**: Beautiful BMO-themed interface
- **REST API**: Well-structured backend API
- **Dockerized**: Complete containerization for easy setup

## 🛠️ Tech Stack

### Backend
- **Node.js** with Express.js
- **PostgreSQL** database
- **Prisma** ORM
- **Socket.io** for real-time communication
- **OpenAI API** for AI responses

### Frontend
- **React** with TypeScript
- **Tailwind CSS** for styling
- **Socket.io Client** for real-time chat
- **Axios** for API calls

### DevOps
- **Docker** & Docker Compose
- **Environment variables** for configuration

## 🚀 Quick Start

### Prerequisites
- Docker and Docker Compose installed
- OpenAI API key

### Setup

1. **Clone and navigate to the project:**
   ```bash
   cd "C:\Users\rhaque34\Documents\Junior Year Class Files\Cursor\Cursor Project 1"
   ```

2. **Create environment file:**
   ```bash
   cp .env.example .env
   ```
   Add your OpenAI API key to `.env`:
   ```
   OPENAI_API_KEY=your_openai_api_key_here
   ```

3. **Start the application:**
   ```bash
   docker-compose up --build
   ```

4. **Access the application:**
   - Frontend: http://localhost:3000
   - Backend API: http://localhost:3001
   - Database: localhost:5432

## 📁 Project Structure

```
bmo-chatbot/
├── backend/                 # Node.js/Express API
│   ├── src/
│   │   ├── controllers/     # Route handlers
│   │   ├── models/         # Database models
│   │   ├── routes/         # API routes
│   │   ├── services/       # Business logic
│   │   └── utils/          # Helper functions
│   ├── prisma/             # Database schema
│   └── Dockerfile
├── frontend/               # React application
│   ├── src/
│   │   ├── components/     # React components
│   │   ├── pages/          # Page components
│   │   ├── services/       # API services
│   │   └── utils/          # Helper functions
│   └── Dockerfile
├── database/               # Database initialization
├── docker-compose.yml      # Container orchestration
└── README.md
```

## 🎯 Learning Objectives

This project covers:
- **System Design**: Microservices architecture, API design
- **REST APIs**: CRUD operations, authentication, error handling
- **Backend Frameworks**: Express.js, middleware, routing
- **SQL Databases**: PostgreSQL, Prisma ORM, migrations
- **GenAI**: OpenAI API integration, prompt engineering
- **Full Stack**: Frontend-backend communication, real-time features
- **DevOps**: Docker, containerization, environment management

## 🤖 BMO Personality

BMO responds with:
- Childlike enthusiasm and curiosity
- Gaming references and tech-savvy advice
- Helpful problem-solving attitude
- Occasional Finnish phrases (like in the show)
- Encouragement and positivity

## 🔧 Development

### Backend Development
```bash
cd backend
npm install
npm run dev
```

### Frontend Development
```bash
cd frontend
npm install
npm start
```

### Database Management
```bash
# Run migrations
npx prisma migrate dev

# View database
npx prisma studio
```

## 📝 API Endpoints

- `GET /api/health` - Health check
- `POST /api/chat` - Send message to BMO
- `GET /api/conversations` - Get chat history
- `POST /api/conversations` - Start new conversation

## 🎨 Customization

- Modify BMO's personality in `backend/src/services/aiService.js`
- Update UI theme in `frontend/src/styles/`
- Add new features following the established patterns

## 📚 Additional Learning

While this project focuses on JavaScript/TypeScript, you can explore:
- **Ruby**: Build a Ruby version of the backend using Sinatra or Rails
- **Go**: Create a Go backend using Gin or Echo frameworks
- **Python**: Implement with FastAPI or Django
- **Rust**: Build with Actix-web or Axum

## 🤝 Contributing

This is a learning project! Feel free to:
- Add new features
- Improve the UI/UX
- Optimize performance
- Add tests
- Experiment with different AI models

## 📄 License

MIT License - Feel free to use this project for learning and personal use!
