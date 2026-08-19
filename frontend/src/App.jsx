import React, { useState } from 'react';
import { ThemeProvider, defaultTheme } from 'retro-react';
import Header from './components/Header';
import StatusBar from './components/StatusBar';
import SearchPage from './components/SearchPage';
import IndexPage from './components/IndexPage';
import AboutPage from './components/AboutPage';
import './index.css';

function App() {
  const [currentView, setCurrentView] = useState('search'); // 'search' or 'index'
  const [globalStats, setGlobalStats] = useState(null);

  React.useEffect(() => {
    const fetchStats = async () => {
      try {
        const res = await fetch('http://localhost:8080/api/stats');
        if (res.ok) {
          const data = await res.json();
          setGlobalStats(data);
        }
      } catch (err) {}
    };
    fetchStats();
    // Poll every 1 second
    const interval = setInterval(fetchStats, 1000);
    return () => clearInterval(interval);
  }, []);

  const isIndexing = false; 
  const indexReady = globalStats ? globalStats.index_ready : false;

  return (
    <ThemeProvider theme={defaultTheme}>
      <div style={{ display: 'flex', flexDirection: 'column', height: '100vh', width: '100vw', overflow: 'hidden' }}>
        <Header currentView={currentView} setCurrentView={setCurrentView} />
        
        <div className="desktop" style={{ flex: 1, overflowY: 'auto' }}>
          {currentView === 'search' && <SearchPage setView={setCurrentView} globalStats={globalStats} />}
          {currentView === 'index' && <IndexPage setView={setCurrentView} globalStats={globalStats} />}
          {currentView === 'about' && <AboutPage setView={setCurrentView} />}
        </div>

        <StatusBar isIndexing={isIndexing} indexReady={indexReady} globalStats={globalStats} />
      </div>
    </ThemeProvider>
  );
}

export default App;
