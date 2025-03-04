#pragma once 

#include "../domain/player_record.h"

namespace db_app {
  class UseCases {
  public:

    
    virtual void SavePlayersRecords(const std::vector<db_domain::PlayerRecord>& players_record) = 0;
    
    virtual std::vector<db_domain::PlayerRecord> GetPlayersRecords(size_t offset, size_t limit) = 0;


  protected:

    ~UseCases() = default;
  };

}