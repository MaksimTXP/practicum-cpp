#pragma once


#include <string>

#include "../model/game_session.h"


namespace application {

class Player {

  inline static size_t max_id_cont_ = 0;

public:

  using Id = util::Tagged<size_t, class PlayerTag>;

  explicit Player(const std::string& name) :
    id_(Id{ Player::max_id_cont_++ }),
    name_(name) {};

  Player(Id id, const std::string& name) :
    id_(id),
    name_(name) {};

  Player(const Player&) = default;
  Player& operator = (Player&) = default;

  const Id& GetId() const {
    return id_;
  }

  const std::string& GetName() const {
    return name_;
  }

  const model::GameSession::Id& GetGameSessionId() const {
    return session_->GetId();
  }

  void SetGameSession(std::shared_ptr<model::GameSession> session) {
    session_ = session;
  }

 const std::shared_ptr<model::GameSession> GetGameSession() const {
    return session_;
  }
  void SetDog(std::shared_ptr<model::Dog> dog) {
    dog_ = dog;
  }

  std::shared_ptr<model::Dog> GetDog() const {
    return dog_;
  }


private:

  Id id_;
  std::string name_;
  std::shared_ptr<model::GameSession> session_ = nullptr;
  std::shared_ptr<model::Dog> dog_ = nullptr;
};
}

