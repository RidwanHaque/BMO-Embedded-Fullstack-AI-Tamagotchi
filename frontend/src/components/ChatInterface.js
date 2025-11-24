import React, { useState, useEffect, useRef } from 'react';
import { Send, Bot, User, Loader2 } from 'lucide-react';
import { useSocket } from '../contexts/SocketContext';
import { chatAPI } from '../services/api';

const ChatInterface = ({ conversationId, setCurrentConversation }) => {
  const [messages, setMessages] = useState([]);
  const [inputMessage, setInputMessage] = useState('');
  const [isLoading, setIsLoading] = useState(false);
  const [isTyping, setIsTyping] = useState(false);
  const messagesEndRef = useRef(null);
  const { socket } = useSocket();

  const scrollToBottom = () => {
    messagesEndRef.current?.scrollIntoView({ behavior: 'smooth' });
  };

  useEffect(() => {
    scrollToBottom();
  }, [messages]);

  useEffect(() => {
    if (conversationId) {
      loadConversation(conversationId);
    } else {
      setMessages([]);
    }
  }, [conversationId]);

  useEffect(() => {
    if (socket) {
      socket.on('user-typing', (data) => {
        setIsTyping(data.isTyping);
      });

      return () => {
        socket.off('user-typing');
      };
    }
  }, [socket]);

  const loadConversation = async (id) => {
    try {
      const conversation = await chatAPI.getConversation(id);
      setMessages(conversation.messages || []);
    } catch (error) {
      console.error('Failed to load conversation:', error);
    }
  };

  const sendMessage = async (e) => {
    e.preventDefault();
    if (!inputMessage.trim() || isLoading) return;

    const userMessage = {
      id: Date.now(),
      content: inputMessage,
      role: 'USER',
      timestamp: new Date()
    };

    setMessages(prev => [...prev, userMessage]);
    setInputMessage('');
    setIsLoading(true);

    try {
      const response = await chatAPI.sendMessage(inputMessage, conversationId);
      
      const bmoMessage = {
        id: response.bmoResponse.id,
        content: response.bmoResponse.content,
        role: 'ASSISTANT',
        timestamp: new Date(response.bmoResponse.timestamp)
      };

      setMessages(prev => [...prev, bmoMessage]);
      
      if (!conversationId) {
        setCurrentConversation(response.conversationId);
      }
    } catch (error) {
      console.error('Failed to send message:', error);
      const errorMessage = {
        id: Date.now(),
        content: "Oops! Something went wrong. BMO's circuits are acting up!",
        role: 'ASSISTANT',
        timestamp: new Date()
      };
      setMessages(prev => [...prev, errorMessage]);
    } finally {
      setIsLoading(false);
    }
  };

  const handleTyping = (e) => {
    setInputMessage(e.target.value);
    
    if (socket && conversationId) {
      if (e.target.value.length > 0) {
        socket.emit('typing-start', { conversationId });
      } else {
        socket.emit('typing-stop', { conversationId });
      }
    }
  };

  return (
    <div className="flex flex-col h-[560px] rounded-3xl bg-bmo-screen/70 border border-bmo-shell p-4 shadow-inner">
      <div className="flex items-center space-x-3 pb-4 border-b border-white/10">
        <Bot className="h-6 w-6 text-bmo-mint animate-bounce-slow" />
        <h2 className="text-xl font-bold text-bmo-mint font-bmo">
          Chat with BMO
        </h2>
        {isTyping && (
          <div className="flex items-center space-x-1 text-bmo-yellow">
            <Loader2 className="h-4 w-4 animate-spin" />
            <span className="text-sm font-pixel">BMO is processing...</span>
          </div>
        )}
      </div>

      <div className="flex-1 overflow-y-auto py-4 space-y-4">
        {messages.length === 0 && (
          <div className="text-center text-bmo-mint/70 py-8">
            <Bot className="h-16 w-16 mx-auto mb-4 text-bmo-mint animate-pulse" />
            <p className="text-lg font-bmo">Hei! I'm BMO, your friendly robot friend!</p>
            <p className="text-sm font-pixel mt-2">Ask me anything - I love to help and chat!</p>
          </div>
        )}

        {messages.map((message) => (
          <div
            key={message.id}
            className={`flex ${message.role === 'USER' ? 'justify-end' : 'justify-start'}`}
          >
            <div
              className={`max-w-xs lg:max-w-md px-4 py-3 rounded-2xl shadow-lg ${
                message.role === 'USER'
                  ? 'bg-bmo-yellow/90 text-bmo-dark'
                  : 'bg-bmo-mint text-bmo-dark'
              }`}
            >
              <div className="flex items-start space-x-2">
                {message.role === 'ASSISTANT' && (
                  <Bot className="h-4 w-4 mt-1 flex-shrink-0 text-bmo-shell-dark" />
                )}
                {message.role === 'USER' && (
                  <User className="h-4 w-4 mt-1 flex-shrink-0 text-bmo-shell-dark" />
                )}
                <div className="flex-1">
                  <p className="text-sm font-bmo leading-relaxed">{message.content}</p>
                  <p className="text-xs opacity-70 mt-1 font-pixel text-bmo-dark/70">
                    {new Date(message.timestamp).toLocaleTimeString()}
                  </p>
                </div>
              </div>
            </div>
          </div>
        ))}

        {isLoading && (
          <div className="flex justify-start">
            <div className="bg-bmo-mint text-bmo-dark px-4 py-2 rounded-2xl">
              <div className="flex items-center space-x-2">
                <Bot className="h-4 w-4" />
                <Loader2 className="h-4 w-4 animate-spin text-bmo-shell-dark" />
                <span className="text-sm font-bmo">BMO is thinking...</span>
              </div>
            </div>
          </div>
        )}

        <div ref={messagesEndRef} />
      </div>

      <form onSubmit={sendMessage} className="pt-4 border-t border-white/10">
        <div className="flex space-x-2">
          <input
            type="text"
            value={inputMessage}
            onChange={handleTyping}
            placeholder="Type your message to BMO..."
            className="flex-1 bmo-input"
            disabled={isLoading}
          />
          <button
            type="submit"
            disabled={!inputMessage.trim() || isLoading}
            className="bmo-button flex items-center space-x-2 disabled:opacity-50 disabled:cursor-not-allowed"
          >
            <Send className="h-4 w-4" />
            <span className="font-bmo">Send</span>
          </button>
        </div>
      </form>
    </div>
  );
};

export default ChatInterface;
