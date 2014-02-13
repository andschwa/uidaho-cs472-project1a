/* Copyright 2014 Andrew Schwartzmeyer
 *
 * Source file for derived genetic algorithm class
 */

#include <algorithm>
#include <iostream>
#include <iterator>

#include "genetic_algorithm.hpp"
#include "random_generator.hpp"

std::array <parameter, dimension> Genetic::mutate(const Individual * subject) const {
  // unit Gaussian distribution for delta
  std::normal_distribution<parameter> delta(mean, stddev);
  std::array <parameter, dimension> mutation = subject->solution;
  for (parameter & value : mutation) {
    parameter d = delta(rg->engine);
    // std::cout << d << ' ';
    subject->mutate(value, d);
  }
  // std::cout << '\n';
  return mutation;
}

const Individual Genetic::solve() const {
  // create initial population
  std::array<Individual, pop_size> population;
  population.fill(problem->potential());
  Individual best;

  for (long i = 0; i < problem->iterations; i++) {
    std::array<Individual, pop_size> mating_pool;
    mating_pool = population;

    // calculate fitnesses of individuals in population
    std::array<parameter, pop_size> weights;
    for (int i = 0; i < pop_size; i++)
      weights[i] = problem->fitness(&population[i]);

    int max = std::distance(weights.begin(),
			    std::max_element(weights.begin(), weights.end()));
    best = population[max];
    if (problem->fitness(&best) > problem->goal) goto finished;
    // std::cout << problem->fitness(&best) << '\n';

    // setup roulette wheel selection
    std::discrete_distribution<> mate(weights.begin(), weights.end());

    // selection and mutation stage
    for (Individual & individual : mating_pool) {
      // select
      individual = population[mate(rg->engine)];
      // mutate
      std::array <parameter, dimension> mutation = mutate(&individual);
      individual.solution = mutation;
    }
    population = mating_pool;
  }
 finished:
  return best;
}