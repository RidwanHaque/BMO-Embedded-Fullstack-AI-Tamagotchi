#!/bin/bash

# BMO Chatbot Setup Script
echo "🤖 Setting up BMO Chatbot..."

# Check if Docker is installed
if ! command -v docker &> /dev/null; then
    echo "❌ Docker is not installed. Please install Docker first."
    exit 1
fi

# Check if Docker Compose is installed
if ! command -v docker-compose &> /dev/null; then
    echo "❌ Docker Compose is not installed. Please install Docker Compose first."
    exit 1
fi

# Create .env file if it doesn't exist
if [ ! -f .env ]; then
    echo "📝 Creating .env file..."
    cp env.example .env
    echo "⚠️  Please edit .env file and add your OpenAI API key!"
    echo "   You can get one from: https://platform.openai.com/api-keys"
fi

# Build and start the application
echo "🔨 Building and starting BMO Chatbot..."
docker-compose up --build -d

# Wait for services to be ready
echo "⏳ Waiting for services to start..."
sleep 10

# Check if services are running
echo "🔍 Checking service status..."
docker-compose ps

echo ""
echo "🎉 BMO Chatbot is ready!"
echo ""
echo "📱 Frontend: http://localhost:3000"
echo "🔧 Backend API: http://localhost:3001"
echo "🗄️  Database: localhost:5432"
echo ""
echo "📚 To view logs: docker-compose logs -f"
echo "🛑 To stop: docker-compose down"
echo ""
echo "🤖 Happy chatting with BMO!"
