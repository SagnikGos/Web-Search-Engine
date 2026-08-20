# search++

[![C++ Standard](https://img.shields.io/badge/C%2B%2B-17-blue.svg)](https://en.cppreference.com/w/cpp/17)
[![React](https://img.shields.io/badge/Frontend-React%2019%20%2B%20Vite-61dafb.svg)](https://react.dev/)
[![Build System](https://img.shields.io/badge/Build-CMake-064F8C.svg)](https://cmake.org/)
[![License](https://img.shields.io/badge/License-MIT-green.svg)](LICENSE)

A high-performance, full-stack web search engine built from first principles in modern **C++17**, featuring an in-memory inverted index, TF-IDF ranking, concurrent crawling, and a retro **Windows 95 React frontend**.

---

## ⚡ Highlights

* **Microsecond Search:** Sub-millisecond query evaluation ($100\,\mu\text{s} - 1.5\,\text{ms}$) via in-memory postings maps and cache-friendly vector stores.
* **Concurrent Web Crawler:** Multi-threaded worker pool utilizing `std::condition_variable` and synchronized URL queues with cycle prevention.
* **HTML5 DOM Extraction:** Powered by Google's C99 Gumbo parser to extract clean body text, titles, and resolve relative links.
* **Relevance Scoring:** Length-normalized Term Frequency (TF) combined with logarithmic Inverse Document Frequency (IDF).
* **Dynamic Snippets & UTF-8 Safety:** Contextual preview extraction around matched terms with bitwise UTF-8 continuation byte correction.
* **Zero-Downtime Hot Swapping:** Atomic index reloads (`std::shared_ptr`) upon crawl completion without interrupting active searches.
* **Nostalgic UI:** Windows 95 retro theme built with React 19, pixel typography, and live crawl metrics.

---

## 🏗️ Architecture

```
[ Web Graph ] 
      │ (Concurrent HTTP / CPR)
      ▼
[ Crawler (ThreadPool + UrlQueue) ] ──► [ HTML Parser (Gumbo DOM) ]
                                                        │
                                                        ▼
[ Search API (cpp-httplib) ] ◄── [ Inverted Index & TF-IDF Ranker ]
      ▲                                                 │ (JSON Serialization)
      │                                                 ▼
[ Windows 95 React UI ]                        [ Disk Persistence (~11ms Boot) ]
```

---

## 🚀 Quick Start

### Prerequisites
* **C++ Compiler:** MSVC (Visual Studio 2019+), GCC 9+, or Clang 10+ (supporting C++17)
* **CMake:** Version 3.20 or newer
* **Node.js & npm:** Node 18+ (for frontend)

---

### One-Click Launch (Windows)

```powershell
# Build both backend & frontend
.\build.ps1

# Start backend (port 8080) & frontend (port 5173)
.\start.ps1

# Stop all running services
.\stop.ps1
```

---

### Manual Build & Execution

#### 1. Build Backend
```bash
cmake -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

#### 2. Build Frontend
```bash
cd frontend
npm install
npm run build
```

#### 3. CLI Commands
```bash
# Crawl web pages
./build/search_engine crawl https://en.wikipedia.org/wiki/Computer_science --max-pages 50

# Build inverted index
./build/search_engine index

# Query directly from terminal
./build/search_engine search "virtual memory"

# Run HTTP API Server
./build/search_engine server --port 8080
```

---

## 📊 Performance

| Metric | Measurement |
| :--- | :--- |
| **Query Latency** | $100\,\mu\text{s} - 1.5\,\text{ms}$ per query |
| **Cold Boot Startup** | $\sim 11\,\text{ms}$ (deserializing pre-built index) |
| **Crawl Throughput** | $10 - 25\,\text{pages/sec}$ (8 worker threads) |
| **Memory Footprint** | $\sim 40 - 80\,\text{MB}$ RAM per 1,000 pages |

---

## 🛠️ Tech Stack

* **Core Engine:** C++17, STL (`unordered_map`, `vector`, `atomic`, `threads`)
* **HTTP / Networking:** [CPR](https://github.com/libcpr/cpr) (libcurl/WinSSL), [cpp-httplib](https://github.com/yhirose/cpp-httplib)
* **HTML Parsing:** [Google Gumbo Parser](https://github.com/google/gumbo-parser)
* **JSON Serialization:** [nlohmann/json](https://github.com/nlohmann/json)
* **Frontend:** React 19, Vite, Emotion CSS, `retro-react`
* **Build System:** CMake (with `FetchContent`)

---

## 📁 Repository Structure

```
search-engine/
├── src/
│   ├── crawler/       # Multi-threaded crawler, HTML DOM parser, URL queue
│   ├── indexer/       # Inverted index, tokenizer, stop-word filter, serializer
│   ├── search/        # TF-IDF relevance ranker, query processor, UTF-8 snippets
│   ├── server/        # cpp-httplib REST API route handlers
│   └── main.cpp       # CLI router & server orchestrator
├── frontend/          # React 19 single-page application (Windows 95 UI)
├── data/              # Storage for crawled pages and serialized index files
└── CMakeLists.txt     # Root CMake configuration
```

---

## 📄 License

This project is licensed under the [MIT License](LICENSE).

