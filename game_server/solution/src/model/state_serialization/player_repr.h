#pragma once 

#include "game_session_repr.h"
#include "../../application/player.h"
#include "../../application/player_tokens.h"

namespace state_serialization {


  class PlayerRepr {
  public:

    PlayerRepr() = default;

    explicit PlayerRepr(const application::Player& player, const application::Token& token) :
      id_(*player.GetId()),
      map_id_(*player.GetGameSession()->GetMap()->GetId()),
      name_(player.GetName()),
      dog_(*player.GetDog()),
      token_(*token) 
    {
    }


    [[nodiscard]] application::Player ToPlayer() const {

      return application::Player(application::Player::Id(id_), name_);
    
    }

    [[nodiscard]] model::Map::Id ToMapId() const {

      return model::Map::Id(map_id_);
    }

    [[nodiscard]] model::Dog ToDog() const {
      return dog_.ToDog();
    }

    [[nodiscard]] application::Token ToToken() const {
    
      return application::Token(token_);
    }


    template<typename Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned version) {
      ar& id_;
      ar& map_id_;
      ar& name_;
      ar& dog_;
      ar& token_;
    
    }



  private:
    size_t id_;
    std::string map_id_;
    std::string name_;
    DogRepr dog_;
    std::string token_;
  
  };

}