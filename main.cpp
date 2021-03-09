#include <SFML/Graphics.hpp>
#include <cstdio>
#include <cmath>
#include <ctime>
#include <unistd.h>
#include "TLattice.h"
#include "TParticle.h"
#include <vector>
#include <iostream>
#include <fstream>
#include "tqdm.hpp"

namespace parameters {
// Define system parameters

static constexpr int N_PART = 100;
static constexpr int GRID_LEN_X = TSite::Lx;
static constexpr int GRID_LEN_Y = TSite::Ly;

static constexpr int T_MAX = 50000;
static constexpr int MSEC_WAIT = 0;
static constexpr int VIEW = 1000; //visualize every VIEW time steps. FOR REAL TIME SET TO 1

#define DISPLAY_SIMULATION false

static constexpr double ZY_ROT_RATE = 1;
static constexpr double X_ROT_RATE = 0.66;
static constexpr double TRANSL_RATE = 0.9;

static constexpr double LEN_WIDHT_RATIO = 0.1;

static constexpr double ACT_TRESH = 0.0012;
static constexpr double CLO_TRESH = 0.003;
static constexpr double DL2YL_RATE = 0;
}

int main() {
  using namespace parameters;

  // Files where save analysis

    std::string rawdataP("RawDataParameters");
    rawdataP.append("_210309_1").append(".txt");
    std::ofstream outputP(rawdataP);

    outputP << "Fibrin Aggregation Simulation: parameter's set" << "\n\n" <<
    "Grid dimension x: " << GRID_LEN_X * 0.5 * 43 * 0.001 << "um (" << GRID_LEN_X << " steps) \n" <<
    "Grid dimension y: " << GRID_LEN_Y * 0.5 * sqrt(3) * 43 * 0.001 << " um (" << GRID_LEN_Y << " steps) \n" <<
    "Initial number of particles: " << N_PART << "\n" <<
    "Total time of simulation: " << T_MAX * 0.00001 << " s (" << T_MAX << " steps) \n\n" <<
    "Particles parameters" << "\n\n" <<
    "Translational Diffusion Rate: " << TRANSL_RATE <<
    "ZY Rotational Diffusion Rate: " << ZY_ROT_RATE <<
    "X Rotational Diffusion Rate: " << X_ROT_RATE <<
    "Simultaneous A & B sites Activation Rate: " << ACT_TRESH <<
    "YL to DS Closing rate: " << CLO_TRESH;




    std::string rawdata("RawData_Tmax");
    rawdata.append("_210309_1").append(".txt");
    std::ofstream output(rawdata);

    std::string nYLnDL("nYlnDL_Tmax");
    nYLnDL.append(std::to_string(T_MAX)).append("_Every")
            .append(std::to_string(VIEW)).append(".txt");
    std::ofstream outputYLDL(nYLnDL);

#if DISPLAY_SIMULATION
  // Create the Render Window
  sf::RenderWindow app(sf::VideoMode(GRID_LEN_X, GRID_LEN_Y), "Simulazione Aggregazione Fibrina");
#endif

// Create the Lattice
  TLattice Lattice;

  //TODO: declare these parameters as constexpr inside TParticle
// Set Particle's diffusion parameters
  TParticle::ZY_ROT_RATE = ZY_ROT_RATE;
  TParticle::X_ROT_RATE = X_ROT_RATE;
  TParticle::TRANSL_RATE = TRANSL_RATE;

// Set Particle's Activation and Closing Treshold
  TParticle::ACT_TRESH = ACT_TRESH;
  TParticle::CLO_TRESH = CLO_TRESH;
  TParticle::DL2YL_RATE = DL2YL_RATE;

// Fill the Lattice with the Particles
  Lattice.RandomFill(N_PART);

// Set for the DLA simulation
  Lattice.SetForDLA();

// Time Evolution
  for (int t : tq::trange(T_MAX)) {

    // Possibility of slowing down the simulation
    if constexpr(MSEC_WAIT)usleep(MSEC_WAIT * 1000);

    // Ask the Lattice to Evolve all the System by a time step
    Lattice.Evolve();

    //Visualize Lattice every VIEW steps
    if (t % VIEW == 0) {

#if DISPLAY_SIMULATION
      app.clear();
      app.draw(Lattice);
#endif

        outputYLDL << Lattice.nYL << "\t" << Lattice.nDL << "\n";

        output << "\n";
        for (auto i : Lattice.Parts) {
            if (i.mob != TParticle::MobState::FREE) {
                output << i.LSite << " " << i.CSite << " " << i.RSite << " " << i.Spin << "\n";
            }
        }

#if DISPLAY_SIMULATION
      // Display Parts positions
      app.display();
      for (sf::Event event; app.pollEvent(event);)

        if (event.type == sf::Event::Closed) {
          app.close();
          return 0;
        }
#endif

    }
  }
#if DISPLAY_SIMULATION
  while (app.isOpen()) {
    for (sf::Event event; app.pollEvent(event);) {
      if (event.type == sf::Event::Closed)
        app.close();
      else {
        app.clear();
        app.draw(Lattice);
        app.display();
      }
    }
  }
#endif
  return 0;
}
