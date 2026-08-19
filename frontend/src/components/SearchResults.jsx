import React from 'react';
import { Text } from 'retro-react';

export default function SearchResults({ results }) {
  if (results.length === 0) {
    return (
      <div style={{ padding: '16px', textAlign: 'center' }}>
        <Text style={{ fontWeight: 'bold' }}>No results found.</Text>
        <Text>Try a different search.</Text>
      </div>
    );
  }

  return (
    <div style={{ display: 'flex', flexDirection: 'column', gap: '16px', padding: '16px 0' }}>
      {results.map((res, idx) => (
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
    </div>
  );
}
