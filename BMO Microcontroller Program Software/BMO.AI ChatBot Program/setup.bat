@echo off
REM BMO Chatbot Setup Script for Windows

echo 🤖 Setting up BMO Chatbot...

REM Check if Docker is installed
docker --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Docker is not installed. Please install Docker Desktop first.
    pause
    exit /b 1
)

REM Check if Docker Compose is installed
docker-compose --version >nul 2>&1
if %errorlevel% neq 0 (
    echo ❌ Docker Compose is not installed. Please install Docker Compose first.
    pause
    exit /b 1
)

REM Create .env file if it doesn't exist
if not exist .env (
    echo 📝 Creating .env file...
    copy env.example .env
    echo ⚠️  Please edit .env file and add your OpenAI API key!
    echo    You can get one from: https://platform.openai.com/api-keys
)

REM Build and start the application
echo 🔨 Building and starting BMO Chatbot...
docker-compose up --build -d

REM Wait for services to be ready
echo ⏳ Waiting for services to start...
timeout /t 10 /nobreak >nul

REM Check if services are running
echo 🔍 Checking service status...
docker-compose ps

echo.
echo 🎉 BMO Chatbot is ready!
echo.
echo 📱 Frontend: http://localhost:3000
echo 🔧 Backend API: http://localhost:3001
echo 🗄️  Database: localhost:5432
echo.
echo 📚 To view logs: docker-compose logs -f
echo 🛑 To stop: docker-compose down
echo.
echo 🤖 Happy chatting with BMO!
pause
