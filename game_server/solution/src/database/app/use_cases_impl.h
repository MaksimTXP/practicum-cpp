#pragma once

#include "use_cases.h"
#include "unit_of_work.h"

namespace db_app {

  class UseCasesImpl : public UseCases {
  public:

    explicit UseCasesImpl(UnitOfWorkFactory& unit_of_work_factory)
      : unit_of_work_factory_(unit_of_work_factory) {}



    void SavePlayersRecords(const std::vector<db_domain::PlayerRecord>& players_record) override {
      auto work = unit_of_work_factory_.CreateUnitOfWork();

      work->PlayerRecords().SavePlayersRecords(players_record);
      work->Commit();
    
    }

    std::vector<db_domain::PlayerRecord> GetPlayersRecords(size_t offset, size_t limit) override {

      auto work = unit_of_work_factory_.CreateUnitOfWork();

      return work->PlayerRecords().GetPlayersRecords(offset, limit);

    }

    


  private:

    UnitOfWorkFactory& unit_of_work_factory_;
  };


}