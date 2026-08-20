import React, { useState, useEffect } from 'react';
import { Text, Button } from 'retro-react';

export default function SearchResults({ results }) {
  const [currentPage, setCurrentPage] = useState(1);
  const resultsPerPage = 20;

  // Reset page to 1 when a new search comes in
  useEffect(() => {
    setCurrentPage(1);
  }, [results]);

  if (results.length === 0) {
    return (
      <div style={{ padding: '16px', textAlign: 'center' }}>
        <Text style={{ fontWeight: 'bold' }}>No results found.</Text>
        <Text>Try a different search.</Text>
      </div>
    );
  }

  const totalPages = Math.ceil(results.length / resultsPerPage);
  const startIndex = (currentPage - 1) * resultsPerPage;
  const currentResults = results.slice(startIndex, startIndex + resultsPerPage);

  const handlePrev = () => {
    if (currentPage > 1) {
      setCurrentPage(currentPage - 1);
      window.scrollTo(0, 0); // Scroll to top when changing page
    }
  };

  const handleNext = () => {
    if (currentPage < totalPages) {
      setCurrentPage(currentPage + 1);
      window.scrollTo(0, 0);
    }
  };

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '16px', padding: '16px 0' }}>
      {currentResults.map((res, idx) => (
        <div key={idx} style={{ paddingBottom: '12px', borderBottom: '1px solid #c0c0c0' }}>
          <div>
            <a href={res.url} target="_blank" rel="noopener noreferrer" style={{ fontSize: '20px', fontWeight: 'bold' }}>
              {res.title || res.url}
            </a>
          </div>
          <div style={{ color: '#008000', fontSize: '20px', marginBottom: '8px' }}>{res.url}</div>
          <div style={{ fontSize: '20px', lineHeight: '1.4' }} dangerouslySetInnerHTML={{ __html: res.snippet || "No snippet available." }}></div>
          {res.score && (
            <div style={{ fontSize: '15px', color: '#808080', marginTop: '8px' }}>Relevance Score: {res.score.toFixed(4)}</div>
          )}
        </div>
      ))}

      {totalPages > 1 && (
        <div style={{ 
          display: 'flex', justifyContent: 'center', alignItems: 'center', 
          gap: '24px', marginTop: '32px', padding: '16px',
          backgroundColor: 'var(--app-bg)',
          borderTop: '2px solid var(--border-light)',
          borderLeft: '2px solid var(--border-light)',
          borderBottom: '2px solid var(--border-darker)',
          borderRight: '2px solid var(--border-darker)'
        }}>
          <Button onClick={handlePrev} disabled={currentPage === 1} style={{ fontWeight: 'bold', padding: '4px 16px', width: '100px' }}>
            &lt; Prev
          </Button>
          
          <div style={{ fontSize: '20px', fontWeight: 'bold', minWidth: '120px', textAlign: 'center', color: 'var(--border-darker)' }}>
            Page {currentPage} of {totalPages}
          </div>
          
          <Button onClick={handleNext} disabled={currentPage === totalPages} style={{ fontWeight: 'bold', padding: '4px 16px', width: '100px' }}>
            Next &gt;
          </Button>
        </div>
      )}
    </div>
  );
}
