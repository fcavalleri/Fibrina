#include <SFML/Graphics.hpp>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "TLattice.h"
#include "TParticle.h"
#include <vector>
#include <iostream>

void DrawParticle(TParticle &i, sf::RenderWindow *app) {
    sf::Vertex monomeroC = sf::Vertex(sf::Vector2f(i.CSite.x, i.CSite.y), sf::Color::Red);
    app->draw(&monomeroC, 1, sf::Points);

    sf::Vertex monomeroS = sf::Vertex(sf::Vector2f(i.LSite.x, i.LSite.y), sf::Color::White);
    app->draw(&monomeroS, 1, sf::Points);

    sf::Vertex monomeroD = sf::Vertex(sf::Vector2f(i.RSite.x, i.RSite.y), sf::Color::White);
    app->draw(&monomeroD, 1, sf::Points);
}

int main() {

// Define system parameters
    const int N_PART = 200;
    const int GRID_LEN = 200;

    const int T_MAX = 500000;
    const int MSEC_WAIT = 0;
    const int VIEW = 500; //visualize every VIEW time steps. FOR REAL TIME SET TO 1

    const double ZY_ROT_RATE = 1;
    const double X_ROT_RATE = 0.66;
    const double TRANSL_RATE = 0.9;

    const double ACT_TRESH = 0.001;
    const double CLO_TRESH = 0.01;

// Create the Render Window
    sf::RenderWindow app(sf::VideoMode(GRID_LEN, GRID_LEN), "Simulazione Diffusione Monomeri");
    sf::Event event{};

// Create the Lattice
    TLattice Lattice(GRID_LEN);

// Set Particle's diffusion parameters
    TParticle::ZY_ROT_RATE = ZY_ROT_RATE;
    TParticle::X_ROT_RATE = X_ROT_RATE;
    TParticle::TRANSL_RATE = TRANSL_RATE;

// Set Particle's Activation Treshold
    TParticle::ActivationTreshold = ACT_TRESH;

// Set the graphic context
    Lattice.SetGraphicContext(&app);

// Fill the Lattice with the Particles
    Lattice.RandomFill(N_PART);

// Set for the DLA simulation
    Lattice.SetForDLA();

//Time Evolution
    for (int t = 0; t < T_MAX; ++t) {

        // Possibility of slowing down the simulation
        usleep(MSEC_WAIT * 1000);
        // Clear between each time step
        app.clear();

        // Ask the Lattice to Evolve all the System by a time step
        Lattice.Evolve();

        if (t % VIEW == 0) {
            for (auto &i : Lattice.Parts) {

                //It's necessary another cycle to draw all particles, no matters if they have moved or not in this moment
                sf::Vertex monomeroC = sf::Vertex(sf::Vector2f(i.CSite.x, i.CSite.y), sf::Color::Red);
                app.draw(&monomeroC, 1, sf::Points);

                sf::Vertex monomeroS = sf::Vertex(sf::Vector2f(i.LSite.x, i.LSite.y), sf::Color::White);
                app.draw(&monomeroS, 1, sf::Points);

                sf::Vertex monomeroD = sf::Vertex(sf::Vector2f(i.RSite.x, i.RSite.y), sf::Color::White);
                app.draw(&monomeroD, 1, sf::Points);
            }

            // Display Parts positions
            app.display();
            app.pollEvent(event);
            if (event.type == sf::Event::Closed) app.close();

        }
    }
    while (app.isOpen()) while (app.pollEvent(event)) if (event.type == sf::Event::Closed) app.close();
    return 0;
}
