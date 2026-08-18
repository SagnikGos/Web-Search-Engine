import React, { useState } from 'react';
import './index.css';

function App() {
  const [query, setQuery] = useState('');
  const [activeQuery, setActiveQuery] = useState('');
  const [results, setResults] = useState([]);
  const [stats, setStats] = useState(null);
  const [hasSearched, setHasSearched] = useState(false);
  const [loading, setLoading] = useState(false);

  // Crawler UI states
  const [showCrawler, setShowCrawler] = useState(false);
  const [crawlUrl, setCrawlUrl] = useState('');
  const [crawlMaxPages, setCrawlMaxPages] = useState(10);
  const [isCrawling, setIsCrawling] = useState(false);
  const [crawlMessage, setCrawlMessage] = useState(null);

  const handleCrawl = async (e) => {
    e.preventDefault();
    if (!crawlUrl.trim()) return;

    setIsCrawling(true);
    setCrawlMessage({ type: 'info', text: 'Crawling & indexing pages... this may take a while.' });

    try {
      const response = await fetch('http://localhost:8080/api/crawl', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ url: crawlUrl, max_pages: parseInt(crawlMaxPages) })
      });

      if (response.ok) {
        setCrawlMessage({ type: 'success', text: `Successfully crawled and indexed ${crawlUrl}!` });
        setCrawlUrl('');
      } else {
        const errorData = await response.json();
        setCrawlMessage({ type: 'error', text: errorData.error || 'Crawling failed.' });
      }
    } catch (error) {
      setCrawlMessage({ type: 'error', text: 'Failed to communicate with server.' });
    } finally {
      setIsCrawling(false);
    }
  };

  const handleSearch = async (e) => {
    if (e) e.preventDefault();
    if (!query.trim()) return;

    setHasSearched(true);
    setLoading(true);
    setActiveQuery(query);

    try {
      const response = await fetch(`http://localhost:8080/api/search?q=${encodeURIComponent(query)}`);
      if (response.ok) {
        const data = await response.json();
        setResults(data.results || []);
        setStats({
          time_us: data.time_us || 0,
          total_terms: data.total_terms || 0
        });
      } else {
        console.error("Search API returned an error");
        setResults([]);
        setStats(null);
      }
    } catch (error) {
      console.error("Failed to fetch search results", error);
      setResults([]);
      setStats(null);
    } finally {
      setLoading(false);
    }
  };

  const resetSearch = () => {
    setHasSearched(false);
    setQuery('');
    setActiveQuery('');
    setResults([]);
    setStats(null);
  };

  return (
    <div className={`app-container ${hasSearched ? 'results-state' : 'hero-state'}`}>
      <header className="search-section">
        <div className="header-top">
          <h1 className="logo" onClick={resetSearch}>Nexus</h1>
          <button 
            className="toggle-crawler-btn" 
            onClick={() => setShowCrawler(!showCrawler)}
            title="Feed Database"
          >
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={1.5} stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" d="M20.25 6.375c0 2.278-3.694 4.125-8.25 4.125S3.75 8.653 3.75 6.375m16.5 0c0-2.278-3.694-4.125-8.25-4.125S3.75 4.097 3.75 6.375m16.5 0v11.25c0 2.278-3.694 4.125-8.25 4.125s-8.25-1.847-8.25-4.125V6.375m16.5 0v3.75m-16.5-3.75v3.75m16.5 0v3.75C20.25 16.153 16.556 18 12 18s-8.25-1.847-8.25-4.125v-3.75m16.5 0v3.75C20.25 20.153 16.556 22 12 22s-8.25-1.847-8.25-4.125v-3.75m0 0v-3.75m0 0h.008v.008H12V14.25Z" />
            </svg>
          </button>
        </div>

        {showCrawler && (
          <div className="crawler-panel">
            <h3>Feed Database</h3>
            <form onSubmit={handleCrawl} className="crawler-form">
              <input 
                type="url" 
                placeholder="https://example.com" 
                value={crawlUrl}
                onChange={(e) => setCrawlUrl(e.target.value)}
                required
                disabled={isCrawling}
                className="crawler-input"
              />
              <div className="slider-container">
                <label>Max Pages: <span>{crawlMaxPages}</span></label>
                <input 
                  type="range" 
                  min="1" 
                  max="1000" 
                  value={crawlMaxPages}
                  onChange={(e) => setCrawlMaxPages(e.target.value)}
                  disabled={isCrawling}
                  className="crawler-slider"
                />
              </div>
              <button type="submit" disabled={isCrawling} className="crawl-submit-btn">
                {isCrawling ? <span className="loader"></span> : "Start Crawling"}
              </button>
            </form>
            {crawlMessage && (
              <div className={`crawl-message ${crawlMessage.type}`}>
                {crawlMessage.text}
              </div>
            )}
          </div>
        )}
        <form className="search-form" onSubmit={handleSearch}>
          <input
            type="text"
            className="search-input"
            value={query}
            onChange={(e) => setQuery(e.target.value)}
            placeholder="What are you looking for?"
            autoFocus
          />
          <button type="submit" className="search-button" aria-label="Search">
            <svg xmlns="http://www.w3.org/2000/svg" fill="none" viewBox="0 0 24 24" strokeWidth={2} stroke="currentColor">
              <path strokeLinecap="round" strokeLinejoin="round" d="m21 21-5.197-5.197m0 0A7.5 7.5 0 1 0 5.196 5.196a7.5 7.5 0 0 0 10.607 10.607Z" />
            </svg>
          </button>
        </form>
      </header>

      {hasSearched && (
        <main className="results-container">
          {loading ? (
            <div className="loading">Searching...</div>
          ) : (
            <>
              {stats && (
                <div className="stats">
                  Found {results.length} results in {(stats.time_us / 1000).toFixed(2)} ms (Terms: {stats.total_terms})
                </div>
              )}
              
              <div className="results-list">
                {results.length > 0 ? (
                  results.map((res, idx) => (
                    <div key={idx} className="result-item">
                      <a href={res.url} target="_blank" rel="noopener noreferrer" className="result-title">
                        {res.title || res.url}
                      </a>
                      <div className="result-url">{res.url}</div>
                      <div className="result-snippet" dangerouslySetInnerHTML={{ __html: res.snippet || "No snippet available." }}></div>
                    </div>
                  ))
                ) : (
                  <div className="no-results">
                    No results found for "{activeQuery}".
                  </div>
                )}
              </div>
            </>
          )}
        </main>
      )}
    </div>
  );
}

export default App;
