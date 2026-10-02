#include "aiws/processing_core.hpp"
#include "aiws/chunking_strategy.hpp"
#include "aiws/retrieval_engine.hpp"
#include "aiws/context_builder.hpp"
#include "aiws/document.hpp"
#include "aiws/workspace.hpp"

#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>
#include <cassert>

// Student-written M2 tests
//
// Add your own tests to this file. Your tests are part of the submitted work
// and are evaluated under Student-Written Testing & Validation.
//
// Do not modify tests/public_tests.cpp.
//
// Your tests should exercise important M2 behavior beyond the supplied public
// tests. Consider default compatibility, custom strategies, runtime dispatch,
// invalid configuration, ownership/move behavior, and component interactions.

class CustomChunkingStrategy : public aiws::ChunkingStrategy {
    public :
        std::vector<aiws::Chunk> chunk(const aiws::Document& document, std::size_t document_order) const override {
            aiws::Chunk chunk;
            chunk.id = document.id() + "-custom";
            chunk.document_id = document.id();
            chunk.document_order = document_order;
            chunk.sequence = 0;
            chunk.text = "custom strategy output";
            chunk.token_count = 3;
            chunk.source_begin = 0;
            chunk.source_end = document.text().size();

            return {chunk};
        }
};

void test_custom_chunking_strategy_is_used() {
    auto chunking = std::make_unique<CustomChunkingStrategy>();
    auto retrieval = std::make_unique<aiws::RetrievalEngine>();
    auto context = std::make_unique<aiws::ContextBuilder>();

    aiws::ProcessingCore core(std::move(chunking), std::move(retrieval), std::move(context));

    aiws::Workspace workspace;
    workspace.add_document(aiws::Document("doc1", "Test Document", "Original document text"));

    core.rebuild(workspace);

    assert(core.chunk_count() == 1);
    assert(core.chunks()[0].id == "doc1-custom");
    assert(core.chunks()[0].text == "custom strategy output");
}

void test_null_strategy_rejected() {
    bool threw = false;

    try {
        std::unique_ptr<aiws::ChunkingStrategy> chunking;
        auto retrieval = std::make_unique<aiws::RetrievalEngine>();
        auto context = std::make_unique<aiws::ContextBuilder>();

        aiws::ProcessingCore core(std::move(chunking), std::move(retrieval), std::move(context));
    } catch (const std::invalid_argument&) {
        threw = true;
    }

    assert(threw);
}

int main() {
    // TODO: Add your own M2 tests here.
    test_custom_chunking_strategy_is_used();
    test_null_strategy_rejected();

    std::cout << "All student tests passed." << std::endl;
    return 0;
}