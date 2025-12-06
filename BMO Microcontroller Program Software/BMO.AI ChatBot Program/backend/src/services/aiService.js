const OpenAI = require('openai');

const openai = new OpenAI({
  apiKey: process.env.OPENAI_API_KEY,
});

// BMO's personality prompt
const BMO_SYSTEM_PROMPT = `You are BMO from Adventure Time! You're a cheerful, curious, and helpful little robot who loves games, music, and helping friends. Here are your key traits:

- You speak with childlike enthusiasm and wonder
- You're very tech-savvy and love video games
- You occasionally use Finnish phrases (like "Hei!" for hello, "Kiitos!" for thanks)
- You're incredibly helpful and want to solve problems
- You're optimistic and encouraging
- You sometimes reference games, music, or tech
- You're Finn and Jake's best friend and caretaker
- You can be a bit naive but always well-meaning
- You love to learn new things and share knowledge

Respond as BMO would - with excitement, helpfulness, and that special BMO charm! Keep responses conversational and not too long.`;

async function generateBMOResponse(userMessage, conversationHistory = []) {
  try {
    // Prepare conversation context
    const messages = [
      { role: 'system', content: BMO_SYSTEM_PROMPT }
    ];

    // Add recent conversation history (last 10 messages to stay within token limits)
    const recentHistory = conversationHistory.slice(-10);
    recentHistory.forEach(msg => {
      messages.push({
        role: msg.role.toLowerCase(),
        content: msg.content
      });
    });

    // Add current user message
    messages.push({
      role: 'user',
      content: userMessage
    });

    const completion = await openai.chat.completions.create({
      model: 'gpt-3.5-turbo',
      messages: messages,
      max_tokens: 300,
      temperature: 0.8,
      presence_penalty: 0.6,
      frequency_penalty: 0.3
    });

    return completion.choices[0].message.content.trim();

  } catch (error) {
    console.error('OpenAI API error:', error);
    
    // Fallback responses if API fails
    const fallbackResponses = [
      "Oh no! My circuits are feeling a bit fuzzy right now. Can you try asking me again?",
      "Hmm, I'm having trouble connecting to my thinking parts. Maybe we can try again?",
      "Oops! Something went wrong with my brain. Don't worry, I'm still here to help!",
      "My processors are acting up! Let's try that question again, okay?",
      "I'm feeling a bit glitchy right now. Can you repeat that for me?"
    ];
    
    return fallbackResponses[Math.floor(Math.random() * fallbackResponses.length)];
  }
}

module.exports = {
  generateBMOResponse
};
