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

static int N_PART = 200;
static constexpr int GRID_LEN_X = TSite::Lx;
static constexpr int GRID_LEN_Y = TSite::Ly;

static constexpr int T_MAX = 220000;
static constexpr int N_FIX_MAX = 50;
static constexpr int MSEC_WAIT = 10;
static constexpr int VIEW = 1; //visualize every VIEW time steps. FOR REAL TIME SET TO 1

#define DISPLAY_SIMULATION true

static constexpr double ZY_ROT_RATE = 1;
static constexpr double X_ROT_RATE = 0.66;
static constexpr double TRANSL_RATE = 0.9;

static constexpr double LEN_WIDHT_RATIO = 0.3;

static constexpr double ACT_TRESH = 0.001; //0.0012;
static double CLO_TRESH = 0.1;
static constexpr double DL2YL_RATE = 0;

}

int main(int argc, char*argv[]) {
  using namespace parameters;

  // Files where save analysis

    std::string strCLO_TRESH = "";
    std::string iteration = {};

    if (argc>=2) {
        strCLO_TRESH = std::string(argv[1]);
        CLO_TRESH = std::stod(strCLO_TRESH);
    }

    if (argc>=3) {
        N_PART = std::stoi(std::string(argv[2]));
    }

    if (argc>=4) {
        iteration =std::string(argv[3]);
    }

    std::string rawdataP("RawDataParameters_");
    rawdataP.append(iteration).append("_N=").append(std::to_string(N_PART)).append("_cl=").append(strCLO_TRESH).append(".txt");

    std::ofstream outputP(rawdataP);

    outputP << "Fibrin Aggregation Simulation: parameter's set" << "\n\n" <<
    "Grid dimension x: " << GRID_LEN_X * 0.25 * 45 * 0.001 << "um (" << GRID_LEN_X << " steps) \n" <<
    "Grid dimension y: " << GRID_LEN_Y * 0.25 * sqrt(3) * 45 * 0.001 << " um (" << GRID_LEN_Y << " steps) \n" <<
    "Initial number of particles: " << N_PART << "\n" <<
    "Initial free monomer concentration: " << N_PART / (GRID_LEN_X * 0.25 * 45 * 0.001 * GRID_LEN_Y * 0.25 * sqrt(3) * 45 * 0.001) << " N/um^2\n" <<
    "Reinjection every link event to keep constant free particles concentration: FALSE \n" <<
    "\nParticles parameters" << "\n\n" <<
    "Translational Diffusion Rate: " << TRANSL_RATE << "\n" <<
    "ZY Rotational Diffusion Rate: " << ZY_ROT_RATE << "\n" <<
    "X Rotational Diffusion Rate: " << X_ROT_RATE << "\n" <<
    "Simultaneous A & B sites Activation Rate: " << ACT_TRESH << "\n" <<
    "DS to YL in Formation Correction rate: " << DL2YL_RATE << "\n" <<
    "YL to DS Closing rate: " << CLO_TRESH << "\n\n" <<
    "Raw data taken every " << VIEW * 0.00001 << " s (" << VIEW << " steps)" << std::endl;

    std::string rawdata("RawData_");
    rawdata.append(iteration).append("_N=").append(std::to_string(N_PART)).append("_cl=").append(strCLO_TRESH).append(".txt");
    std::ofstream output(rawdata);

    std::string nYLnDL("nYlnDL_");
    nYLnDL.append(iteration).append("_N=").append(std::to_string(N_PART)).append("_").append(strCLO_TRESH).append(".txt");
    std::ofstream outputYLDL(nYLnDL);

    std::string fincor("FinalCoord_");
    fincor.append(iteration).append("_N=").append(std::to_string(N_PART)).append("_").append(strCLO_TRESH).append(".txt");
    std::ofstream finoutput(fincor);

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

// Set a limit for simulation
  Lattice.MAX_Nfix=N_FIX_MAX;

// Set Raw Data Header
  output << "Time\tPol\tXc\tYc\tOrien"; //\tXl\tYl\tXr\tYr\n";
  outputYLDL <<"Time\tnYl\tnDS\n";

// Time Evolution
  for (int t : tq::trange(T_MAX)) {

    // Ask the Lattice to Evolve all the System by a time step
    if (Lattice.Evolve()) {
        outputP << "Total time of simulation: " << t * 0.00001 << " s (" << t << " steps) \n" <<
                "Number of monomers in the aggregate: " << Lattice.Nfix;

        for (auto i : Lattice.Parts) {
            if (i.mob != TParticle::MobState::FREE) {
                finoutput << i.CSite << "\t" << i.LSite << "\t" << i.RSite << "\t" << i.Spin << std::endl;
            }
        }

        return 0;
    }

    //Visualize Lattice every VIEW steps
    if (t % VIEW == 0) {

        // Possibility of slowing down the simulation
        if constexpr(MSEC_WAIT != 0)usleep(MSEC_WAIT * 1000);

#if DISPLAY_SIMULATION
      app.clear();
      app.draw(Lattice);
#endif

        outputYLDL << t << "\t" << Lattice.nYL << "\t" << Lattice.nDL << std::endl;

        output << "\n";
        for (auto i : Lattice.Parts) {
            if (i.mob != TParticle::MobState::FREE) {
                output << t << "\t1\t "<< i.CSite << "\t" << i.Spin /*<< "\t" << i.LSite << "\t" << i.RSite*/ << std::endl;
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

  outputP << "Total time of simulation: " << T_MAX * 0.00001 << " s (" << T_MAX << " steps) \n" <<
  "Number of monomers in the aggregate: " << Lattice.Nfix;

    for (auto i : Lattice.Parts) {
        if (i.mob != TParticle::MobState::FREE) {
            finoutput << i.CSite << "\t" << i.LSite << "\t" << i.RSite << "\t" << i.Spin << std::endl;
        }
    }

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