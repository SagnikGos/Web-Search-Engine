import React from 'react';

export default function MetricsPanel({ stats, resultsCount, globalStats }) {
  if (!stats) return null;

  return (
    <div style={{ 
      width: '100%', 
      display: 'flex', 
      flexDirection: 'column', 
      gap: '16px',
      marginBottom: '32px'
    }}>
      <div style={{ fontSize: '20px', fontWeight: 'bold', letterSpacing: '1px', color: 'var(--text-muted)' }}>
        SEARCH COMPLETE
      </div>
      <div style={{ 
        display: 'flex', 
        justifyContent: 'space-between', 
        padding: '24px', 
        backgroundColor: '#ffffff',
        borderTop: '2px solid var(--border-dark)',
        borderLeft: '2px solid var(--border-dark)',
        borderRight: '2px solid var(--border-light)',
        borderBottom: '2px solid var(--border-light)',
        flexWrap: 'wrap', 
        gap: '24px' 
      }}>
        <div>
          <span style={{ fontSize: '40px', fontWeight: 'bold', display: 'block', color: 'var(--success-green)' }}>
            {(stats.time_us / 1000).toFixed(2)} ms
          </span>
          <span style={{ fontSize: '20px', color: 'var(--text-muted)' }}>Search completed</span>
        </div>
        
        <div>
          <span style={{ fontSize: '40px', fontWeight: 'bold', display: 'block' }}>
            {globalStats ? globalStats.total_pages.toLocaleString() : 'N/A'}
          </span>
          <span style={{ fontSize: '20px', color: 'var(--text-muted)' }}>Pages searched</span>
        </div>

        <div>
          <span style={{ fontSize: '40px', fontWeight: 'bold', display: 'block' }}>
            {globalStats ? globalStats.total_chars.toLocaleString() : 'N/A'}
          </span>
          <span style={{ fontSize: '20px', color: 'var(--text-muted)' }}>Chars processed</span>
        </div>

        <div>
          <span style={{ fontSize: '40px', fontWeight: 'bold', display: 'block', color: 'var(--accent-blue)' }}>
            {resultsCount.toLocaleString()}
          </span>
          <span style={{ fontSize: '20px', color: 'var(--text-muted)' }}>Results found</span>
        </div>
      </div>
    </div>
  );
}
