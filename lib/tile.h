#include "lib/SFML-2.5.1/include/SFML/Graphics.hpp"
#include <map>
#include <unordered_map>
#include <stdexcept>
#include <iostream>

namespace lib::tile{

namespace{
	constexpr auto opacity = 120;
	struct color_hash {
    std::size_t operator () (const sf::Color& color) const {
        return std::hash<int>()(color.r) ^ std::hash<int>()(color.g) ^ std::hash<int>()(color.b) ^ std::hash<int>()(color.a);
    }
	};

	// TODO: Add more color for more agents.
	// To distinguish agent and cleaned tile.
	std::unordered_map<sf::Color, sf::Color, color_hash> color_tile_grid_for_cleaned_tile{
		{sf::Color::Red, sf::Color(200, 0, 0, opacity)},
		{sf::Color::Green, sf::Color(0, 200, 0, opacity)},
		{sf::Color::Blue, sf::Color(0, 0, 200, opacity)}
	}; 
}	

class Tile {
    public:
    Tile(int display_width, int display_height, int number_of_tile_per_line = 20)
     : number_of_tile_per_line_(number_of_tile_per_line)
     {
				// GUI setup.
        const float line_thickness = 2;
				const float tile_size = 100;

        center_offset_ = {display_width/ 2 - (tile_size*number_of_tile_per_line)/2, display_height/2 - (tile_size*number_of_tile_per_line)/2};

        for(int i = 0; i < number_of_tile_per_line; i++){
            std::vector<sf::RectangleShape*> row;
            for(int j = 0; j < number_of_tile_per_line; j++){
              sf::RectangleShape* tile = new sf::RectangleShape;
              tile->setSize({tile_size, tile_size});
              tile->setOutlineThickness(line_thickness);
              tile->setOutlineColor(sf::Color::White);
              tile->setFillColor(sf::Color::Transparent);
              tile->setPosition({(tile_size + line_thickness)*i + center_offset_.x, (tile_size + line_thickness)*j + center_offset_.y});
							row.push_back(tile);

							// Setup the cleaned_map.
							cleaned_tile_grid_[std::make_pair(i, j)];
            }
						tile_grid_.push_back(row);
        }
     }

    std::vector<std::vector<sf::RectangleShape*>> GetTileVector(){
        return tile_grid_;
    }

		sf::RectangleShape* GetTile(const std::pair<int,int>& location){
			return tile_grid_[location.first][location.second];
		}

		sf::Vector2f GetTilePixelPosition(const std::pair<int,int>& location){
			return tile_grid_[location.first][location.second]->getPosition();
		}

		void ClearCleanedTileState(){
			for(auto& tile : cleaned_tile_grid_){
				tile.second = false;
			}
			for(auto& row : tile_grid_){
				for(auto& tile : row){
					tile->setFillColor(sf::Color::Transparent);
				}
			}
		}

		void UpdateCleanedTile(const std::pair<int,int>& location, const sf::Color& color){
			try{
				cleaned_tile_grid_[location] = true;
				// Only update the tile color when it is transparent.
				if(tile_grid_[location.first][location.second]->getFillColor() == sf::Color::Transparent){
					tile_grid_[location.first][location.second]->setFillColor(color_tile_grid_for_cleaned_tile[color]);
				}
				

			} catch (const std::exception& e) {
        // print the exception
        std::cout << "Exception " << e.what() << std::endl;
    	}
		}

    ~Tile(){
			for(auto row: tile_grid_){
				for(auto each: row){
					delete each;
				}
			}
    }

    private:
		std::map<std::pair<int, int>, bool> cleaned_tile_grid_;
    int number_of_tile_per_line_;
    sf::Vector2f center_offset_;
    std::vector<std::vector<sf::RectangleShape*>> tile_grid_;
};
}