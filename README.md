# Web-Search-Engine

A full-stack C++ Web Search Engine featuring a crawler, inverted indexer with TF-IDF / BM25 scoring, REST API server, and a modern React frontend.

## Quick Start (One-Click)

### Windows (Recommended)
Double-click [`start.bat`](file:///c:/Users/sagni/c++_projects/search-engine/start.bat) or run from PowerShell:
```powershell
.\start.ps1
```

This will automatically:
1. Compile the C++ backend if not already built.
2. Build the search index if missing.
3. Install frontend dependencies (`npm install`) if needed.
4. Launch the C++ backend server on `http://localhost:8080`.
5. Launch the React frontend server on `http://localhost:5173`.
6. Open `http://localhost:5173` in your default browser.

### Stopping the Services
- Simply close the opened terminal windows, or run:
```powershell
.\stop.ps1
```
*(or double click `stop.bat`)*

## Manual Commands

### Build Backend
```powershell
cmake -B build
cmake --build build --config Release
```

### Run Commands
- **Crawl Web**: `.\build\search_engine.exe crawl https://en.wikipedia.org/wiki/C++ --max-pages 100`
- **Build Index**: `.\build\search_engine.exe index`
- **CLI Search**: `.\build\search_engine.exe search "vector database"`
- **Start Backend**: `.\build\search_engine.exe server --port 8080`
- **Start Frontend**: `cd frontend && npm run dev`
