import React, { createContext, useContext, useEffect, useState } from 'react';
import { io } from 'socket.io-client';

const SocketContext = createContext();

export const useSocket = () => {
  const context = useContext(SocketContext);
  if (!context) {
    throw new Error('useSocket must be used within a SocketProvider');
  }
  return context;
};

export const SocketProvider = ({ children }) => {
  const [socket, setSocket] = useState(null);
  const [isConnected, setIsConnected] = useState(false);

  useEffect(() => {
    const socketInstance = io(process.env.REACT_APP_API_URL || 'http://localhost:3001', {
      transports: ['websocket', 'polling']
    });

    socketInstance.on('connect', () => {
      console.log('🔌 Connected to BMO server');
      setIsConnected(true);
    });

    socketInstance.on('disconnect', () => {
      console.log('🔌 Disconnected from BMO server');
      setIsConnected(false);
    });

    socketInstance.on('connect_error', (error) => {
      console.error('🔌 Connection error:', error);
      setIsConnected(false);
    });

    setSocket(socketInstance);

    return () => {
      socketInstance.close();
    };
  }, []);

  const value = {
    socket,
    isConnected
  };

  return (
    <SocketContext.Provider value={value}>
      {children}
    </SocketContext.Provider>
  );
};
