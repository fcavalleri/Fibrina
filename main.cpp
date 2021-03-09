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

static constexpr int T_MAX = 500000;
static constexpr int MSEC_WAIT = 0;
static constexpr int VIEW = 100; //visualize every VIEW time steps. FOR REAL TIME SET TO 1

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
  FILE *fp1 = fopen("NfixRg", "w"); //particles in aggregate and Gyration Radius
  FILE *fp2 = fopen("nYLnDL", "w"); //number of YL and DL links

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

      fprintf(fp2, "%d \t %d \t %d \n", t, Lattice.nYL, Lattice.nYL);
      fflush(fp2);

#if DISPLAY_SIMULATION
      app.clear();
      app.draw(Lattice);
#endif

      //TODO: move this computation elsewhere (e.g. in TLattice)
      int Nfix = 0;
      double Rg = 0;
      double Rg2 = 0;
      double xmean = 0;
      double ymean = 0;
      double xmean2 = 0;
      double ymean2 = 0;

      for (auto &i : Lattice.Parts) {
        if (i.mob != TParticle::MobState::FREE) {
          // Compute baricenter
          xmean = xmean + (i.CSite.x) * 0.5;
          ymean = ymean + i.CSite.y * 0.5 * sqrt(3);
          xmean2 = xmean2 + (i.CSite.x) * 0.5*(i.CSite.x) * 0.5;
          ymean2 = ymean2 + i.CSite.y * 0.5 * sqrt(3)*i.CSite.y * 0.5 * sqrt(3);
          Nfix = Nfix + 1;
        }
      }

        //fprintf(fp1, "%e \t %e \t %e \t %e \t %d \n", xmean2, Lattice.sumx2, ymean2, Lattice.sumy2, Nfix);
        //fflush(fp1);

      Rg2=xmean2/Nfix - (xmean/Nfix)*(xmean/Nfix) + ymean2/Nfix - (ymean/Nfix)*(ymean/Nfix);

      xmean = xmean / Nfix;
      ymean = ymean / Nfix;

      // Compute Rg^2
      for (auto &i : Lattice.Parts) {
        if (i.mob != TParticle::MobState::FREE) {
          Rg = Rg + (i.CSite.x * 0.5 - xmean) * (i.CSite.x * 0.5 - xmean)
             + (i.CSite.y * 0.5 * sqrt(3) - ymean) * (i.CSite.y * 0.5 * sqrt(3) - ymean);
        }
      }

      Rg = Rg / Nfix;

      fprintf(fp1, "%e \t %e \t %e \t %d \n", Rg, Rg2, Lattice.Rg2, Nfix);
      fflush(fp1);

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
