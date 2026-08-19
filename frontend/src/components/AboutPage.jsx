import React from 'react';
import { Button } from 'retro-react';

export default function AboutPage({ setView }) {
  return (
    <div style={{ display: 'flex', flexDirection: 'column', alignItems: 'center', width: '100%', maxWidth: '800px', margin: '40px auto', padding: '0 24px', boxSizing: 'border-box' }}>
      <div style={{ width: '100%', marginBottom: '32px' }}>
        <Button onClick={() => setView('search')} style={{ fontWeight: 'bold', padding: '4px 16px', marginBottom: '24px' }}>
          &lt; Back to Search
        </Button>
        <h1 style={{ fontSize: '40px', fontWeight: 'bold', margin: '0 0 8px 0', color: 'var(--accent-blue)' }}>About Search++</h1>
        <p style={{ fontSize: '20px', color: 'var(--text-muted)', margin: 0 }}>A high-performance C++ search engine.</p>
      </div>

      <div style={{ 
        width: '100%', 
        padding: '32px', 
        backgroundColor: '#ffffff',
        borderTop: '2px solid var(--border-dark)',
        borderLeft: '2px solid var(--border-dark)',
        borderRight: '2px solid var(--border-light)',
        borderBottom: '2px solid var(--border-light)',
        marginBottom: '24px',
        lineHeight: '1.6'
      }}>
        <h2 style={{ fontSize: '20px', color: 'var(--accent-blue)', marginTop: 0 }}>Architecture</h2>
        <p>
          Search++ is built with a custom C++ backend optimized for speed and efficiency. It features a multithreaded crawler, a custom HTML parser, and an in-memory inverted index for lightning-fast term lookups.
        </p>

        <h2 style={{ fontSize: '20px', color: 'var(--accent-blue)', marginTop: '24px' }}>How It Works</h2>
        <ol style={{ paddingLeft: '20px', margin: '16px 0' }}>
          <li style={{ marginBottom: '8px' }}><strong>Crawling:</strong> The backend crawls starting from a seed URL, fetching pages concurrently.</li>
          <li style={{ marginBottom: '8px' }}><strong>Parsing & Tokenization:</strong> HTML is stripped, and the text is tokenized. Stop words are filtered out to keep the index lean.</li>
          <li style={{ marginBottom: '8px' }}><strong>Indexing:</strong> Words are added to an inverted index, mapping terms to the documents they appear in.</li>
          <li style={{ marginBottom: '0' }}><strong>Searching:</strong> Queries are tokenized and scored using a TF-IDF inspired ranking algorithm, returning results in milliseconds.</li>
        </ol>

        <h2 style={{ fontSize: '20px', color: 'var(--accent-blue)', marginTop: '24px' }}>Frontend</h2>
        <p style={{ margin: 0 }}>
          The frontend is a React application styled with a modern-retro aesthetic, communicating with the C++ backend via a REST API to provide a seamless, snappy search experience.
        </p>
      </div>
    </div>
  );
}
