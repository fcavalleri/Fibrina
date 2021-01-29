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
    const int N_PART = 40;
    const int GRID_LEN = 500;
    const int T_MAX = 5000000;
    const int MSEC_WAIT = 0;
    const int VIEW = 5000; //visualize every VIEW time steps. FOR REAL TIME SET TO 1
    const int INSERT_PART = 5000;
    const double ACT_TRESH = 0.5;
    const double TROT_RATIO = 0.8;

// Create the Render Window
    sf::RenderWindow app(sf::VideoMode(GRID_LEN, GRID_LEN), "Simulazione Diffusione Monomeri");
    sf::Event event;

// Create the Lattice
    TLattice Lattice(GRID_LEN);

// Set Particle's Activation Treshold
    TParticle::ActivationTreshold = ACT_TRESH;

// Set the graphic context
    Lattice.SetGraphicContext(&app);

// Fill the Lattice with the Particles
    Lattice.RandomFill(2);

// Set for the DLA simulation
    Lattice.SetForDLA();

//Time Evolution
    for (int t = 0; t < T_MAX; ++t) {

        //Insert a particle every INSERT_PART steps
        if (t % INSERT_PART == 0) Lattice.RandomFill(1);
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
