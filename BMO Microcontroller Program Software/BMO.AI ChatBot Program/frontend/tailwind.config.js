module.exports = {
  content: [
    "./src/**/*.{js,jsx,ts,tsx}",
  ],
  theme: {
    extend: {
      colors: {
        'bmo-shell': '#29b5a2',
        'bmo-shell-dark': '#1f8f7d',
        'bmo-screen': '#103b4a',
        'bmo-mint': '#c9f5ec',
        'bmo-yellow': '#ffd25f',
        'bmo-pink': '#ff5f7e',
        'bmo-dark': '#0a2230',
        'bmo-darker': '#071a25',
      },
      fontFamily: {
        'bmo': ['Comic Sans MS', 'cursive'],
        'pixel': ['Courier New', 'monospace'],
      },
      animation: {
        'bounce-slow': 'bounce 2s infinite',
        'pulse-slow': 'pulse 3s infinite',
        'glow': 'glow 2s ease-in-out infinite alternate',
      },
      keyframes: {
        glow: {
          '0%': { boxShadow: '0 0 5px #00ff88' },
          '100%': { boxShadow: '0 0 20px #00ff88, 0 0 30px #00ff88' },
        }
      }
    },
  },
  plugins: [],
}
