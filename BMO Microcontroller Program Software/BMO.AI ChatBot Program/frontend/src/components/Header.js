import React from 'react';

const Header = () => {
  return (
    <header className="bmo-face-wrapper">
      <div className="bmo-face">
        <div className="bmo-eyes">
          <span className="bmo-eye" />
          <span className="bmo-eye" />
        </div>
        <div className="bmo-mouth" />
        <h1 className="text-3xl font-bold text-bmo-dark tracking-wide mt-2">
          Hei! I'm BMO
        </h1>
        <p className="text-sm text-bmo-dark/70 font-pixel text-center">
          Retro game console • Loyal friend • Ready to chat
        </p>
        <div className="bmo-badges">
          <span className="bmo-badge">CHAT</span>
          <span className="bmo-badge">RETRO</span>
          <span className="bmo-badge">FRIEND</span>
        </div>
      </div>
    </header>
  );
};

export default Header;
