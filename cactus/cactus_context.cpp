#include "cactus.h"
#include "common.h" // For common_sampler_free, common_sampler_init

namespace cactus {

/**
 * @brief Destructor for cactus_context
 * 
 * Cleans up resources, including the sampling context
 */
cactus_context::~cactus_context() {
    if (ctx_sampling != nullptr) {
        common_sampler_free(ctx_sampling);
        ctx_sampling = nullptr; // Good practice to nullify after freeing
    }
    // Note: llama_init (which holds model and ctx shared_ptrs) 
    // will automatically clean up model and ctx when cactus_context is destroyed.
}

/**
 * @brief Rewinds the context to start a new generation
 * 
 * Resets internal state to prepare for a new generation task
 */
void cactus_context::rewind() {
    is_interrupted = false;
    is_predicting = false; // Ensure predicting flag is reset too
    params.antiprompt.clear();
    params.sampling.grammar.clear();
    num_prompt_tokens = 0;
    num_tokens_predicted = 0;
    generated_text = "";
    generated_text.reserve(params.n_ctx); // Reserve based on loaded context size
    generated_token_probs.clear();
    truncated = false;
    stopped_eos = false;
    stopped_word = false;
    stopped_limit = false;
    stopping_word = "";
    incomplete = false;
    n_remain = 0;
    n_past = 0;
    embd.clear(); // Clear the embedding vector
    if (ctx_sampling) {
        // Reset sampler state if it exists
        // common_sampler_reset(ctx_sampling); // Assuming such a function exists or is needed
    }
    // params.sampling.n_prev = n_ctx; // This might be set dynamically or during initSampling
}

/**
 * @brief Initializes the sampling context
 * 
 * @return true if initialization succeeded, false otherwise
 */
bool cactus_context::initSampling() {
    if (ctx_sampling != nullptr) {
        common_sampler_free(ctx_sampling);
        ctx_sampling = nullptr;
    }
    if (model) { // Ensure model is loaded before initializing sampler
        ctx_sampling = common_sampler_init(model, params.sampling);
        if (ctx_sampling) {
             params.sampling.n_prev = n_ctx; // Set n_prev based on context size
        }
    } else {
        LOG_ERROR("Cannot initialize sampling context: model is not loaded.");
        return false;
    }
    return ctx_sampling != nullptr;
}


} // namespace cactus 