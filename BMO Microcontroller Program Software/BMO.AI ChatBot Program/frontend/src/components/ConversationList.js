import React, { useState, useEffect } from 'react';
import { MessageSquare, Plus, Trash2, Clock } from 'lucide-react';
import { conversationAPI } from '../services/api';

const ConversationList = ({ currentConversation, setCurrentConversation }) => {
  const [conversations, setConversations] = useState([]);
  const [isLoading, setIsLoading] = useState(true);

  useEffect(() => {
    loadConversations();
  }, []);

  const loadConversations = async () => {
    try {
      setIsLoading(true);
      const data = await conversationAPI.getAll();
      setConversations(data);
    } catch (error) {
      console.error('Failed to load conversations:', error);
    } finally {
      setIsLoading(false);
    }
  };

  const createNewConversation = async () => {
    try {
      const newConversation = await conversationAPI.create({
        title: 'New Chat with BMO'
      });
      setConversations(prev => [newConversation, ...prev]);
      setCurrentConversation(newConversation.id);
    } catch (error) {
      console.error('Failed to create conversation:', error);
    }
  };

  const deleteConversation = async (id) => {
    try {
      await conversationAPI.delete(id);
      setConversations(prev => prev.filter(conv => conv.id !== id));
      if (currentConversation === id) {
        setCurrentConversation(null);
      }
    } catch (error) {
      console.error('Failed to delete conversation:', error);
    }
  };

  const formatDate = (dateString) => {
    const date = new Date(dateString);
    const now = new Date();
    const diffTime = Math.abs(now - date);
    const diffDays = Math.ceil(diffTime / (1000 * 60 * 60 * 24));

    if (diffDays === 1) return 'Today';
    if (diffDays === 2) return 'Yesterday';
    if (diffDays <= 7) return `${diffDays - 1} days ago`;
    return date.toLocaleDateString();
  };

  return (
    <div className="h-[560px] rounded-3xl bg-bmo-screen/70 border border-bmo-shell p-4 flex flex-col shadow-inner">
      <div className="flex items-center justify-between pb-4 border-b border-white/10">
        <h3 className="text-lg font-bold text-bmo-mint font-bmo">
          Conversations
        </h3>
        <button
          onClick={createNewConversation}
          className="bmo-button flex items-center space-x-1"
        >
          <Plus className="h-4 w-4" />
          <span className="font-bmo">New</span>
        </button>
      </div>

      <div className="flex-1 overflow-y-auto mt-4">
        {isLoading ? (
          <div className="flex items-center justify-center h-32">
            <div className="animate-spin rounded-full h-8 w-8 border-b-2 border-bmo-mint"></div>
          </div>
        ) : conversations.length === 0 ? (
          <div className="text-center text-bmo-mint/70 py-8">
            <MessageSquare className="h-12 w-12 mx-auto mb-4 text-bmo-mint" />
            <p className="font-bmo">No conversations yet!</p>
            <p className="text-sm font-pixel">Start chatting with BMO!</p>
          </div>
        ) : (
          <div className="space-y-3">
            {conversations.map((conversation) => (
              <div
                key={conversation.id}
                className={`p-3 rounded-2xl cursor-pointer transition-all duration-200 border ${
                  currentConversation === conversation.id
                    ? 'bg-bmo-mint text-bmo-dark border-transparent shadow-lg'
                    : 'bg-bmo-screen text-white border-white/10 hover:border-bmo-mint/40'
                }`}
                onClick={() => setCurrentConversation(conversation.id)}
              >
                <div className="flex items-start justify-between">
                  <div className="flex-1 min-w-0">
                    <h4 className="font-bmo text-sm font-bold truncate uppercase tracking-wide">
                      {conversation.title}
                    </h4>
                    <div className="flex items-center space-x-2 mt-1">
                      <Clock className="h-3 w-3 text-bmo-yellow" />
                      <span className="text-xs font-pixel text-white/70">
                        {formatDate(conversation.updatedAt)}
                      </span>
                      <span className="text-xs font-pixel text-white/50">
                        ({conversation._count?.messages || 0} messages)
                      </span>
                    </div>
                  </div>
                  <button
                    onClick={(e) => {
                      e.stopPropagation();
                      deleteConversation(conversation.id);
                    }}
                    className="ml-2 p-1 hover:bg-bmo-pink rounded transition-colors"
                  >
                    <Trash2 className="h-3 w-3" />
                  </button>
                </div>
              </div>
            ))}
          </div>
        )}
      </div>
    </div>
  );
};

export default ConversationList;
