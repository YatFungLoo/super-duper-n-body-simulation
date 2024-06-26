#include <iostream>
#include <fstream>
#include <string>

#include <SFML/Graphics.hpp>
#include <unistd.h>
#include <math.h>

/*
 * Using an arbitrary unit system.
 * G is approximately the gaussian gravitational constant, in order to make testing easier if necessary
 */

#define G 0.0172
#define dt 1E-6
#define t_max 1E-2

struct Particle
{
    double m; // mass of the particle
    double x; // x-coordinate of the position
    double y; // y-coordinate of the position
    double vx;
    double vy;
    double ax;
    double ay;
};

void init_particles(struct Particle *particle_1, struct Particle *particle_2);
void update_accelerations(struct Particle *particle_1, struct Particle *particle_2); // uses Law of Universal Gravitation
void update_velocities(struct Particle *particle_1, struct Particle *particle_2);    // uses Euler scheme
void update_positions(struct Particle *particle_1, struct Particle *particle_2);     // uses Euler scheme

// Output related.
void printstep(double t, struct Particle particle_1, struct Particle particle_2, std::ofstream *outfile);
void dispBody(double t, struct Particle *particle_1, struct Particle *particle_2, sf::RenderWindow *window);
sf::Font font;
bool dispMode = false;
bool printMode = false;

int main(int argc, char **argv)
{

    if (argc >= 2)
    {
        std::cout << "Usage: " << argv[0] << " {output file}" << std::endl;
        return 1;
    }
    if (argc == 1) // SFML mode
    {
        std::cout << "Display mode" << std::endl;
        dispMode = true;
        printMode = false;
    }
    if (argc == 2) // Output to file
    {
        std::cout << "Output mode" << std::endl;
        dispMode = false;
        printMode = true;
    }

    struct Particle particle_1;
    struct Particle particle_2;
    double t = 0.0;
    init_particles(&particle_1, &particle_2);

    if (printMode == true)
    {
        std::ofstream outfile;
        std::string filename = argv[1];
        outfile.open(filename);
        for (t = 0; t <= t_max; t += dt)
        {
            printstep(t, particle_1, particle_2, &outfile);

            update_accelerations(&particle_1, &particle_2);
            update_velocities(&particle_1, &particle_2);
            update_positions(&particle_1, &particle_2);
        }
        outfile.close();
    } // printMode

    if (dispMode == true)
    {
        sf::RenderWindow window(sf::VideoMode(1200, 600), "2Body");
        // Load font.
        if (!font.loadFromFile("Candy.ttf"))
        {
            std::cout << "can't load font" << std::endl;
        }
        // while (window.isOpen() && t <= t_max)
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                if (event.type == sf::Event::Closed)
                {
                    window.close();
                }
            }
            dispBody(t, &particle_1, &particle_2, &window);

            update_accelerations(&particle_1, &particle_2);
            update_velocities(&particle_1, &particle_2);
            update_positions(&particle_1, &particle_2);

            t += dt;
        }
        window.close();
    } // dispMode

    printf("Simulation complete.\n");
    return 0;
}

void init_particles(struct Particle *particle_1, struct Particle *particle_2)
{

    particle_1->m = 1;
    particle_2->m = 1;

    particle_1->x = 0;
    particle_1->y = 0;
    particle_2->x = 0.1;
    particle_2->y = 0;

    particle_1->vx = 0;
    particle_1->vy = 0;
    particle_2->vx = 0;
    particle_2->vy = 0.1;
}

void update_accelerations(struct Particle *particle_1, struct Particle *particle_2)
{

    double delta_x_21 = particle_2->x - particle_1->x; // horizontal distance of particle 2 with respect to particle 1
    double delta_y_21 = particle_2->y - particle_1->y; // vertical distance of particle 2 with respect to particle 1

    double r2 = delta_x_21 * delta_x_21 + delta_y_21 * delta_y_21; // r^2 = x^2 + y^2
    double r3 = pow(r2, 1.5);                                      // (r^2)^(1.5) = r^3

    double m1 = particle_1->m;
    double m2 = particle_2->m;

    double ax_1 = G * m2 / r3 * delta_x_21; // acceleration of particle 1 in the x-direction
    double ay_1 = G * m2 / r3 * delta_y_21;
    double ax_2 = -G * m1 / r3 * delta_x_21; // Negative sign accounts for the fact that delta_x_12 = - delta_x_21
    double ay_2 = -G * m1 / r3 * delta_y_21;

    particle_1->ax = ax_1;
    particle_1->ay = ay_1;
    particle_2->ax = ax_2;
    particle_2->ay = ay_2;
}

void update_velocities(struct Particle *particle_1, struct Particle *particle_2)
{

    particle_1->vx = particle_1->vx + particle_1->ax * dt; // approximation of v is o(dt^2)
    particle_1->vy = particle_1->vy + particle_1->ay * dt;
    particle_2->vx = particle_2->vx + particle_2->ax * dt;
    particle_2->vy = particle_2->vy + particle_2->ay * dt;
}

void update_positions(struct Particle *particle_1, struct Particle *particle_2)
{

    double dt2 = pow(dt, 2);

    particle_1->x = particle_1->x + particle_1->vx * dt + 0.5 * particle_1->ax * dt2; // approximation of r is o(dt^3)
    particle_1->y = particle_1->y + particle_1->vy * dt + 0.5 * particle_1->ay * dt2;
    particle_2->x = particle_2->x + particle_2->vx * dt + 0.5 * particle_2->ax * dt2;
    particle_2->y = particle_2->y + particle_2->vy * dt + 0.5 * particle_2->ay * dt2;
}

void dispBody(double t, Particle *particle_1, Particle *particle_2, sf::RenderWindow *window)
{
    window->clear(sf::Color::White);

    const auto scale = 10000;
    const auto offset_x = -100;
    const auto offset_y = 200;
    // const auto scale = 1000000;
    sf::Text text(std::to_string(t), font, 20);
    text.setFillColor(sf::Color::Red);
    window->draw(text);

    sf::CircleShape shape1(5);
    shape1.setFillColor(sf::Color::Blue);
    auto x1 = particle_1->x * scale;
    auto y1 = particle_1->y * scale;
    shape1.setPosition(x1 + offset_x, y1 + offset_y);
    window->draw(shape1);

    sf::CircleShape shape2(5);
    shape2.setFillColor(sf::Color::Green);
    auto x2 = particle_2->x * scale;
    auto y2 = particle_2->y * scale;
    shape2.setPosition(x2 + offset_x, y2 + offset_y);
    window->draw(shape2);
    window->display();
}

void printstep(double t, struct Particle particle_1, struct Particle particle_2, std::ofstream *outfile)
{

    double x1 = particle_1.x;
    double y1 = particle_1.y;
    double x2 = particle_2.x;
    double y2 = particle_2.y;

    *outfile << t << "," << x1 << "," << y1;
    *outfile << "," << x2 << "," << y2 << std::endl;
}