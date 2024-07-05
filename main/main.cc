#include "lib/tile.h"
#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"

#include <iostream>
#include <string>
#include <vector>
#include <math.h>

int main(int argc, char **argv)
{
    // Display size.
    int display_width = sf::VideoMode::getDesktopMode().width;
    int display_height = sf::VideoMode::getDesktopMode().height;
    sf::RenderWindow window(sf::VideoMode(display_width, display_height), "Test Window", sf::Style::Fullscreen);

    const float tile_size = 100;    
    const float line_thickness = 2;
    const int number_of_tile_per_line = 20;
    const sf::Vector2f center_offset = {display_width/ 2 - (tile_size*number_of_tile_per_line)/2, display_height/2 - (tile_size*number_of_tile_per_line)/2};

    sf::RectangleShape* map[number_of_tile_per_line][number_of_tile_per_line];
    for(int i = 0; i < number_of_tile_per_line; i++){
        for(int j = 0; j < number_of_tile_per_line; j++){
            map[i][j] = new sf::RectangleShape;
            map[i][j]->setSize({tile_size, tile_size});
            map[i][j]->setOutlineThickness(line_thickness);
            map[i][j]->setOutlineColor(sf::Color::White);
            map[i][j]->setFillColor(sf::Color::Transparent);
            map[i][j]->setPosition({(tile_size + line_thickness)*i + center_offset.x, (tile_size + line_thickness)*j + center_offset.y});            
        }    
    }

    sf::CircleShape circle;
    circle.setRadius(50);
    circle.setPosition(map[10][10]->getPosition());
 
    while (window.isOpen())
    {
        window.clear();
        // Check close event.
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed || event.key.code == sf::Keyboard::Escape)
                window.close();
          
        }
        for(int i = 0; i < 20; i++){
            for(int j = 0; j < 20; j++){
                window.draw(*map[i][j]);
            }    
        }

        window.draw(circle);

        window.display();
    }
}