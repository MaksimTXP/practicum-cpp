#pragma once

#include <string>
#include <chrono>
#include <vector>

namespace db_domain {

  class PlayerRecord {
    
  public:

    PlayerRecord(std::string name, size_t score, std::chrono::milliseconds play_time) :
      name_(name),
      score_(score),
      play_time_(play_time) {}

    const std::string GetName() const noexcept { 
      return name_; 
    }

    const size_t GetScore() const noexcept { 
      return score_; 
    }

    const std::chrono::milliseconds GetPlayTime() const noexcept {
      return play_time_;
    }

  private:

    std::string name_;
    size_t score_;
    std::chrono::milliseconds play_time_;
  
  };

  class PlayerRecordRepository {
   
  public:

    virtual void Save(const PlayerRecord& record) = 0;
    virtual void SavePlayersRecords(const std::vector<PlayerRecord>& players_records) = 0;
    virtual std::vector<PlayerRecord> GetPlayersRecords(size_t offset, size_t limit) = 0;

  protected:

    ~PlayerRecordRepository() = default;

  };


}