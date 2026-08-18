#ifndef INDEX_SERIALIZER_H
#define INDEX_SERIALIZER_H

#include <string>
#include "inverted_index.h"
#include "document_store.h"

// IndexSerializer: saves and loads the InvertedIndex and DocumentStore
// to/from disk as JSON files for fast startup without re-indexing.
//
// Files produced:
//   <output_dir>/index.json       - the inverted index (term -> postings)
//   <output_dir>/documents.json   - the document store (doc_id -> metadata)
class IndexSerializer {
public:
    // Save both the index and document store to the given directory
    // Creates the directory if it doesn't exist
    static bool Save(const std::string& output_dir,
                     const InvertedIndex& index,
                     const DocumentStore& doc_store);

    // Load the inverted index from <dir>/index.json
    static bool LoadIndex(const std::string& input_dir,
                          InvertedIndex& index);

    // Load the document store from <dir>/documents.json
    static bool LoadDocumentStore(const std::string& input_dir,
                                  DocumentStore& doc_store);

    // Check if serialized index files exist in the directory
    static bool IndexExists(const std::string& dir);

private:
    static const std::string kIndexFilename;
    static const std::string kDocumentsFilename;
};

#endif // INDEX_SERIALIZER_H
