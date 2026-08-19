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
          <div style={{ marginTop: '24px', display: 'flex', alignItems: 'center', gap: '24px' }}>
             <span style={{ fontSize: '20px', color: 'var(--accent-blue)', cursor: 'pointer', fontWeight: 'bold' }} onClick={() => setView('index')}>
                + Add pages to the index
             </span>
             <span style={{ fontSize: '20px', color: 'var(--text-muted)' }}>
                ● Index ready · {globalStats ? globalStats.total_pages.toLocaleString() : '...'} pages
             </span>
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
