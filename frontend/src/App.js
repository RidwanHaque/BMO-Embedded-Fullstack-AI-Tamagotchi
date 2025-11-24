import React, { useState } from 'react';
import { BrowserRouter as Router, Routes, Route } from 'react-router-dom';
import ChatInterface from './components/ChatInterface';
import ConversationList from './components/ConversationList';
import Header from './components/Header';
import { SocketProvider } from './contexts/SocketContext';
import './App.css';

function App() {
  const [currentConversation, setCurrentConversation] = useState(null);

  return (
    <SocketProvider>
      <Router>
        <div className="app-background">
          <div className="bmo-shell">
            <Header />

            <main className="bmo-screen">
              <Routes>
                <Route
                  path="/"
                  element={
                    <div className="grid grid-cols-1 lg:grid-cols-3 gap-6">
                      <div className="lg:col-span-1">
                        <ConversationList
                          currentConversation={currentConversation}
                          setCurrentConversation={setCurrentConversation}
                        />
                      </div>
                      <div className="lg:col-span-2">
                        <ChatInterface
                          conversationId={currentConversation}
                          setCurrentConversation={setCurrentConversation}
                        />
                      </div>
                    </div>
                  }
                />
                <Route
                  path="/chat/:conversationId"
                  element={
                    <ChatInterface
                      conversationId={currentConversation}
                      setCurrentConversation={setCurrentConversation}
                    />
                  }
                />
              </Routes>
            </main>

            <div className="bmo-controls">
              <span>BMO</span>
              <div className="bmo-dpad">
                <span />
                <span />
                <span />
              </div>
              <div className="bmo-happy-buttons">
                <span />
                <span />
                <span />
              </div>
            </div>
          </div>
        </div>
      </Router>
    </SocketProvider>
  );
}

export default App;
