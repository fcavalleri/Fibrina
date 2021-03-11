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
#include <string>

const std::string currentDateTime();

namespace parameters {
// Define system parameters

static constexpr int N_PART = 75;
static constexpr int GRID_LEN_X = TSite::Lx;
static constexpr int GRID_LEN_Y = TSite::Ly;

static constexpr int T_MAX = 700000;
static constexpr int MSEC_WAIT = 0;
static constexpr int VIEW = 5000; //visualize every VIEW time steps. FOR REAL TIME SET TO 1

#define DISPLAY_SIMULATION false

static constexpr double ZY_ROT_RATE = 1;
static constexpr double X_ROT_RATE = 0.66;
static constexpr double TRANSL_RATE = 0.9;

static constexpr double LEN_WIDHT_RATIO = 0.1;

static constexpr double ACT_TRESH = 0.0012;
static constexpr double CLO_TRESH = 0.004;
static constexpr double DL2YL_RATE = 0;

}

int main(int argc, char*argv[] ) {
  using namespace parameters;

  // Files where save analysis

    std::string extension = "";
    if (argc==2) extension = std::string(argv[1]);

    std::string rawdataP("RawDataParameters_");
    rawdataP.append(currentDateTime()).append("_").append(extension).append(".txt");
    std::ofstream outputP(rawdataP);

    outputP << "Fibrin Aggregation Simulation: parameter's set" << "\n\n" <<
    "Grid dimension x: " << GRID_LEN_X * 0.5 * 43 * 0.001 << "um (" << GRID_LEN_X << " steps) \n" <<
    "Grid dimension y: " << GRID_LEN_Y * 0.5 * sqrt(3) * 43 * 0.001 << " um (" << GRID_LEN_Y << " steps) \n" <<
    "Initial number of particles: " << N_PART << "\n" <<
    "Reinjection every link event to keep constant free particles concentration: TRUE \n" <<
    "Total time of simulation: " << T_MAX * 0.00001 << " s (" << T_MAX << " steps) \n\n" <<
    "Particles parameters" << "\n\n" <<
    "Translational Diffusion Rate: " << TRANSL_RATE << "\n" <<
    "ZY Rotational Diffusion Rate: " << ZY_ROT_RATE << "\n" <<
    "X Rotational Diffusion Rate: " << X_ROT_RATE << "\n" <<
    "Simultaneous A & B sites Activation Rate: " << ACT_TRESH << "\n" <<
    "DS to YL in Formation Correction rate: " << DL2YL_RATE << "\n" <<
    "YL to DS Closing rate: " << CLO_TRESH << "\n\n" <<
    "Raw data taken every " << VIEW * 0.00001 << " s (" << VIEW << " steps):" << std::endl;

    std::string rawdata("RawData_");
    rawdata.append(currentDateTime()).append("_").append(extension).append(".txt");
    std::ofstream output(rawdata);

    std::string nYLnDL("nYlnDL_");
    nYLnDL.append(currentDateTime()).append("_").append(extension).append(".txt");
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

// Set Raw Data Header
  output << "Time\tXc\tYc\tOrientation\n";
  outputYLDL <<"Time\tnYl\tnDS\n";

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

        outputYLDL << t << "\t" << Lattice.nYL << "\t" << Lattice.nDL << std::endl;

        for (auto i : Lattice.Parts) {
            if (i.mob != TParticle::MobState::FREE) {
                output << t << "\t"<< i.CSite << "\t" << i.Spin << std::endl;
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
  outputP << "\n Final Number of particles in polimer: " << Lattice.Nfix;
  return 0;
}

const std::string currentDateTime() {
    time_t     now = time(0);
    struct tm  tstruct;
    char       buf[80];
    tstruct = *localtime(&now);
    // Visit http://en.cppreference.com/w/cpp/chrono/c/strftime
    // for more information about date/time format
    strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct);

    return buf;
}