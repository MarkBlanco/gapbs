// Copyright (c) 2015, The Regents of the University of California (Regents)
// See LICENSE.txt for license details

#ifndef BENCHMARK_H_
#define BENCHMARK_H_

#include <algorithm>
#include <cinttypes>
#include <functional>
#include <random>
#include <utility>
#include <vector>
#include <limits>

#include "builder.h"
#include "graph.h"
#include "timer.h"
#include "util.h"
#include "writer.h"

#define REPEAT_TRIALS 10

/*
GAP Benchmark Suite
File:   Benchmark
Author: Scott Beamer

Various helper functions to ease writing of kernels
*/


// Default type signatures for commonly used types
typedef int32_t NodeID;
typedef int32_t WeightT;
typedef NodeWeight<NodeID, WeightT> WNode;

typedef CSRGraph<NodeID> Graph;
typedef CSRGraph<NodeID, WNode> WGraph;

typedef BuilderBase<NodeID, NodeID, WeightT> Builder;
typedef BuilderBase<NodeID, WNode, WeightT> WeightedBuilder;

typedef WriterBase<NodeID, NodeID> Writer;
typedef WriterBase<NodeID, WNode> WeightedWriter;


// Used to pick random non-zero degree starting points for search algorithms
template<typename GraphT_>
class SourcePicker {
 public:
  explicit SourcePicker(const GraphT_ &g, NodeID given_source = -1, int64_t trials=REPEAT_TRIALS)
      : given_source(given_source), rng(kRandSeed), udist(0, g.num_nodes()-1),
        g_(g) {
					trials_run = 0;
					trials_to_run = trials;
				}

  NodeID PickNext() {
		if (given_source != -1)
			return given_source;
		NodeID source;
		trials_run++;
		if (trials_run==1){
			// Find new source for first run.
			do {
				source = udist(rng);
			} while (g_.out_degree(source) == 0);
			last_src = source;
		} else if (trials_run < trials_to_run){
			// Continue giving out the same source ID to repeat the trial.
			source = last_src;
		} else if (trials_run == trials_to_run){
			// Reset the trial counter, set up to move to next source
			// in the following call.
			source = last_src;// Last time we use this one
			trials_run = 0;
		}
		printf("Giving source node %u\n", source);
    return source;
  }

 private:
	NodeID last_src;
	int64_t trials_to_run;
	int64_t trials_run;
  NodeID given_source;
  std::mt19937 rng;
  std::uniform_int_distribution<NodeID> udist;
  const GraphT_ &g_;
};


// Returns k pairs with largest values from list of key-value pairs
template<typename KeyT, typename ValT>
std::vector<std::pair<ValT, KeyT>> TopK(
    const std::vector<std::pair<KeyT, ValT>> &to_sort, size_t k) {
  std::vector<std::pair<ValT, KeyT>> top_k;
  ValT min_so_far = 0;
  for (auto kvp : to_sort) {
    if ((top_k.size() < k) || (kvp.second > min_so_far)) {
      top_k.push_back(std::make_pair(kvp.second, kvp.first));
      std::sort(top_k.begin(), top_k.end(),
                std::greater<std::pair<ValT, KeyT>>());
      if (top_k.size() > k)
        top_k.resize(k);
      min_so_far = top_k.back().first;
    }
  }
  return top_k;
}


bool VerifyUnimplemented(...) {
  std::cout << "** verify unimplemented **" << std::endl;
  return false;
}


// Changes made by Mark Blanco to do multiple trials and take the min:
#define MIN(a,b) (a) > (b) ? (b) : (a)
// Calls (and times) kernel according to command line arguments
template<typename GraphT_, typename GraphFunc, typename AnalysisFunc,
         typename VerifierFunc>
void BenchmarkKernel(const CLApp &cli, const GraphT_ &g,
                     GraphFunc kernel, AnalysisFunc stats,
                     VerifierFunc verify) {
  g.PrintStats();
  double total_seconds = 0;
  Timer trial_timer;
  for (int iter=0; iter < cli.num_trials(); iter++) {
		double repeat_trial_time = std::numeric_limits<double>::max();
		pvector<NodeID> result;
		for (int repeat_trial=0; repeat_trial < REPEAT_TRIALS; repeat_trial++){
			trial_timer.Start();
			result = kernel(g);
			trial_timer.Stop();
			PrintTime("Repeat Trial Time", trial_timer.Seconds());
			repeat_trial_time = MIN(repeat_trial_time, trial_timer.Seconds());
		}
		PrintTime("Best Trial Time", repeat_trial_time);
    total_seconds += repeat_trial_time;
    if (cli.do_analysis() && (iter == (cli.num_trials()-1)))
      stats(g, result);
    if (cli.do_verify()) {
      trial_timer.Start();
      PrintLabel("Verification",
                 verify(std::ref(g), std::ref(result)) ? "PASS" : "FAIL");
      trial_timer.Stop();
      PrintTime("Verification Time", trial_timer.Seconds());
    }
  }
  PrintTime("Average Time", total_seconds / cli.num_trials());
}

#endif  // BENCHMARK_H_
