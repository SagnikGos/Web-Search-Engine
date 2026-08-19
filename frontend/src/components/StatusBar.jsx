import React from 'react';
import { Text } from 'retro-react';

export default function StatusBar({ indexReady, isIndexing, globalStats }) {
  return (
    <div style={{ display: 'flex', borderTop: '2px solid #ffffff', borderLeft: '2px solid #ffffff', borderBottom: '2px solid #808080', borderRight: '2px solid #808080', backgroundColor: '#c0c0c0', padding: '2px', marginTop: 'auto' }}>
      <div className="status-bar-item" style={{ flex: 1 }}>
        <Text>{isIndexing ? '● Indexing...' : (indexReady ? '● Index ready' : '● Index status unknown')}</Text>
      </div>
      <div className="status-bar-item" style={{ width: '200px' }}>
        <Text>{globalStats ? globalStats.total_pages.toLocaleString() : 'N/A'} pages indexed</Text>
      </div>
      <div className="status-bar-item" style={{ width: '200px' }}>
        <Text>{globalStats ? globalStats.total_chars.toLocaleString() : 'N/A'} chars indexed</Text>
      </div>
    </div>
  );
}
