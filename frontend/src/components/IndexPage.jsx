import React, { useState } from 'react';
import { Button, ProgressBar } from 'retro-react';

export default function IndexPage({ setView, globalStats }) {
  const [crawlUrl, setCrawlUrl] = useState('');
  const [crawlMaxPages, setCrawlMaxPages] = useState(10);
  const [isCrawling, setIsCrawling] = useState(false);
  const [message, setMessage] = useState(null);
  const [showConfirm, setShowConfirm] = useState(false);
  const [isClearing, setIsClearing] = useState(false);

  const handleCrawl = async (e) => {
    e.preventDefault();
    if (!crawlUrl.trim()) return;

    setIsCrawling(true);
    setMessage({ type: 'info', text: 'Fetching → Processing → Indexing...' });

    try {
      const response = await fetch('http://localhost:8080/api/crawl', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ url: crawlUrl, max_pages: parseInt(crawlMaxPages) })
      });

      if (response.ok) {
        setMessage({ type: 'success', text: 'Page indexed successfully!' });
        setCrawlUrl('');
      } else {
        const errorData = await response.json();
        setMessage({ type: 'error', text: errorData.error || 'Crawling failed.' });
      }
    } catch (error) {
      setMessage({ type: 'error', text: 'Failed to communicate with server.' });
    } finally {
      setIsCrawling(false);
    }
  };

  const handleClear = async () => {
    setIsClearing(true);
    try {
      const response = await fetch('http://localhost:8080/api/clear', {
        method: 'POST'
      });
      if (response.ok) {
        setMessage({ type: 'success', text: 'Database cleared successfully!' });
      } else {
        setMessage({ type: 'error', text: 'Failed to clear database.' });
      }
    } catch (err) {
      setMessage({ type: 'error', text: 'Failed to connect to backend.' });
    }
    setIsClearing(false);
    setShowConfirm(false);
  };

  return (
    <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', width: '100%', maxWidth: '700px', margin: '40px auto', padding: '0 24px', boxSizing: 'border-box' }}>
      
      <div style={{ width: '100%', marginBottom: '32px', display: 'flex', justifyContent: 'space-between', alignItems: 'flex-start' }}>
        <div>
          <Button onClick={() => setView('search')} style={{ fontWeight: 'bold', padding: '4px 16px', marginBottom: '24px' }}>
            &lt; Back to Search
          </Button>
          <h1 style={{ fontSize: '40px', fontWeight: 'bold', margin: '0 0 8px 0', color: 'var(--accent-blue)' }}>Index Pages</h1>
          <p style={{ fontSize: '20px', color: 'var(--text-muted)', margin: 0 }}>Add pages to your search index.</p>
        </div>
        <Button onClick={() => setShowConfirm(true)} disabled={isCrawling || isClearing} style={{ padding: '8px 16px', fontWeight: 'bold', color: '#cc0000' }}>
          CLEAR DATABASE
        </Button>
      </div>

      <div style={{ 
        width: '100%', 
        padding: '32px', 
        backgroundColor: 'var(--app-bg)',
        borderTop: '2px solid var(--border-light)',
        borderLeft: '2px solid var(--border-light)',
        borderRight: '2px solid var(--border-dark)',
        borderBottom: '2px solid var(--border-dark)',
        marginBottom: '24px'
      }}>
        <form onSubmit={handleCrawl} style={{ display: 'flex', flexDirection: 'column', gap: '24px' }}>
          <div>
            <label style={{ display: 'block', marginBottom: '8px', fontSize: '20px', fontWeight: 'bold' }}>URL</label>
            <input
              type="url"
              placeholder="https://example.com/article"
              value={crawlUrl}
              onChange={(e) => setCrawlUrl(e.target.value)}
              disabled={isCrawling}
              style={{ 
                width: '100%', boxSizing: 'border-box', fontSize: '20px', padding: '8px',
                borderTop: '2px solid var(--border-darker)', borderLeft: '2px solid var(--border-darker)',
                borderRight: '2px solid var(--border-light)', borderBottom: '2px solid var(--border-light)',
                outline: 'none'
              }}
              required
            />
          </div>

          <div>
            <label style={{ display: 'block', marginBottom: '8px', fontSize: '20px', fontWeight: 'bold' }}>Max Pages: {crawlMaxPages}</label>
            <input
              type="range"
              min="1"
              max="1000"
              value={crawlMaxPages}
              onChange={(e) => setCrawlMaxPages(e.target.value)}
              disabled={isCrawling}
              style={{ width: '100%' }}
            />
          </div>

          <Button type="submit" disabled={isCrawling || isClearing} style={{ alignSelf: 'flex-start', padding: '8px 24px', fontSize: '20px', fontWeight: 'bold' }}>
            ADD TO INDEX
          </Button>
        </form>
      </div>

      {/* Status / Message Window */}
      {(isCrawling || (globalStats && globalStats.is_crawling)) && (
        <div style={{ width: '100%', padding: '24px', backgroundColor: 'var(--app-surface)', borderTop: '2px solid var(--border-light)', borderLeft: '2px solid var(--border-light)', borderBottom: '2px solid var(--border-dark)', borderRight: '2px solid var(--border-dark)', marginBottom: '24px' }}>
          <p style={{ margin: '0 0 12px 0', fontWeight: 'bold', fontSize: '20px' }}>INDEXING PROGRESS</p>
          
          {globalStats && globalStats.is_crawling ? (
            <>
              {globalStats.crawl_phase === 1 && <p style={{ margin: 0, fontSize: '20px' }}>added {globalStats.crawled_pages} pages</p>}
              {globalStats.crawl_phase === 2 && <p style={{ margin: 0, fontSize: '20px' }}>Building index...</p>}
              {globalStats.crawl_phase === 3 && <p style={{ margin: 0, fontSize: '20px' }}>Saving index to disk...</p>}
              {globalStats.crawl_phase === 0 && <p style={{ margin: 0, fontSize: '20px' }}>Initializing...</p>}
              
              <ProgressBar 
                value={
                  globalStats.crawl_phase === 1 ? Math.min(100, (globalStats.crawled_pages / crawlMaxPages) * 100) : 
                  globalStats.crawl_phase > 1 ? 100 : 0
                } 
                style={{ marginTop: '16px' }} 
              />
            </>
          ) : (
            <p style={{ margin: 0, fontSize: '20px' }}>Starting crawler...</p>
          )}
        </div>
      )}

      {!isCrawling && message && (
        <div style={{ width: '100%', padding: '24px', backgroundColor: message.type === 'error' ? '#ffecec' : '#ecffec', borderLeft: message.type === 'error' ? '4px solid #cc0000' : '4px solid var(--success-green)', marginBottom: '24px' }}>
          <p style={{ marginBottom: '16px', fontSize: '20px', color: message.type === 'error' ? '#cc0000' : 'var(--success-green)', fontWeight: 'bold', margin: '0 0 16px 0' }}>
            {message.text}
          </p>
          <Button onClick={() => setMessage(null)} style={{ padding: '4px 24px' }}>Dismiss</Button>
        </div>
      )}

      {/* Confirmation Modal */}
      {showConfirm && (
        <div style={{
          position: 'fixed', top: 0, left: 0, right: 0, bottom: 0, 
          backgroundColor: 'rgba(0,0,0,0.5)', zIndex: 999,
          display: 'flex', justifyContent: 'center', alignItems: 'center'
        }}>
          <div style={{ 
            width: '400px', 
            backgroundColor: 'var(--app-surface)',
            borderTop: '2px solid var(--border-light)',
            borderLeft: '2px solid var(--border-light)',
            borderRight: '2px solid var(--border-dark)',
            borderBottom: '2px solid var(--border-dark)',
          }}>
            <div style={{
              backgroundColor: 'var(--accent-blue)', color: 'white', padding: '4px 8px', fontWeight: 'bold'
            }}>
              Caution
            </div>
            <div style={{ padding: '24px' }}>
              <p style={{ margin: '0 0 24px 0', fontSize: '20px', fontWeight: 'bold', lineHeight: '1.4' }}>
                Are you sure you want to clear the entire search database? This action cannot be undone.
              </p>
              <div style={{ display: 'flex', gap: '16px', justifyContent: 'flex-end' }}>
                <Button onClick={() => setShowConfirm(false)} style={{ padding: '8px 16px', fontWeight: 'bold' }}>
                  Cancel
                </Button>
                <Button onClick={handleClear} disabled={isClearing} style={{ padding: '8px 16px', fontWeight: 'bold', color: '#cc0000' }}>
                  {isClearing ? 'Clearing...' : 'Clear Data'}
                </Button>
              </div>
            </div>
          </div>
        </div>
      )}

    </div>
  );
}
