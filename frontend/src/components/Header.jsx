import React from 'react';

export default function Header({ currentView, setCurrentView }) {
  return (
    <header style={{ 
      display: 'flex', 
      justifyContent: 'space-between', 
      alignItems: 'center', 
      padding: '12px 24px', 
      backgroundColor: 'var(--app-surface)',
      borderBottom: '2px solid var(--border-darker)',
      boxShadow: '0 2px 4px rgba(0,0,0,0.1)'
    }}>
      <div 
        style={{ fontSize: '30px', fontWeight: 'bold', color: 'var(--accent-blue)', cursor: 'pointer', letterSpacing: '-0.5px' }}
        onClick={() => setCurrentView('search')}
      >
        Search++
      </div>
      <nav style={{ display: 'flex', gap: '24px', fontWeight: 'bold', fontSize: '20px' }}>
        <div 
          onClick={() => setCurrentView('search')} 
          style={{ cursor: 'pointer', color: currentView === 'search' ? 'var(--text-main)' : 'var(--text-muted)', borderBottom: currentView === 'search' ? '2px solid var(--text-main)' : 'none', paddingBottom: '2px' }}
        >
          Search
        </div>
        <div 
          onClick={() => setCurrentView('index')} 
          style={{ cursor: 'pointer', color: currentView === 'index' ? 'var(--text-main)' : 'var(--text-muted)', borderBottom: currentView === 'index' ? '2px solid var(--text-main)' : 'none', paddingBottom: '2px' }}
        >
          Index Pages
        </div>
        <div 
          onClick={() => setCurrentView('about')} 
          style={{ cursor: 'pointer', color: currentView === 'about' ? 'var(--text-main)' : 'var(--text-muted)', borderBottom: currentView === 'about' ? '2px solid var(--text-main)' : 'none', paddingBottom: '2px' }}
        >
          About
        </div>
      </nav>
    </header>
  );
}
