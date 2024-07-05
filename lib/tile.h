#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"

class Tile {
    public:
    Tile(int display_width, int display_height, float tile_size = 100, int number_of_tile_per_line = 20)
     : tile_size_(tile_size), number_of_tile_per_line_(number_of_tile_per_line)
     {
        const float line_thickness = 2;
        center_offset_ = {display_width/ 2 - (tile_size*number_of_tile_per_line)/2, display_height/2 - (tile_size*number_of_tile_per_line)/2};
        sf::RectangleShape* map[number_of_tile_per_line][number_of_tile_per_line];
        for(int i = 0; i < number_of_tile_per_line; i++){
            for(int j = 0; j < number_of_tile_per_line; j++){
                map[i][j] = new sf::RectangleShape;
                map[i][j]->setSize({tile_size, tile_size});
                map[i][j]->setOutlineThickness(line_thickness);
                map[i][j]->setOutlineColor(sf::Color::White);
                map[i][j]->setFillColor(sf::Color::Transparent);
                map[i][j]->setPosition({(tile_size + line_thickness)*i + center_offset_.x, (tile_size + line_thickness)*j + center_offset_.y});            
            }    
        }

        map_ = *map;
        delete[] map;
     }

    sf::RectangleShape** GetTileMap(){
        return map_;
    }

    ~Tile(){
        delete[] map_;
    }

    private:
    float tile_size_;    
    int number_of_tile_per_line_;
    sf::Vector2f center_offset_;
    sf::RectangleShape** map_;
};