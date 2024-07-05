#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include <memory>

namespace lib::agent{
class Agent{
    public:
    Agent(int id, sf::Vector2f vector2f, std::pair<int, int> tile_grid, float radius = 50) : 
    id_(id),
    current_position_pixel_(vector2f),
    current_tile_grid_location_(tile_grid),
    circle_radius_(radius)
    {   
        const float outline_thickness = 1;
        circle = std::make_shared<sf::CircleShape>();
        circle->setRadius(radius); 
        circle->setPosition(vector2f);
        circle->setOutlineColor(sf::Color::White);
        circle->setOutlineThickness(outline_thickness);

        current_tile_grid_location_ = tile_grid;
    }

    int GetID(){
        return id_;
    }
    
    std::pair<int, int> GetCurrentTileGridLocation(){
        return current_tile_grid_location_;
    }
    
    void UpdateTileGridLocation(const std::pair<int,int>& location){
        current_tile_grid_location_ = location;
    }

    void UpdatePixelLocation(const sf::Vector2f& location){
        circle->setPosition(location);
    }

    // Make shared_ptr and public to easily modfiy the position.
    std::shared_ptr<sf::CircleShape> circle;

    private:
    int id_;
    std::pair<int, int> current_tile_grid_location_;    
    sf::Vector2f current_position_pixel_;
    float circle_radius_;
    float circle_position_;
};
}