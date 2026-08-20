import React, { useState } from 'react';
import { Button, ProgressBar } from 'retro-react';
import MetricsPanel from './MetricsPanel';
import SearchResults from './SearchResults';

export default function SearchPage({ setView, globalStats }) {
  const [query, setQuery] = useState('');
  const [activeQuery, setActiveQuery] = useState('');
  const [results, setResults] = useState(null);
  const [stats, setStats] = useState(null);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState(null);

  const handleSearch = async (e) => {
    e.preventDefault();
    if (!query.trim()) return;

    setLoading(true);
    setError(null);
    setActiveQuery(query);
    setResults(null);
    setStats(null);

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
        setError('Search server is unavailable or returned an error.');
      }
    } catch (err) {
      setError('Search could not be completed. The search server is unavailable.');
    } finally {
      setLoading(false);
    }
  };

  return (
    <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', width: '100%', maxWidth: '900px', margin: '40px auto', padding: '0 24px', boxSizing: 'border-box' }}>
      
      {/* Hero Search Area */}
      <div style={{ width: '100%', display: 'flex', flexDirection: 'column', alignItems: 'center', marginBottom: results ? '40px' : '80px', marginTop: results ? '0' : '10vh', transition: 'all 0.3s ease' }}>
        
        {!results && (
          <div style={{ textAlign: 'center', marginBottom: '32px' }}>
            <h1 style={{ fontSize: '60px', fontWeight: 'bold', margin: '0 0 12px 0', color: 'var(--accent-blue)', letterSpacing: '-1px' }}>Search++</h1>
            <p style={{ fontSize: '20px', color: 'var(--text-muted)', margin: 0 }}>Search your indexed pages at incredible speed.</p>
          </div>
        )}

        {results && (
          <div style={{ width: '100%', marginBottom: '24px' }}>
            <Button onClick={() => setResults(null)} style={{ fontWeight: 'bold', padding: '4px 16px', marginBottom: '16px' }}>
              &lt; Back
            </Button>
            <h1 style={{ fontSize: '40px', fontWeight: 'bold', margin: '0 0 16px 0', color: 'var(--accent-blue)' }}>Search++</h1>
          </div>
        )}
        
        <form onSubmit={handleSearch} style={{ width: '100%', display: 'flex', flexDirection: 'column', alignItems: results ? 'flex-start' : 'center' }}>
          <div style={{ display: 'flex', width: '100%', gap: '12px' }}>
            <input
              type="text"
              placeholder="What are you looking for?"
              value={query}
              onChange={(e) => setQuery(e.target.value)}
              style={{ 
                flex: 1,
                fontSize: '20px', 
                padding: '0 16px',
                height: '48px',
                borderTop: '2px solid var(--border-darker)',
                borderLeft: '2px solid var(--border-darker)',
                borderRight: '2px solid var(--border-light)',
                borderBottom: '2px solid var(--border-light)',
                backgroundColor: '#ffffff',
                outline: 'none',
                boxSizing: 'border-box'
              }}
              autoFocus
            />
            <Button type="submit" style={{ padding: '0 32px', height: '48px', fontSize: '20px', fontWeight: 'bold', letterSpacing: '0.5px', boxSizing: 'border-box' }}>
              SEARCH
            </Button>
          </div>
        </form>

        {!results && (
          <div style={{ marginTop: '24px', width: '100%' }}>
            
            <fieldset style={{
              borderTop: '2px solid var(--border-dark)',
              borderLeft: '2px solid var(--border-dark)',
              borderBottom: '2px solid var(--border-light)',
              borderRight: '2px solid var(--border-light)',
              padding: '16px 24px',
              margin: '0 0 32px 0',
              width: '100%',
              boxSizing: 'border-box'
            }}>
              <legend style={{ padding: '0 8px', fontSize: '20px' }}>System Status</legend>
              <div style={{ display: 'flex', flexDirection: 'column', gap: '8px', fontSize: '20px' }}>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span>Crawler</span>
                  <span style={{ color: 'var(--success-green)' }}>● Online</span>
                </div>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span>Indexer</span>
                  <span>Ready</span>
                </div>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span>Documents</span>
                  <span>{globalStats ? globalStats.total_pages.toLocaleString() : '...'}</span>
                </div>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span>Characters</span>
                  <span>{globalStats ? globalStats.total_chars.toLocaleString() : '...'}</span>
                </div>
                <div style={{ display: 'flex', justifyContent: 'space-between' }}>
                  <span>Backend</span>
                  <span>C++</span>
                </div>
              </div>
            </fieldset>

            <div style={{ display: 'flex', gap: '24px', flexWrap: 'wrap' }}>
              
              {/* Feature 1 */}
              <div style={{
                flex: '1 1 250px',
                backgroundColor: 'var(--app-bg)',
                borderTop: '2px solid var(--border-light)',
                borderLeft: '2px solid var(--border-light)',
                borderBottom: '2px solid var(--border-darker)',
                borderRight: '2px solid var(--border-darker)',
                boxShadow: 'inset -2px -2px 0px var(--border-dark), inset 2px 2px 0px var(--app-bg)',
                padding: '2px'
              }}>
                <div style={{
                  backgroundColor: 'var(--accent-blue)', color: 'white', padding: '4px 8px',
                  fontWeight: 'bold', fontSize: '18px', display: 'flex', justifyContent: 'space-between'
                }}>
                  <span>net.exe</span>
                  <div style={{
                    backgroundColor: 'var(--app-bg)', borderTop: '2px solid var(--border-light)', borderLeft: '2px solid var(--border-light)', borderBottom: '2px solid var(--border-darker)', borderRight: '2px solid var(--border-darker)', width: '20px', height: '20px', display: 'flex', justifyContent: 'center', alignItems: 'center', color: 'black', fontSize: '14px', cursor: 'pointer'
                  }}>x</div>
                </div>
                <div style={{ padding: '16px', fontSize: '20px' }}>
                  <p style={{ margin: '0 0 12px 0', fontWeight: 'bold' }}>Multithreaded Crawler</p>
                  <p style={{ margin: 0, color: 'var(--text-muted)' }}>Concurrent page fetching for fast indexing.</p>
                </div>
              </div>

              {/* Feature 2 */}
              <div style={{
                flex: '1 1 250px',
                backgroundColor: 'var(--app-bg)',
                borderTop: '2px solid var(--border-light)',
                borderLeft: '2px solid var(--border-light)',
                borderBottom: '2px solid var(--border-darker)',
                borderRight: '2px solid var(--border-darker)',
                boxShadow: 'inset -2px -2px 0px var(--border-dark), inset 2px 2px 0px var(--app-bg)',
                padding: '2px'
              }}>
                <div style={{
                  backgroundColor: 'var(--accent-blue)', color: 'white', padding: '4px 8px',
                  fontWeight: 'bold', fontSize: '18px', display: 'flex', justifyContent: 'space-between'
                }}>
                  <span>index.sys</span>
                  <div style={{
                    backgroundColor: 'var(--app-bg)', borderTop: '2px solid var(--border-light)', borderLeft: '2px solid var(--border-light)', borderBottom: '2px solid var(--border-darker)', borderRight: '2px solid var(--border-darker)', width: '20px', height: '20px', display: 'flex', justifyContent: 'center', alignItems: 'center', color: 'black', fontSize: '14px', cursor: 'pointer'
                  }}>x</div>
                </div>
                <div style={{ padding: '16px', fontSize: '20px' }}>
                  <p style={{ margin: '0 0 12px 0', fontWeight: 'bold' }}>Inverted Index</p>
                  <p style={{ margin: 0, color: 'var(--text-muted)' }}>Instant term lookups using optimized data structures.</p>
                </div>
              </div>

              {/* Feature 3 */}
              <div style={{
                flex: '1 1 250px',
                backgroundColor: 'var(--app-bg)',
                borderTop: '2px solid var(--border-light)',
                borderLeft: '2px solid var(--border-light)',
                borderBottom: '2px solid var(--border-darker)',
                borderRight: '2px solid var(--border-darker)',
                boxShadow: 'inset -2px -2px 0px var(--border-dark), inset 2px 2px 0px var(--app-bg)',
                padding: '2px'
              }}>
                <div style={{
                  backgroundColor: 'var(--accent-blue)', color: 'white', padding: '4px 8px',
                  fontWeight: 'bold', fontSize: '18px', display: 'flex', justifyContent: 'space-between'
                }}>
                  <span>rank.dll</span>
                  <div style={{
                    backgroundColor: 'var(--app-bg)', borderTop: '2px solid var(--border-light)', borderLeft: '2px solid var(--border-light)', borderBottom: '2px solid var(--border-darker)', borderRight: '2px solid var(--border-darker)', width: '20px', height: '20px', display: 'flex', justifyContent: 'center', alignItems: 'center', color: 'black', fontSize: '14px', cursor: 'pointer'
                  }}>x</div>
                </div>
                <div style={{ padding: '16px', fontSize: '20px' }}>
                  <p style={{ margin: '0 0 12px 0', fontWeight: 'bold' }}>TF-IDF Ranking</p>
                  <p style={{ margin: 0, color: 'var(--text-muted)' }}>Relevant search results in milliseconds.</p>
                </div>
              </div>

            </div>

            <div style={{ marginTop: '32px', textAlign: 'center' }}>
              <span style={{ fontSize: '20px', color: 'var(--accent-blue)', cursor: 'pointer', fontWeight: 'bold', textDecoration: 'underline' }} onClick={() => setView('index')}>
                Go to Indexing Console →
              </span>
            </div>
            
          </div>
        )}
      </div>

      {/* Error Dialog */}
      {error && (
        <div style={{ width: '100%', padding: '16px', backgroundColor: '#ffecec', borderLeft: '4px solid #cc0000', marginBottom: '24px' }}>
          <p style={{ margin: '0 0 12px 0', fontWeight: 'bold', color: '#cc0000' }}>Error</p>
          <p style={{ margin: '0 0 16px 0' }}>{error}</p>
          <Button onClick={() => setError(null)}>Dismiss</Button>
        </div>
      )}

      {/* Loading State */}
      {loading && (
        <div style={{ width: '100%', maxWidth: '600px', padding: '32px', textAlign: 'center' }}>
          <ProgressBar indeterminate style={{ marginBottom: '16px', height: '12px' }} />
          <p style={{ margin: 0, fontWeight: 'bold', color: 'var(--text-muted)' }}>Searching index...</p>
        </div>
      )}

      {/* Search Results Area */}
      {!loading && results !== null && (
        <div style={{ width: '100%' }}>
          <MetricsPanel stats={stats} resultsCount={results.length} globalStats={globalStats} />
          <div style={{ marginTop: '32px' }}>
            <SearchResults results={results} />
          </div>
        </div>
      )}

    </div>
  );
}
