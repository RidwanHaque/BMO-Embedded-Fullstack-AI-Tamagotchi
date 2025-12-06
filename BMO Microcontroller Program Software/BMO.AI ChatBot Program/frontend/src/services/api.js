import axios from 'axios';

const API_BASE_URL = process.env.REACT_APP_API_URL || 'http://localhost:3001';

const api = axios.create({
  baseURL: API_BASE_URL,
  timeout: 10000,
  headers: {
    'Content-Type': 'application/json',
  },
});

// Request interceptor
api.interceptors.request.use(
  (config) => {
    console.log(`🚀 API Request: ${config.method?.toUpperCase()} ${config.url}`);
    return config;
  },
  (error) => {
    console.error('🚀 API Request Error:', error);
    return Promise.reject(error);
  }
);

// Response interceptor
api.interceptors.response.use(
  (response) => {
    console.log(`✅ API Response: ${response.status} ${response.config.url}`);
    return response;
  },
  (error) => {
    console.error('❌ API Response Error:', error.response?.data || error.message);
    return Promise.reject(error);
  }
);

export const chatAPI = {
  sendMessage: async (message, conversationId = null) => {
    const response = await api.post('/api/chat/message', {
      message,
      conversationId
    });
    return response.data;
  },

  getConversation: async (conversationId) => {
    const response = await api.get(`/api/chat/conversation/${conversationId}`);
    return response.data;
  }
};

export const conversationAPI = {
  getAll: async () => {
    const response = await api.get('/api/conversations');
    return response.data;
  },

  create: async (data) => {
    const response = await api.post('/api/conversations', data);
    return response.data;
  },

  update: async (id, data) => {
    const response = await api.patch(`/api/conversations/${id}`, data);
    return response.data;
  },

  delete: async (id) => {
    const response = await api.delete(`/api/conversations/${id}`);
    return response.data;
  }
};

export const healthAPI = {
  check: async () => {
    const response = await api.get('/api/health');
    return response.data;
  }
};

export default api;
